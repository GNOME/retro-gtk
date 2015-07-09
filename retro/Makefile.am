## Process this file with automake to produce Makefile.in

NULL =

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DPROJECT_NAME=\""retro"\" \
	-DPROJECT_API_VERSION=\""0.2"\" \
	-DPROJECT_DIR_NAME=\""retro-0.2"\" \
	-DPROJECT_PLUGINS_DIR=\""$(libdir)/retro-0.2/plugins"\" \
	$(RETRO_GOBJECT_CFLAGS) \
	$(NULL)

AM_CFLAGS =\
	 -Wall\
	 -g \
	$(NULL)

lib_LTLIBRARIES = libretro-gobject.la

vapidir = $(datadir)/vala/vapi
dist_vapi_DATA = retro-gobject-0.2.vapi retro-gobject-0.2.deps

BUILT_SOURCES = retro-gobject-internal.h

libretro_gobject_la_SOURCES = \
	input/Controller.vala \
	input/Device.vala \
	input/Input.vala \
	input/InputDevice.vala \
	input/InputDeviceManager.vala \
	input/Keyboard.vala \
	log/FileStreamLog.vala \
	log/Log.vala \
	loop/Loop.vala \
	loop/MainLoop.vala \
	loop/ThreadedLoop.vala \
	variable/Options.vala \
	variable/Variables.vala \
	Audio.vala \
	AvInfo.vala \
	Camera.vala \
	Core.vala \
	CoreFactory.vala \
	DiskController.vala \
	FrameTime.vala \
	GameInfo.vala \
	GameType.vala \
	HardwareRender.vala \
	Language.vala \
	Location.vala \
	Memory.vala \
	Module.vala \
	Performance.vala \
	Region.vala \
	Retro.vala \
	Rumble.vala \
	Sensor.vala \
	Subsystem.vala \
	SystemInfo.vala \
	Video.vala \
	retro-environment.c \
	retro-environment-interfaces.c \
	$(NULL)


retro-environment.c: retro-gobject-internal.h

retro-environment-interfaces.c: retro-gobject-internal.h

libretro_gobject_la_LDFLAGS =

libretro_gobject_la_VALAFLAGS = \
	--pkg config \
	--pkg glib-2.0 \
	--pkg gobject-2.0 \
	--pkg gmodule-2.0 \
	--pkg gio-2.0  \
	--vapidir=vapi \
	--thread \
	--target-glib 2.32 \
	--library retro-gobject-0.2 \
	--vapi retro-gobject-0.2.vapi \
	--gir Retro-0.2.gir \
	-H retro-gobject.h \
	-h retro-gobject-internal.h \
	-X -fPIC -X -shared \
	$(NULL)

libretro_gobject_la_LIBADD = $(RETRO_GOBJECT_LIBS)

retro_gobjectincludedir = $(includedir)/retro-gobject-0.2
retro_gobjectinclude_HEADERS = \
	retro-gobject.h \
	$(NULL)

# Introspection
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

INTROSPECTION_GIRS += Retro-0.2.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

introspection_sources = $(retro_gobjectinclude_HEADERS)

BUILT_SOURCES += libretro-gobject.la Retro-0.2.gir

Retro_0_2_gir_INCLUDES = GLib-2.0 GObject-2.0 Gio-2.0
Retro_0_2_gir_SCANNERFLAGS = --c-include=retro-gobject.h --pkg-export=retro
Retro_0_2_gir_CFLAGS = $(RETRO_GOBJECT_CFLAGS)
Retro_0_2_gir_LIBS = libretro-gobject.la
Retro_0_2_gir_FILES = $(introspection_sources)
Retro_0_2_gir_EXPORT_PACKAGES = retro

CLEANFILES = $(gir_DATA) $(typelib_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = retro-gobject-0.2.pc

EXTRA_DIST = \
	retro-gobject-0.2.pc.in \
	$(NULL)

