## Process this file with automake to produce Makefile.in

NULL =

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DPROJECT_NAME=\""retro"\" \
	-DPROJECT_API_VERSION=\""0.6"\" \
	-DPROJECT_DIR_NAME=\""retro-1.0"\" \
	-DPROJECT_PLUGINS_DIR=\""$(libdir)/retro-1.0/plugins"\" \
	$(RETRO_GOBJECT_CFLAGS) \
	$(NULL)

AM_CFLAGS =\
	 -Wall\
	 -g \
	$(NULL)

lib_LTLIBRARIES = libretro-gobject.la

vapidir = $(datadir)/vala/vapi
dist_vapi_DATA = retro-gobject-0.6.vapi retro-gobject-0.6.deps

BUILT_SOURCES = retro-gobject-internal.h

libretro_gobject_la_SOURCES = \
	input/controller.vala \
	input/device.vala \
	input/input.vala \
	input/input-device.vala \
	input/input-device-manager.vala \
	input/keyboard.vala \
	log/file-stream-log.vala \
	log/log.vala \
	loop/loop.vala \
	loop/main-loop.vala \
	loop/threaded-loop.vala \
	variable/options.vala \
	variable/variables.vala \
	audio.vala \
	av-info.vala \
	camera.vala \
	core.vala \
	core-factory.vala \
	disk-controller.vala \
	frame-time.vala \
	game-info.vala \
	game-type.vala \
	hardware-render.vala \
	language.vala \
	location.vala \
	memory.vala \
	module.vala \
	performance.vala \
	region.vala \
	retro.vala \
	rumble.vala \
	sensor.vala \
	subsystem.vala \
	system-info.vala \
	video.vala \
	retro-core.c \
	retro-environment.c \
	retro-environment-interfaces.c \
	libretro-environment.h \
	retro-environment-core.h \
	retro-environment-input.h \
	retro-environment-interfaces.h \
	retro-environment-variables.h \
	retro-environment-video.h \
	$(NULL)

retro-core.c: retro-gobject-internal.h

retro-environment.c: retro-gobject-internal.h libretro-environment.h

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
	--library retro-gobject-0.6 \
	--vapi retro-gobject-0.6.vapi \
	--gir Retro-0.6.gir \
	-H retro-gobject.h \
	-h retro-gobject-internal.h \
	-X -fPIC -X -shared \
	$(NULL)

libretro_gobject_la_LIBADD = $(RETRO_GOBJECT_LIBS)

retro_gobjectincludedir = $(includedir)/retro-gobject-0.6
retro_gobjectinclude_HEADERS = \
	retro-gobject.h \
	$(NULL)

# Introspection
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

INTROSPECTION_GIRS += Retro-0.6.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

introspection_sources = $(retro_gobjectinclude_HEADERS)

BUILT_SOURCES += libretro-gobject.la Retro-0.6.gir

Retro_0_6_gir_INCLUDES = GLib-2.0 GObject-2.0 Gio-2.0
Retro_0_6_gir_SCANNERFLAGS = --c-include=retro-gobject.h --pkg-export=retro-gobject-0.6
Retro_0_6_gir_CFLAGS = $(RETRO_GOBJECT_CFLAGS)
Retro_0_6_gir_LIBS = libretro-gobject.la
Retro_0_6_gir_FILES = $(introspection_sources)
Retro_0_6_gir_EXPORT_PACKAGES = retro-gobject-0.6

CLEANFILES = $(gir_DATA) $(typelib_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = retro-gobject-0.6.pc

EXTRA_DIST = \
	retro-gobject-0.6.pc.in \
	$(NULL)

