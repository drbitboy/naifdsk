
### ./alpha_dsk_c/ is a directory or a symlink e.g.
###
###   ln -s $(HOME)/alpha_dsk_c ./alpha_dsk_c
###
### to NAIF/SPICE DSK-capable CSPICE; c.f. http://naif.jpl.nasa.gov/

LDLIBS=alpha_dsk_c/lib/dsklib_c.a alpha_dsk_c/lib/cspice.a -lm
CPPFLAGS=-Ialpha_dsk_c/src/dsklib_c/ -Ialpha_dsk_c/include

MODELS=churyumov-gerasimenko.mdl churyumov-gerasimenko.wrl

all: dsk2mdlinsert

chuger: $(MODELS)

churyumov-gerasimenko.mdl: dsk2mdlinsert CSHP_DV_001_01_______00001.BDS
	./$^ > $@

churyumov-gerasimenko.wrl: dsk2mdlinsert CSHP_DV_001_01_______00001.BDS
	./$^ > $@ --wrl

clean:
	$(RM) dsk2mdlinsert $(MODELS)
