dnl Process this file with autoconf to produce a configure script.

AC_INIT(retro-gtk, 0.11.4.1)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 subdir-objects tar-ustar no-dist-gzip dist-xz])

AM_SILENT_RULES([yes])

AC_PROG_CC

AM_PROG_AR

AM_PROG_VALAC

# GResource
AC_PATH_PROG(GLIB_COMPILE_RESOURCES, glib-compile-resources)

GOBJECT_INTROSPECTION_CHECK([0.6.7])

dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=retro-gtk
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT




AC_DISABLE_STATIC

LT_INIT


###########
# retro-gtk
###########

retro_gtk_pkg_modules="gobject-2.0 glib-2.0 gmodule-2.0 gio-2.0 gtk+-3.0 cairo libpulse libpulse-simple"
PKG_CHECK_MODULES(RETRO_GTK, $retro_gtk_pkg_modules)
AC_SUBST(retro_gtk_pkg_modules)





AC_OUTPUT([
Makefile
demos/Makefile
retro-gtk/retro-gtk-0.12.pc
retro-gtk/Makefile
po/Makefile.in])
