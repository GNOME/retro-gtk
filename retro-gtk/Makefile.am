## Process this file with automake to produce Makefile.in

NULL =

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DRETRO_GOBJECT_USE_UNSTABLE_API \
	-I$(top_srcdir)/retro-gobject \
	$(RETRO_GTK_CFLAGS) \
	$(NULL)

AM_CFLAGS =\
	 -Wall\
	 -g \
	$(NULL)

lib_LTLIBRARIES = libretro-gtk.la

vapidir = $(datadir)/vala/vapi
dist_vapi_DATA = retro-gtk-0.8.vapi retro-gtk-0.8.deps

BUILT_SOURCES =

libretro_gtk_la_SOURCES = \
	input/gamepad-button-type.vala \
	input/gamepad-configuration.vala \
	input/input-device-manager.vala \
	input/keyboard.vala \
	input/keyboard-state.vala \
	input/mouse.vala \
	input/virtual-gamepad.vala \
	$(NULL)

libretro_gtk_la_LDFLAGS =

libretro_gtk_la_VALAFLAGS = \
	--vapidir $(top_builddir)/retro-gobject \
	--pkg glib-2.0 \
	--pkg gobject-2.0 \
	--pkg gtk+-3.0 \
	--pkg retro-gobject-0.8 \
	--thread \
	--target-glib 2.32 \
	--library retro-gtk-0.8 \
	--vapi retro-gtk-0.8.vapi \
	-H retro-gtk.h \
	-X -fPIC -X -shared \
	$(NULL)

libretro_gtk_la_LIBADD = \
	$(top_builddir)/retro-gobject/libretro-gobject.la \
	$(RETRO_GTK_LIBS) \
	$(NULL)

libretro_gtk_la_DEPENDENCIES = \
	$(top_builddir)/retro-gobject/libretro-gobject.la \
	$(NULL)

retro_gtkincludedir = $(includedir)/retro-gtk-0.8
retro_gtkinclude_HEADERS = \
	retro-gtk.h \
	$(NULL)

# Add unstable API guard
$(outdir)/retro-gtk.h:
	sed -i "s/#define __RETRO_GTK_H__/#define __RETRO_GTK_H__\n\n#ifndef RETRO_GTK_USE_UNSTABLE_API\n#error    RetroGtk is unstable API. You must define RETRO_GTK_USE_UNSTABLE_API before including retro-gtk.h\n#endif/" $@

# Introspection
-include $(INTROSPECTION_MAKEFILE)

RetroGtk-0.8.gir: $(INTROSPECTION_SCANNER) libretro-gtk.la $(top_builddir)/retro-gobject/libretro-gobject.la $(top_builddir)/retro-gobject/Retro-0.8.gir
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = \
	--includedir=$(srcdir) \
	--includedir=$(top_builddir)/retro-gobject \
	$(NULL)

INTROSPECTION_GIRS += RetroGtk-0.8.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

introspection_sources = $(retro_gtkinclude_HEADERS)

BUILT_SOURCES += libretro-gtk.la RetroGtk-0.8.gir

RetroGtk_0_8_gir_INCLUDES = GLib-2.0 GObject-2.0 Gtk-3.0 cairo-1.0 Retro-0.8
RetroGtk_0_8_gir_SCANNERFLAGS = \
	--c-include=retro-gtk.h \
	--pkg-export=retro-gtk-0.8 \
	-I$(top_builddir)/retro-gobject \
	--add-include-path=$(top_builddir)/retro-gobject \
	--include-uninstalled=$(top_builddir)/retro-gobject/Retro-0.8.gir \
	$(NULL)
RetroGtk_0_8_gir_CFLAGS = -DRETRO_GTK_USE_UNSTABLE_API $(AM_CPPFLAGS)
RetroGtk_0_8_gir_LIBS = libretro-gtk.la
RetroGtk_0_8_gir_FILES = $(introspection_sources)
RetroGtk_0_8_gir_EXPORT_PACKAGES = retro-gtk-0.8

CLEANFILES = $(gir_DATA) $(typelib_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = retro-gtk-0.8.pc

EXTRA_DIST = \
	retro-gtk-0.8.pc.in \
	$(NULL)

-include $(top_srcdir)/git.mk
