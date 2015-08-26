## Process this file with automake to produce Makefile.in

NULL =

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(RETRO_GTK_CFLAGS) \
	$(NULL)

AM_CFLAGS =\
	 -Wall\
	 -g \
	$(NULL)

lib_LTLIBRARIES = libretro-gtk.la

vapidir = $(datadir)/vala/vapi
dist_vapi_DATA = retro-gtk-0.1.vapi retro-gtk-0.1.deps

BUILT_SOURCES =

libretro_gtk_la_SOURCES = \
	audio/pa-player.vala \
	\
	input/gamepad.vala \
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
	\
	retro-gtk.vala \
	$(NULL)

libretro_gtk_la_LDFLAGS =

libretro_gtk_la_VALAFLAGS = \
	--pkg glib-2.0 \
	--pkg gobject-2.0 \
	--pkg gtk+-3.0 \
	--pkg cairo \
	--pkg clutter-gtk-1.0 \
	--pkg libpulse \
	--pkg jsk-0.1 \
	--pkg retro-gobject-0.3 \
	--thread \
	--target-glib 2.32 \
	--library retro-gtk-0.1 \
	--vapi retro-gtk-0.1.vapi \
	--gir RetroGtk-0.1.gir \
	-H retro-gtk.h \
	-X -fPIC -X -shared \
	$(NULL)

libretro_gtk_la_LIBADD = $(RETRO_GTK_LIBS)

retro_gtkincludedir = $(includedir)/retro-gtk-0.1
retro_gtkinclude_HEADERS = \
	retro-gtk.h \
	$(NULL)

# Introspection
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

INTROSPECTION_GIRS += RetroGtk-0.1.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

introspection_sources = $(retro_gtkinclude_HEADERS)

BUILT_SOURCES += libretro-gtk.la RetroGtk-0.1.gir

RetroGtk_0_1_gir_INCLUDES = GLib-2.0 GObject-2.0 Gtk-3.0 cairo-1.0 GtkClutter-1.0 Jsk-0.1 Retro-0.3
RetroGtk_0_1_gir_SCANNERFLAGS = --c-include=retro-gtk.h --pkg-export=retro-gtk-0.1
RetroGtk_0_1_gir_CFLAGS = $(GTK)
RetroGtk_0_1_gir_LIBS = libretro-gtk.la
RetroGtk_0_1_gir_FILES = $(introspection_sources)
RetroGtk_0_1_gir_EXPORT_PACKAGES = retro-gtk-0.1

CLEANFILES = $(gir_DATA) $(typelib_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = retro-gtk-0.1.pc

EXTRA_DIST = \
	retro-gtk-0.1.pc.in \
	$(NULL)

