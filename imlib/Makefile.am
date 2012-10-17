SUBDIRS = @GDK_IMLIB@ Imlib config doc
bin_SCRIPTS = imlib-config

man_MANS = imlib-config.1

EXTRA_DIST = imlib.spec.in imlib.m4 configure configure.in \
	imlib.pc.in imlibgdk.pc.in $(man_MANS)

dist-hook: imlib.spec
	cp imlib.spec $(distdir)

m4datadir = $(datadir)/aclocal
m4data_DATA = imlib.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA= imlib.pc imlibgdk.pc

