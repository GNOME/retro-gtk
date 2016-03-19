## Process this file with automake to produce Makefile.in

NULL =

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-I$(top_srcdir)/retro-gobject \
	$(RETRO_GTK_CFLAGS) \
	$(NULL)

AM_CFLAGS =\
	 -Wall\
	 -g \
	$(NULL)

lib_LTLIBRARIES = libretro-gtk.la

vapidir = $(datadir)/vala/vapi
dist_vapi_DATA = retro-gtk-0.6.vapi retro-gtk-0.6.deps

BUILT_SOURCES =

libretro_gtk_la_SOURCES = \
	audio/pa-player.vala \
	\
	input/gamepad-button-type.vala \
	input/gamepad-configuration.vala \
	input/gamepad-configuration-dialog.vala \
	input/gamepad-view.vala \
	input/input-device-manager.vala \
	input/keyboard.vala \
	input/keyboard-state.vala \
	input/mouse.vala \
	input/virtual-gamepad.vala \
	\
	video/cairo-display.vala \
	video/clutter-display.vala \
	video/display.vala \
	video/video-converter.c \
	video/video-converter.h \
	\
	retro-gtk.vala \
	$(NULL)

libretro_gtk_la_LDFLAGS =

libretro_gtk_la_VALAFLAGS = \
	--vapidir $(top_builddir)/retro-gobject \
	--pkg glib-2.0 \
	--pkg gobject-2.0 \
	--pkg gtk+-3.0 \
	--pkg cairo \
	--pkg clutter-gtk-1.0 \
	--pkg libpulse \
	--pkg retro-gobject-0.6 \
	--thread \
	--target-glib 2.32 \
	--library retro-gtk-0.6 \
	--vapi retro-gtk-0.6.vapi \
	--gir RetroGtk-0.6.gir \
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

retro_gtkincludedir = $(includedir)/retro-gtk-0.6
retro_gtkinclude_HEADERS = \
	retro-gtk.h \
	$(NULL)

# Introspection
-include $(INTROSPECTION_MAKEFILE)

RetroGtk-0.6.gir: $(INTROSPECTION_SCANNER) libretro-gtk.la $(top_builddir)/retro-gobject/libretro-gobject.la $(top_builddir)/retro-gobject/Retro-0.6.gir
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = \
	--includedir=$(srcdir) \
	--includedir=$(top_builddir)/retro-gobject \
	$(NULL)

INTROSPECTION_GIRS += RetroGtk-0.6.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

introspection_sources = $(retro_gtkinclude_HEADERS)

BUILT_SOURCES += libretro-gtk.la RetroGtk-0.6.gir

RetroGtk_0_6_gir_INCLUDES = GLib-2.0 GObject-2.0 Gtk-3.0 cairo-1.0 GtkClutter-1.0 Retro-0.6
RetroGtk_0_6_gir_SCANNERFLAGS = \
	--c-include=retro-gtk.h \
	--pkg-export=retro-gtk-0.6 \
	-I$(top_builddir)/retro-gobject \
	--add-include-path=$(top_builddir)/retro-gobject \
	--include-uninstalled=$(top_builddir)/retro-gobject/Retro-0.6.gir \
	$(NULL)
RetroGtk_0_6_gir_CFLAGS = $(AM_CPPFLAGS)
RetroGtk_0_6_gir_LIBS = libretro-gtk.la
RetroGtk_0_6_gir_FILES = $(introspection_sources)
RetroGtk_0_6_gir_EXPORT_PACKAGES = retro-gtk-0.6

CLEANFILES = $(gir_DATA) $(typelib_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = retro-gtk-0.6.pc

EXTRA_DIST = \
	retro-gtk-0.6.pc.in \
	$(NULL)

