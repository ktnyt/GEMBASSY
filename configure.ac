dnl Configure template for the EMBOSS package.
dnl Process this file with autoconf to produce a configure script.

#if you want to force configure to use the default C compiler cc
## then uncomment the following line.
#CC=cc

AC_PREREQ([2.59])
AC_INIT([GEMBASSY], [1.0.0], [celery@g-language.org],
    [GEMBASSY], [http://www.g-language.org/gembassy/])
AC_REVISION([$Revision: 1.36 $])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_SRCDIR(src/ggcskew.c)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

dnl Make sure CFLAGS is defined to stop LT_INIT adding -g
CFLAGS="$CFLAGS "

dnl Use libtool to make a shared library.
LT_INIT

AC_PROG_AWK
AC_PROG_CC([icc gcc cc])
AC_PROG_CPP
AC_C_INLINE




dnl Check if 64 bit pointer support is required on 32 bit machines
dnl Disabled by default
use_64bit_ptrs=no
AC_ARG_ENABLE([64],
    AS_HELP_STRING([--enable-64], [64 bit pointers on 32 bit machines]))

AS_IF([test "x$enable_64" = "xyes"], [
  AC_MSG_CHECKING([for 64bit compilation support])  
  use_64bit_ptrs=yes 

  case $host_os in
  aix*)
	CPPFLAGS="-DAJ_AIX64 $CPPFLAGS"
	case $CC in
          gcc)
	    ;;
	  *)
	    CC="$CC -q64"
	    ;;
        esac
	NM="nm -B -X 64"
	AR="ar -X 64"
	;;

  hpux*)
	case $CC in
          gcc)
	    ;;
	  *)
	    CC="$CC +DD64"
	    ;;
        esac
	AC_DEFINE([HPUX64PTRS],[1],[Set to 1 if HPUX 64bit ptrs on 32 bit m/c])
	;;
  *)
        ;;
  esac
  AC_MSG_RESULT([done])
])
dnl End of checks for 64 bit pointers on 32 bit machines








dnl Compiler optimisations
dnl The Solaris 64bit ptr check has to be done here owing to param order
AC_ARG_WITH([optimisation],
    AS_HELP_STRING([--without-optimisation], [Disable compiler optimisation]))

AS_IF([test "x$with_optimisation" != "xno"], [
case $CC in

gcc)
	dnl Intel MacOSX requires reduced optimisation for PCRE code
	dnl other OSs just use -O2
	case $host_os in
	darwin*)
		if test "$host_cpu" = "i386"; then
	    	   CFLAGS="$CFLAGS -O1"
		else
	 	   CFLAGS="$CFLAGS -O2"
        	fi
	        ;;

   	*)
		CFLAGS="$CFLAGS -O2"
		;;
	esac
	;;

*)
	case $host_os in

	aix*)
		CFLAGS="$CFLAGS -O3 -qstrict -qarch=auto -qtune=auto"
		;;

	irix*)
		LD="/usr/bin/ld -IPA"
		CFLAGS="$CFLAGS -O3"
		;;

	hpux*)
		CFLAGS="$CFLAGS -fast"
		;;

	osf*)
		CFLAGS="$CFLAGS -fast -U_FASTMATH"
		;;

	solaris*)
		CFLAGS="$CFLAGS -O"

		dnl test for 64 bit ptr here (see Solaris 64bit above)

		if test "${enable_64}" = "yes" ; then
		   CFLAGS="$CFLAGS -xtarget=ultra -xarch=v9"
		fi
		;;

	linux*)
	dnl Default optimisation for non-gcc compilers under Linux
	    	CFLAGS="$CFLAGS -O2"
	    	;;


	freebsd*)
		CFLAGS="$CFLAGS -O2"
		;;

	esac
esac
])
dnl end of compiler tests




dnl Compiler warning levels
dnl Test if --enable-warnings given
dnl Test if --enable-warnings given
AC_ARG_ENABLE([warnings],
    [AS_HELP_STRING([--enable-warnings],
        [warnings])])
if test "${enable_warnings}" = "yes" ; then
  case $CC in 
    gcc)
	CFLAGS="$CFLAGS -Wall -fno-strict-aliasing"
	;;
  esac
fi




dnl Test if --enable-devwarnings given
AC_ARG_ENABLE(devwarnings,
   [AS_HELP_STRING([--enable-devwarnings],
                  [strict warnings for developers])])
if test "${enable_devwarnings}" = "yes" ; then
  case $CC in 
    gcc)
	CFLAGS="$CFLAGS -Wstrict-prototypes -Wmissing-prototypes"
	;;
  esac
fi          




dnl Test whether --with-sgiabi given for IRIX (n32m3 n32m4 64m3 64m4)

case $host_os in
irix*)
	case $CC in
        gcc)
            ;;
        cc)
 	    CHECK_SGI
	    ;;
	esac
	;;
esac




dnl endian test for indexing system
AC_C_BIGENDIAN

dnl PCRE library definitions - see the MAJOR and MINOR values
dnl to see which version's configure.in these lines come from

dnl Provide the current PCRE version information. Do not use numbers
dnl with leading zeros for the minor version, as they end up in a C
dnl macro, and may be treated as octal constants. Stick to single
dnl digits for minor numbers less than 10. There are unlikely to be
dnl that many releases anyway.

PCRE_MAJOR=4
PCRE_MINOR=3
PCRE_DATE=21-May-2003
PCRE_VERSION=${PCRE_MAJOR}.${PCRE_MINOR}

dnl Default values for miscellaneous macros

POSIX_MALLOC_THRESHOLD=-DPOSIX_MALLOC_THRESHOLD=10

dnl Provide versioning information for libtool shared libraries that
dnl are built by default on Unix systems.

PCRE_LIB_VERSION=0:1:0
PCRE_POSIXLIB_VERSION=0:0:0


dnl Program checks
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl Intel MacOSX 10.6 puts X11 in a non-standard place
if test "${with_x}" != "no" ; then
case $host_os in
darwin*)
	OSXX=`sw_vers -productVersion | sed 's/\(10\.[[0-9]]*\).*/\1/'`
	if test ${OSXX} '>' '10.4'; then
	   CFLAGS="$CFLAGS -I/usr/X11/include -L/usr/X11/lib"
	fi
	;;
esac
fi


dnl Check for header files
AC_PATH_XTRA
AC_HEADER_DIRENT
AC_HEADER_STDC

AC_CHECK_HEADERS([unistd.h])

dnl Check for typedefs, structures and compiler characteristics
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_STRUCT_TM

dnl Check for library functions
AC_FUNC_GETPGRP
AC_FUNC_STRFTIME
AC_FUNC_FORK
AC_FUNC_VPRINTF

AC_CHECK_FUNCS([strdup strstr strchr erand48 memmove])

dnl Check for libcurl-devel
AC_CHECK_HEADERS([curl/curl.h])

dnl Library checks
AC_CHECK_LIB(c, socket, LIBS="$LIBS" , LIBS="$LIBS -lsocket")
AC_CHECK_LIB(m, main)

dnl GD for FreeBSD requires libiconv
case $host_os in
freebsd*)
	if test ${with_pngdriver} != "no"; then
	   AC_CHECK_LIB(iconv, main, LIBS="$LIBS", LIBS="-liconv $LIBS")
	fi
	;;
esac

AM_CONDITIONAL(AMPNG, false)
AM_CONDITIONAL(AMPDF, false)



CHECK_GENERAL
CHECK_JAVA
CHECK_JAVAOS
CHECK_HPDF
CHECK_PNGDRIVER
CHECK_AUTH
CHECK_AMD64

AX_LIB_MYSQL
AX_LIB_POSTGRESQL


dnl "Export" these variables for PCRE

AC_SUBST([HAVE_MEMMOVE])
AC_SUBST([HAVE_STRERROR])
AC_SUBST([PCRE_MAJOR])
AC_SUBST([PCRE_MINOR])
AC_SUBST([PCRE_DATE])
AC_SUBST([PCRE_VERSION])
AC_SUBST([PCRE_LIB_VERSION])
AC_SUBST([PCRE_POSIXLIB_VERSION])
AC_SUBST([POSIX_MALLOC_THRESHOLD])

dnl Test if --enable-localforce given
locallink=no
embprefix=/usr/local
AC_ARG_ENABLE([localforce],
  [AS_HELP_STRING([--enable-localforce],
  [force compile/link against /usr/local])])

if test "${enable_localforce}" = "yes" ; then
embprefix=/usr/local
fi          

if test "${prefix}" = "NONE" ; then
   if test "${enable_localforce}" != "yes" ; then
      locallink=yes
   else
      locallink=no
      embprefix=/usr/local
   fi
else
   embprefix=${prefix}
fi
AM_CONDITIONAL(LOCALLINK, test "${locallink}" = "yes")
AC_SUBST([embprefix])

dnl Test if --enable-debug given
AC_ARG_ENABLE([debug],
   [AS_HELP_STRING([--enable-debug], [debug (-g option on compiler)])])
if test "${enable_debug}" = "yes" ; then
  CFLAGS="$CFLAGS -g"
fi


dnl Turn off irritating linker warnings in IRIX
case $host_os in
irix*)
	CFLAGS="-Wl,-LD_MSG:off=85:off=84:off=16:off=134 $CFLAGS"
	;;
esac



dnl Test if --enable-large is given
if test "${enable_large+set}" != set; then
enable_large=yes
fi


AC_ARG_ENABLE([large],
   [AS_HELP_STRING([--enable-large], [over 2Gb file support (default)])])
if test "${enable_large}" = "yes" ; then
AC_MSG_CHECKING([for large file support])


dnl Test for Linux large file support
case $host_os in
linux*)
	CPPFLAGS="-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE $CPPFLAGS"
	CPPFLAGS="-DAJ_LinuxLF -D_FILE_OFFSET_BITS=64 $CPPFLAGS"
	;;

freebsd*)
	CPPFLAGS="-DAJ_FreeBSDLF $CPPFLAGS"
	;;

solaris*)
	CPPFLAGS="-D_LARGEFILE_SOURCE $CPPFLAGS"
	CPPFLAGS="-DAJ_SolarisLF -D_FILE_OFFSET_BITS=64 $CPPFLAGS"
	;;

osf*)
	CPPFLAGS="-DAJ_OSF1LF $CPPFLAGS"
	;;


irix*)
	CPPFLAGS="-DAJ_IRIXLF -D_LARGEFILE64_SOURCE $CPPFLAGS"
	;;

aix*)
	CPPFLAGS="-DAJ_AIXLF -D_LARGE_FILES $CPPFLAGS"
	;;

hpux*)
	CPPFLAGS="-D_FILE_OFFSET_BITS=64 $CPPFLAGS"
	CPPFLAGS="-DAJ_HPUXLF -D_LARGEFILE_SOURCE $CPPFLAGS"
	;;

darwin*)
	CPPFLAGS="-DAJ_MACOSXLF $CPPFLAGS"
	;;
esac

AC_MSG_RESULT([done])
fi





dnl Test if --enable-systemlibs given
have_systemlibs=no
AC_ARG_ENABLE([systemlibs],
   [AS_HELP_STRING([--enable-systemlibs], [utility for RPM/dpkg bundles])])
if test "${enable_systemlibs}" = "yes" ; then
    have_systemlibs=yes
fi          
AM_CONDITIONAL(ESYSTEMLIBS, test "$have_systemlibs" = "yes")
AC_SUBST([ESYSTEMLIBS])





dnl Test if purify exists and if --enable-purify given if so
dnl set "-g"

AC_MSG_CHECKING([for purify])
dnl if(purify -version) < /dev/null > /dev/null 2>&1; then
   AC_ARG_ENABLE([purify],
      [AS_HELP_STRING([--enable-purify], [purify])])
if test "${enable_purify}" = "yes" ; then
     CC="purify --chain-length="20" -best-effort -windows=yes gcc -g"
     LIBTOOL="$LIBTOOL --tag=CC"
     AC_MSG_RESULT([yes])
else
   AC_MSG_RESULT([no])
fi




dnl Set extra needed compiler flags
if test "$CC" = "cc"; then
  case "$host" in
    alpha*-dec-osf*) CFLAGS="$CFLAGS -ieee";;
  esac
fi
AM_CONDITIONAL(PURIFY, test "${enable_purify}" = "yes")




dnl test for cygwin to set AM_LDFLAGS in library & apps Makefile.ams
dnl Replaces original version which used 'expr' and so wasn't entirely
dnl portable.
platform_cygwin='no'
AC_MSG_CHECKING([for cygwin])
case "$host" in
  *-*-mingw*|*-*-cygwin*)
    platform_cygwin='yes'
    ;;
  *)
    platform_cygwin='no'
    ;;
esac
AC_MSG_RESULT([$platform_cygwin])
AM_CONDITIONAL(ISCYGWIN, test "$platform_cygwin" = "yes")
AC_SUBST([ISCYGWIN])

dnl Tests for AIX
dnl If shared needs -Wl,-G in plplot,ajax,nucleus, -lX11 in plplot,
dnl and -Wl,brtl -Wl,-bdynamic in emboss
dnl We therefore need a static test as well
needajax=no

case $host_os in
aix*)
	AM_CONDITIONAL(ISAIXIA64, true)
	;;
*)
	AM_CONDITIONAL(ISAIXIA64, false)
	;;
esac

AM_CONDITIONAL(ISSHARED, test "${enable_shared}" = "yes")
AC_SUBST([ISAIXIA64])

case $host_os in
aix*)
	if test -d ajax/.libs ; then
	   echo AIX ajax/.libs exists
	else
	   mkdir ajax/.libs
	fi

	case $host_os in
	     aix5*) needajax=no ;;
	     aix4.3.3*) needajax=yes ;;
	     *) needajax=no ;;
	     esac
esac

AM_CONDITIONAL(NEEDAJAX,test "$needajax" = "yes")
AC_SUBST([NEEDAJAX])


dnl HP-UX needs -lsec for shadow passwords
case $host_os in
hpux*)
	LDFLAGS="$LDFLAGS -lsec"
	;;
esac


dnl check and set compilation flags for Jemboss auth server code
CHECK_THREADS

AC_CONFIG_FILES([
Makefile
src/Makefile
acd/Makefile
doc/Makefile
doc/html/Makefile
doc/text/Makefile
])

AC_OUTPUT
