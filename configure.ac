dnl Process this file with autoconf to produce a configure script.

AC_INIT(retro-gobject, 0.6)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC

AM_PROG_AR

AM_PROG_VALAC

GOBJECT_INTROSPECTION_CHECK([0.6.7])

dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=retro-gobject
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT




AC_DISABLE_STATIC

LT_INIT


pkg_modules="gobject-2.0 glib-2.0 gmodule-2.0 gio-2.0"
AC_SUBST(pkg_modules)
PKG_CHECK_MODULES(RETRO_GOBJECT, [gobject-2.0 glib-2.0 gmodule-2.0 gio-2.0])





AC_OUTPUT([
Makefile
retro-gobject/retro-gobject-0.6.pc
retro-gobject/Makefile
po/Makefile.in])
