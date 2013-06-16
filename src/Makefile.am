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
-lezlib -leplplot -lcurl
else
AIX_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lacd \
-lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre -lezlib -leplplot -lcurl
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

bin_PROGRAMS = genret \
               gaaui\
               gaminoinfo\
               gb1\
               gb2\
               gbasecounter\
               gbaseentropy\
               gbaseinformationcontent\
               gbaserelativeentropy\
               gbasezvalue\
	       gbui\
               gcai\
               gcbi\
               gcgr\
               gcircularmap\
               gcodoncompiler\
               gconsensusz\
               gdeltaenc\
               gdeltagcskew\
               gdinuc\
               gdistincc\
               gdnawalk\
               genc\
               gentrez\
               gew\
               gfindoriter\
               gfop\
               ggcsi\
               ggcskew\
               ggcwin\
               ggeneskew\
               ggenomicskew\
               ggenomemap3\
               gicdi\
               gkmertable\
               gldabias\
               gnucleotideperiodicity\
	       goligomercounter\
	       goligomersearch\
               gp2\
               gpalindrome\
               gphx\
               gqueryarm\
               gquerystrand\
               greporiter\
               gsvalue\
               gscs\
               gseq2png\
               gseqinfo\
               gshuffleseq\
               gsignature\
               gviewcds\
               gwvalue

genret_SOURCES                  = genret.c
gaaui_SOURCES                   = gaaui.c
gaminoinfo_SOURCES              = gaminoinfo.c
gb1_SOURCES                     = gb1.c
gb2_SOURCES                     = gb2.c
gbasecounter_SOURCES            = gbasecounter.c
gbaseentropy_SOURCES            = gbaseentropy.c
gbaseinformationcontent_SOURCES = gbaseinformationcontent.c
gbaserelativeentropy_SOURCES    = gbaserelativeentropy.c
gbasezvalue_SOURCES             = gbasezvalue.c
gbui_SOURCES                    = gbui.c
gcai_SOURCES                    = gcai.c
gcbi_SOURCES                    = gcbi.c
gcgr_SOURCES                    = gcgr.c
gcircularmap_SOURCES            = gcircularmap.c
gcodoncompiler_SOURCES          = gcodoncompiler.c
gconsensusz_SOURCES             = gconsensusz.c
gdeltaenc_SOURCES               = gdeltaenc.c
gdeltagcskew_SOURCES            = gdeltagcskew.c
gdistincc_SOURCES               = gdistincc.c
gdinuc_SOURCES                  = gdinuc.c
gdnawalk_SOURCES                = gdnawalk.c
genc_SOURCES                    = genc.c
gentrez_SOURCES                 = gentrez.c
gew_SOURCES                     = gew.c
gfindoriter_SOURCES             = gfindoriter.c
gfop_SOURCES                    = gfop.c
ggcsi_SOURCES                   = ggcsi.c
ggcwin_SOURCES                  = ggcwin.c
ggeneskew_SOURCES               = ggeneskew.c
ggenomicskew_SOURCES            = ggenomicskew.c
ggenomemap3_SOURCES             = ggenomemap3.c
gicdi_SOURCES                   = gicdi.c
gkmertable_SOURCES              = gkmertable.c
gldabias_SOURCES                = gldabias.c
gnucleotideperiodicity_SOURCES  = gnucleotideperiodicity.c
goligomercounter_SOURCES        = goligomercounter.c
goligomersearch_SOURCES         = goligomersearch.c
gp2_SOURCES                     = gp2.c
gpalindrome_SOURCES             = gpalindrome.c
gphx_SOURCES                    = gphx.c
gqueryarm_SOURCES               = gqueryarm.c
gquerystrand_SOURCES            = gquerystrand.c
greporiter_SOURCES              = greporiter.c
gsvalue_SOURCES                 = gsvalue.c
gscs_SOURCES                    = gscs.c
gseq2png_SOURCES                = gseq2png.c
gseqinfo_SOURCES                = gseqinfo.c
gshuffleseq_SOURCES             = gshuffleseq.c
gsignature_SOURCES              = gsignature.c
gviewcds_SOURCES                = gviewcds.c
gwvalue_SOURCES                 = gwvalue.c

if LOCALLINK
LDADD = ../../../nucleus/libnucleus.la ../../../ajax/acd/libacd.la \
	../../../ajax/ajaxdb/libajaxdb.la \
	../../../ajax/ensembl/libensembl.la \
	../../../ajax/graphics/libajaxg.la \
	../../../ajax/core/libajax.la \
	../../../ajax/zlib/libezlib.la \
	../../../ajax/expat/libeexpat.la \
	../../../ajax/pcre/libepcre.la \
	../../../plplot/libeplplot.la \
        ../include/libgfile.a \
        ../include/libghttp.a \
        ../include/libgplot.a \
        ../include/libgpost.a \
        -lcurl \
        $(XLIB)
else
LDADD = -L${embprefix}/lib -L../include -lnucleus -lacd \
        -lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre -lezlib -leplplot \
        -lcurl $(XLIB)
endif
