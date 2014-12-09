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

libretro_gtk_la_SOURCES = \
	audio/AudioDevice.vala \
	audio/AudioPlayer.vala \
	input/ControllerDevice.vala \
	input/ControllerHandler.vala \
	input/Gamepad.vala \
	input/GamepadButtonType.vala \
	input/GamepadConfiguration.vala \
	input/GamepadConfigurationDialog.vala \
	input/GamepadView.vala \
	input/Keyboard.vala \
	input/KeyboardState.vala \
	input/Mouse.vala \
	video/CairoDisplay.vala \
	video/ClutterDisplay.vala \
	video/Display.vala \
	video/video-converter.c \
	FileStreamLogger.vala \
	Loop.vala \
	MainLoop.vala \
	Options.vala \
	RetroGtk.vala \
	ThreadedLoop.vala \
	$(NULL)

libretro_gtk_la_LDFLAGS =

libretro_gtk_la_VALAFLAGS = \
	--pkg glib-2.0 \
	--pkg gobject-2.0 \
	--pkg gtk+-3.0 \
	--pkg cairo \
	--pkg clutter-gtk-1.0 \
	--pkg libpulse \
	--pkg retro-gobject-0.1 \
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

INTROSPECTION_GIRS = RetroGtk-0.1.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

RetroGtk-0.1.typelib:
	g-ir-compiler --shared-library retro-gtk-0.1.so --output RetroGtk-0.1.typelib RetroGtk-0.1.gir

CLEANFILES = $(gir_DATA) $(typelibs_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = retro-gtk-0.1.pc

EXTRA_DIST = \
	retro-gtk-0.1.pc.in \
	$(NULL)

