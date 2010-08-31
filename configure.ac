#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([libamz], [0.0.1], [nenolod@nenolod.net])
AC_CONFIG_SRCDIR([src/libamz/amzfile.c])
AC_CONFIG_HEADER([src/libamz/amzconfig.h])

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# Checks for programs.
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_ISC_POSIX

test x"$GCC" = x"yes" && CFLAGS="$CFLAGS -pipe -Wall"

# XXX workaround
PACKAGE="amztools"
AC_SUBST(PACKAGE)

# Checks for libraries.
BUILDSYS_SHARED_LIB
LIBS="$LIBS $DYNAMIC_LD_LIBS"

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h unistd.h locale.h stdarg.h sys/types.h sys/stat.h errno.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_CHECK_FUNCS([memset setlocale strcasecmp strchr strdup strerror strtol strtod])
AC_CHECK_FUNCS([printf sprintf snprintf vsnprintf mmap gettimeofday strndup])
AC_FUNC_STAT

PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.10])
PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.10])
PKG_CHECK_MODULES(XML, [libxml-2.0])
PKG_CHECK_MODULES(SOUP, [libsoup-2.4])
AM_PATH_LIBGCRYPT([],[],[AC_MSG_ERROR([libgcrypt not found])])

BUILDSYS_TOUCH_DEPS

AC_CONFIG_FILES([buildsys.mk extra.mk])
AC_OUTPUT
