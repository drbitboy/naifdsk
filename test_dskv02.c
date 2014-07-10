#include <stdio.h>
#include "SpiceUsr.h"
#include "SpiceDLA.h"
#include "dsk_proto.h"

int main()
{   
   /*
   Constants 
   */
   #define PBUFSIZ         10000
   #define FILSIZ          256

   /*
   Local variables 
   */
   SpiceBoolean            found;

   SpiceChar               dsk  [ FILSIZ ];

   SpiceDLADescr           dladsc;

   SpiceDouble             normal [3];
   SpiceDouble             verts  [3][3];

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

   /*
   Prompt for name of DSK and open file for reading.
   */
   prompt_c ( "Enter DSK name > ", FILSIZ, dsk );

   dasopr_c ( dsk,    &handle );

   dlabfs_c ( handle, &dladsc, &found );

   if ( !found )
   { 
      setmsg_c ( "No segment found in file #." );
      errch_c  ( "#",  dsk                     );
      sigerr_c ( "SPICE(NOSEGMENT)"            );
   }

   /*
   Get segment vertex and plate counts.
   */
   dskz02_c ( handle, &dladsc, &nv, &np );

   printf ( "\n" 
            "Number of vertices:  %d\n"
            "Number of plates:    %d\n",
            nv,
            np                           );

   /*
   Display the vertices of each plate.
   */
   remain = np;
   start  = 1;

   while ( remain > 0 )
   { 
      /*
      `nread' is the number of plates we"ll read on this
      loop pass.
      */
      nread  = mini_c ( 2, PBUFSIZ, remain );

      dskp02_c ( handle, &dladsc, start, nread, &n, plates );

      for ( i = 0; i < nread; i++ )
      {
         plix = start + i;

         /*
         Read the vertices of the current plate.
         */
         for ( j = 0;  j < 3;  j++ )
         {         
            dskv02_c ( handle, &dladsc, plates[i][j],  
                       1,      &nvtx,   
                       ( SpiceDouble(*)[3] )(verts[j])  );
         }

         /*
         Display the vertices of the current plate:
         */
         printf ( "\n" 
                  " Plate number: %d\n"
                  "    Vertex 1: ( %16.8e %16.8e %16.8e )\n"
                  "    Vertex 2: ( %16.8e %16.8e %16.8e )\n" 
                  "    Vertex 3: ( %16.8e %16.8e %16.8e )\n",
                  plix,
                  verts[0][0],  verts[0][1],  verts[0][2], 
                  verts[1][0],  verts[1][1],  verts[1][2], 
                  verts[2][0],  verts[2][1],  verts[2][2]   );

         /*
         Display the normal vector of the current plate:
         */
         dskn02_c ( handle, &dladsc, plix, normal );

         printf( "    Normal:   ( %16.8e %16.8e %16.8e )\n",
                 normal[0], normal[1], normal[2]           );
      }

      start  = start  + nread;
      remain = remain - nread;
   }

   /*
   Close the kernel.  This isn't necessary in a stand-
   alone program, but it's good practice in subroutines
   because it frees program and system resources.
   */
   dascls_c ( handle );

   return ( 0 );
}
