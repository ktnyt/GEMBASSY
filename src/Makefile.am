## Process this file with automake to produce Makefile.in

if LOCALLINK
INCLUDES = -I../include -I../../../nucleus -I../../../ajax/pcre \
	-I../../../ajax/expat -I../../../ajax/zlib \
	-I../../../ajax/core -I../../../ajax/graphics \
	-I../../../ajax/ensembl -I../../../ajax/ajaxdb \
	-I../../../ajax/acd -I../../../plplot -I../gsoap
else
INCLUDES = -I../include -I${embprefix}/include -I${embprefix}/include/eplplot -I${embprefix}/include/epcre -I${embprefix}/include/ezlib -I../gsoap
endif

if ISSHARED
if ISAIXIA64
if LOCALLINK
AIX_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L../../../plplot/.libs \
-L../../../ajax/pcre/.libs -L../../../ajax/expat/.libs \
-L../../../ajax/zlib/.libs -L../../../ajax/core/.libs \
-L../../../ajax/graphics/.libs -L../../../ajax/ensembl/.libs \
-L../../../ajax/ajaxdb/.libs -L../../../ajax/acd/.libs \
-L../../../nucleus/.libs \
-lnucleus -lacd -lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre \
-lezlib -leplplot
else
AIX_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lacd \
-lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre -lezlib -leplplot
endif
endif
endif

AM_CFLAGS =  $(AIX_CFLAGS) $(WARN_CFLAGS) $(DEVWARN_CFLAGS)

## To add programs
## Add the program binary name to bin_PROGRAMS
## (using \ as a continuation character for multiple lines)
##
## And add a programname_SOURCES line to define the source files
## to be compiled and linked
##
## make will compile and link the program
## make install will copy the program to the install directory

bin_PROGRAMS = gaminoinfo\
               gbaseinfocontent\
               gbui\
               gcai\
               gcbi\
               gcgr\
               gdnawalk\
               gfindoriter\
               ggcsi\
               ggcskew\
               ggcwin\
               ggeneskew\
               ggenomicskew\
               gicdi\
               gphx\
               gqueryarm\
               gquerystrand\
               greporiter\
               gseq2png\
               gseqinfo\
               gshuffleseq

gaminoinfo_SOURCES       = gaminoinfo.c
gbaseinfocontent_SOURCES = gbaseinfocontent.c
gbui_SOURCES             = gbui.c
gcai_SOURCES             = gcai.c
gcbi_SOURCES             = gcbi.c
gcgr_SOURCES             = gcgr.c
gdnawalk_SOURCES         = gdnawalk.c
gfindoriter_SOURCES      = gfindoriter.c
ggcsi_SOURCES            = ggcsi.c
ggcwin_SOURCES           = ggcwin.c
ggeneskew_SOURCES        = ggeneskew.c
ggenomicskew_SOURCES     = ggenomicskew.c
gicdi_SOURCES            = gicdi.c
gphx_SOURCES             = gphx.c
gqueryarm_SOURCES        = gqueryarm.c
gquerystrand_SOURCES     = gquerystrand.c
greporiter_SOURCES       = greporiter.c
gseqinfo_SOURCES         = gseqinfo.c
gseq2png_SOURCES         = gseq2png.c
gshuffleseq_SOURCES      = gshuffleseq.c

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/acd/libacd.la \
	../../../ajax/ajaxdb/libajaxdb.la \
	../../../ajax/ensembl/libensembl.la \
	../../../ajax/graphics/libajaxg.la \
	../../../ajax/core/libajax.la \
	../../../ajax/zlib/libezlib.la \
	../../../ajax/expat/libeexpat.la \
	../../../ajax/pcre/libepcre.la \
	../../../plplot/libeplplot.la $(XLIB)
else
LDADD = -L${embprefix}/lib -lnucleus -lacd -lajaxdb -lensembl -lajaxg \
        -lajax -leexpat -lepcre -lezlib -leplplot -lcurl $(XLIB)
endif

