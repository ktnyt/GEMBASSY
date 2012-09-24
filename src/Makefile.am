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

bin_PROGRAMS = gaaui\
               gamino_info\
               gb1\
               gb2\
               gbase_counter\
               gbase_entropy\
               gbase_information_content\
               gbase_relative_entropy\
               gbase_z_value\
               gbui\
               gcai\
               gcbi\
               gcgr\
               gcircular_map\
               gdelta_enc\
               gdelta_gcskew\
               gdist_in_cc\
               genc\
               gentrez\
               gew\
               gfind_ori_ter\
               gfop\
               ggcsi\
               ggcskew\
               ggcwin\
               ggeneskew\
               ggenomicskew\
               gicdi\
               gkmer_table\
               glda_bias\
               gleading_strand\
               gnucleotide_periodicity\
               goligomer_counter\
               gp2\
               gphx\
               gquery_arm\
               gquery_strand\
               grep_ori_ter\
               gseq2png\
               gseqinfo\
               gshuffleseq\
               gview_cds\
               gw_value

gaaui_SOURCES           = gaaui.c
gamino_info_SOURCES     = gamino_info.c
gb1_SOURCES             = gb1.c
gb2_SOURCES             = gb2.c
gbase_counter_SOURCES   = gbase_counter.c
gbase_entropy_SOURCES   = gbase_entropy.c
gbase_information_content_SOURCES = gbase_information_content.c
gbase_relative_entropy_SOURCES    = gbase_relative_entropy.c
gbase_z_value_SOURCES   = gbase_z_value.c
gbui_SOURCES            = gbui.c
gcai_SOURCES            = gcai.c
gcbi_SOURCES            = gcbi.c
gcgr_SOURCES            = gcgr.c
gcircular_map_SOURCES   = gcircular_map.c
gdelta_enc_SOURCES      = gdelta_enc.c
gdelta_gcskew_SOURCES   = gdelta_gcskew.c
gdist_in_cc_SOURCES     = gdist_in_cc.c
genc_SOURCES            = genc.c
gentrez_SOURCES         = gentrez.c
gew_SOURCES             = gew.c
gfind_ori_ter_SOURCES   = gfind_ori_ter.c
gfop_SOURCES            = gfop.c
ggcsi_SOURCES           = ggcsi.c
ggcwin_SOURCES          = ggcwin.c
ggeneskew_SOURCES       = ggeneskew.c
ggenomicskew_SOURCES    = ggenomicskew.c
gicdi_SOURCES           = gicdi.c
gkmer_table_SOURCES     = gkmer_table.c
glda_bias_SOURCES       = glda_bias.c
gleading_strand_SOURCES = gleading_strand.c
gnucleotide_periodicity_SOURCES = gnucleotide_periodicity.c
goligomer_counter_SOURCES       = goligomer_counter.c
gphx_SOURCES            = gphx.c
gp2_SOURCES             = gp2.c
gquery_arm_SOURCES      = gquery_arm.c
gquery_strand_SOURCES   = gquery_strand.c
grep_ori_ter_SOURCES    = grep_ori_ter.c
gseqinfo_SOURCES        = gseqinfo.c
gseq2png_SOURCES        = gseq2png.c
gshuffleseq_SOURCES     = gshuffleseq.c
gview_cds_SOURCES       = gview_cds.c
gw_value_SOURCES        = gw_value.c

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

