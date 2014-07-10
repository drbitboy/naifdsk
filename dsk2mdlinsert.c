/*
 * Reads NAIF/SPICE DS-Kernel (shape model), write STK .MDL or VRML .WRL file.
 *
 * Usage:
 *
 *   Write STK MDL file:
 *
 *     ./dsk2mdlinsert CSHP_DV_001_01_______00001.BDS > churyumov-gerasimenko.mdl
 *
 *   Write VRML .WRL file
 *
 *     ./dsk2mdlinsert CSHP_DV_001_01_______00001.BDS   --wrl  > churyumov-gerasimenko.wrl 
 *
 * Based on NAIF/SPICE sample code in dskv02_c.c
 *
 */
#include <stdio.h>
#include <string.h>
#include "SpiceUsr.h"
#include "SpiceDLA.h"
#include "dsk_proto.h"

int main(int ac, char** av)
{   
   /* Constants */
   #define PBUFSIZ         10000
   #define FILSIZ          256

   /* Local variables */
   SpiceBoolean            found;

   SpiceChar               dsk  [ FILSIZ ];

   SpiceDLADescr           dladsc;

   SpiceDouble             normal [3];
   SpiceDouble             verts  [3][3];
   SpiceDouble             BoundRadius;

   SpiceInt                handle;
   SpiceInt                i;
   SpiceInt                j;
   SpiceInt                n;
   SpiceInt                np;
   SpiceInt                nread;
   SpiceInt                nv;
   SpiceInt                nvtx;
   SpiceInt                plates[PBUFSIZ][3];
   SpiceInt                plix;
   SpiceInt                remain;
   SpiceInt                start;
   int iAc;
   int doMdl = 1;

   /* Prompt for name of DSK and open file for reading.  */
   *dsk = '\0';
   for (iAc=1; iAc<ac; ++iAc) {
     if ( strcmp(av[iAc], "--wrl") ) {
       strncpy(dsk,av[1],FILSIZ);
       continue;
     } else {
       doMdl = 0;
     }
   }
   if ( ! *dsk) {
     if ( doMdl ) {
       prompt_c ( "### Enter DSK (*.bds) filepath > ", FILSIZ, dsk );
     } else {
       fprintf( stderr, "### Enter DSK (*.bds) filepath > " );
       prompt_c ( "", FILSIZ, dsk );
     }
   }

   dasopr_c ( dsk,    &handle );

   dlabfs_c ( handle, &dladsc, &found );

   if ( !found )
   { 
      setmsg_c ( "No segment found in file #." );
      errch_c  ( "#",  dsk                     );
      sigerr_c ( "SPICE(NOSEGMENT)"            );
   }

   /* Get segment vertex and plate counts.  */
   dskz02_c ( handle, &dladsc, &nv, &np );

   /*******************************************************************/
   if ( doMdl ) {
     printf( "\
### File:  %s\n\
Component DSK\n\
  PolygonMesh\n\
    FaceColor %%255255255\n\
    SmoothShading No\n\
    BackfaceCullable Yes\n\
    Translucency 0.5\n\
    Specularity 0.59375000\n\
    Shininess 76.000000\n\
\n\
    NumVerts %d\n\
    Data\n", dsk, nv );
   } else {

     printf( "\
#VRML V2.0 utf8\n\
### File:  %s\n\
Shape {\n\
  geometry IndexedFaceSet {\n\
    ccw TRUE  ### East longitude\n\
    coord Coordinate {\n", dsk );
   }

   BoundRadius = 0.0;

   /* Get vertices' coordinates */
   for (i=1; i<=nv; ++i) {
   SpiceDouble br;

     dskv02_c ( handle, &dladsc, i, 1
              , &nvtx, ( SpiceDouble(*)[3] )(verts[0])
              );

     /*****************************************************************/
     printf( doMdl ? "      "
                   : ( i<2 ? "      point [ " 
                           : "            , " 
                     )
           );
     printf( "%.12g %.12g %.12g\n", verts[0][0], verts[0][1], verts[0][2] );

     br = vnorm_c(verts[0]);
     if ( br > BoundRadius ) BoundRadius = br;
   }

   /*******************************************************************/
   if ( doMdl ) {
     printf( "    NumPolys %d\n", np );
     printf( "    Polys\n" );
   } else {
     printf( "            ]\n" );
     printf( "    }\n" );
   }

   /* Display the vertices of each plate.  */
   remain = np;
   start  = 1;

   while ( remain > 0 )
   { 
      /* `nread' is the number of plates we"ll read on this loop pass.  */
      nread  = mini_c ( 2, PBUFSIZ, remain );

      dskp02_c ( handle, &dladsc, start, nread, &n, plates );

      for ( i = 0; i < nread; i++ )
      {
         plix = start + i;

         /* Write the vertices of the current plate.  */
         printf( doMdl ? "     3"
                       : (i==0 ? "    coordIndex ["
                               : "               ,"
                         )
               );
         for ( j = 0;  j < 3;  j++ ) printf( " %d", plates[i][j]-1);
         printf( "%s\n", doMdl ? "" : " -1" );

      }

      start  = start  + nread;
      remain = remain - nread;
   }

   /* Close the kernel.  This isn't necessary in a stand-alone program,
      but it's good practice in subroutines because it frees program
      and system resources.  */
   dascls_c ( handle );

   /*******************************************************************/
   if ( doMdl ) {
     printf( "  EndPolygonMesh\n" );
     printf( "EndComponent\n\
#\n\
Component ParentLayer1_DSK\n\
	Refer\n\
		Component DSK\n\
	EndRefer\n\
EndComponent\n\
#\n\
# Main body:\n\
Component DSK_ROOT\n\
	Root\n\
	Refer\n\
		Articulation Body\n\
			uniformScale Size 0 1 1\n\
			xRotate Roll -360 0 360\n\
			yRotate Pitch -360 0 360\n\
			zRotate Yaw -360 0 360\n\
			xTranslate MoveX -1000 0 1000\n\
			yTranslate MoveY -1000 0 1000\n\
			zTranslate MoveZ -1000 0 1000\n\
		EndArticulation\n\
		Component ParentLayer1_DSK\n\
	EndRefer\n\
EndComponent\n\
BoundRadius %.12g\n", BoundRadius);

   } else {
     printf( "\
               ]\n\
  }\n\
  appearance Appearance {\n\
    material Material {\n\
      diffuseColor .25 .25 .25\n\
      emissiveColor .25 .25 .25\n\
    }\n\
  }\n\
}\n" );
   }
   return 0;
}
