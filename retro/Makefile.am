## Process this file with automake to produce Makefile.in

NULL =

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DPACKAGE_PLUGINS_DIR=\""$(pkglibdir)/plugins"\" \
	$(RETRO_GOBJECT_CFLAGS) \
	$(NULL)

AM_CFLAGS =\
	 -Wall\
	 -g \
	$(NULL)

lib_LTLIBRARIES = libretro-gobject.la

vapidir = $(datadir)/vala/vapi
dist_vapi_DATA = retro-gobject-0.2.vapi retro-gobject-0.2.deps

libretro_gobject_la_SOURCES = \
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
	Controller.vala \
	Core.vala \
	CoreFactory.vala \
	Device.vala \
	DiskController.vala \
	FrameTime.vala \
	GameInfo.vala \
	GameType.vala \
	HardwareRender.vala \
	Input.vala \
	Keyboard.vala \
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

INTROSPECTION_GIRS = Retro-0.2.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

Retro-0.2.typelib:
	g-ir-compiler --shared-library retro-gobject-0.2.so --output Retro-0.2.typelib Retro-0.2.gir

CLEANFILES = $(gir_DATA) $(typelibs_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = retro-gobject-0.2.pc

EXTRA_DIST = \
	retro-gobject-0.2.pc.in \
	$(NULL)

