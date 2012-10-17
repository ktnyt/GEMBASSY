common_ldflags = -avoid-version -module

# The modules for loading/saving the various graphics formats
# are located in $(moduledir).  Default it to libdir for backwards
# compatibility.
#
moduledir = $(libdir)

libimlib_gif_la_SOURCES = io-gif.c
libimlib_gif_la_LDFLAGS = $(common_ldflags)
libimlib_gif_la_LIBADD  = $(GIFLIBS) libgdk_imlib.la

libimlib_jpeg_la_SOURCES = io-jpeg.c
libimlib_jpeg_la_LDFLAGS = $(common_ldflags)
libimlib_jpeg_la_LIBADD  = $(JPEGLIBS) libgdk_imlib.la

libimlib_png_la_SOURCES = io-png.c
libimlib_png_la_LDFLAGS = $(common_ldflags)
libimlib_png_la_LIBADD  = $(PNGLIBS) libgdk_imlib.la

libimlib_tiff_la_SOURCES = io-tiff.c
libimlib_tiff_la_LDFLAGS = $(common_ldflags)
libimlib_tiff_la_LIBADD  = $(TIFFLIBS) libgdk_imlib.la

libimlib_ps_la_SOURCES = io-ps.c
libimlib_ps_la_LDFLAGS = $(common_ldflags)
libimlib_ps_la_LIBADD = libgdk_imlib.la

libimlib_bmp_la_SOURCES = io-bmp.c
libimlib_bmp_la_LDFLAGS = $(common_ldflags)
libimlib_bmp_la_LIBADD = libgdk_imlib.la

libimlib_xpm_la_SOURCES = io-xpm.c
libimlib_xpm_la_LDFLAGS = $(common_ldflags)
libimlib_xpm_la_LIBADD = libgdk_imlib.la

libimlib_ppm_la_SOURCES = io-ppm.c
libimlib_ppm_la_LDFLAGS = $(common_ldflags)
libimlib_ppm_la_LIBADD = libgdk_imlib.la

if DYNAMIC_MODULES_WORK

if HAVE_GIF
GIF = libimlib-gif.la
endif

if HAVE_JPEG
JPEG = libimlib-jpeg.la
endif

if HAVE_PNG
PNG = libimlib-png.la
endif

if HAVE_TIFF
TIFF = libimlib-tiff.la
endif

OTHER = \
	libimlib-bmp.la		\
	libimlib-xpm.la		\
	libimlib-ppm.la		\
	libimlib-ps.la

DYNAMIC_LIBS = \
	$(OTHER) $(GIF) $(JPEG) $(TIFF) $(PNG)

else

DYNAMIC_LIBS =

endif

lib_LTLIBRARIES = libgdk_imlib.la
module_LTLIBRARIES = $(DYNAMIC_LIBS)

libgdk_imlib_la_SOURCES =	\
	cache.c      		\
	colors.c     		\
	globals.c    		\
	load.c       		\
	misc.c       		\
	rend.c       		\
	utils.c      		\
	save.c			\
	modules.c


libgdk_imlib_la_LDFLAGS = -version-info 10:15:9
libgdk_imlib_la_LIBADD = $(GX_LIBS)


include_HEADERS = gdk_imlib.h gdk_imlib_private.h gdk_imlib_types.h



DEFS =  -DSYSTEM_IMRC=\"$(sysconfdir)/imrc\" \
	-DIMLIB_LIB=\""$(moduledir)"\" -DSYSCONFDIR=\"$(sysconfdir)\"

INCLUDES = -I. -I$(srcdir) -I.. -I$(srcdir)/.. \
	$(GTK_CFLAGS) $(X_CFLAGS) $(GMODULE_FLAGS) 


# These programs are built only to check that the plugin
# libs have correct library information.

check_PROGRAMS = t-load
t_load_LDADD = libgdk_imlib.la
