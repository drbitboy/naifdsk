naifdsk
=======

Misc tools for working with NAIF/SPICE DS-Kernels (shape models)

- see [http://naif.jpl.nasa.gov/pub/naif/misc/alpha_dsk/] and
  [http://naif.jpl.nasa.gov/pub/naif/misc/alpha_dsk/DSK_alpha_test.pdf]


Manifest
========


dsk2mslinsert.c

- Converts DSK to STK .MDL file, or to VRML .WRL file


Makefile

- simple makefile; needs directory or symlink path to SPICE toolkit
  alpha_dsk_c/


test_dskv02.c

- Implementation of sample code from comments in SPICE/DSK toolkit 
  subroutine dskv02_c.c


