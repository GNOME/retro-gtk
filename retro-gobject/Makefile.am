## Process this file with automake to produce Makefile.in

NULL =

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DPROJECT_NAME=\""retro"\" \
	-DPROJECT_API_VERSION=\""0.8"\" \
	-DPROJECT_DIR_NAME=\""retro-1.0"\" \
	-DPROJECT_PLUGINS_DIR=\""$(libdir)/retro-1.0/plugins"\" \
	-DRETRO_PLUGIN_PATH=\""$(libdir)/retro-1.0/plugins:$(libdir)/libretro:$(libdir)"\" \
	$(RETRO_GOBJECT_CFLAGS) \
	$(NULL)

AM_CFLAGS =\
	 -Wall\
	 -g \
	$(NULL)

lib_LTLIBRARIES = libretro-gobject.la

vapidir = $(datadir)/vala/vapi
dist_vapi_DATA = retro-gobject-0.8.vapi retro-gobject-0.8.deps

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
	loop/main-loop.vala \
	variable/options.vala \
	variable/variables.vala \
	av-info.vala \
	core.vala \
	core-error.vala \
	disk-controller.vala \
	game-info.vala \
	memory.vala \
	module.vala \
	retro.vala \
	retro-core-descriptor.vala \
	retro-core-descriptor-error.vala \
	retro-module-query.vala \
	rumble.vala \
	system-info.vala \
	video.vala \
	retro-core.c \
	retro-environment.c \
	libretro-environment.h \
	$(NULL)

retro-core.c: retro-gobject-internal.h

retro-environment.c: retro-gobject-internal.h libretro-environment.h

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
	--library retro-gobject-0.8 \
	--vapi retro-gobject-0.8.vapi \
	-H retro-gobject.h \
	-h retro-gobject-internal.h \
	-X -fPIC -X -shared \
	$(NULL)

libretro_gobject_la_LIBADD = $(RETRO_GOBJECT_LIBS)

retro_gobjectincludedir = $(includedir)/retro-gobject-0.8
retro_gobjectinclude_HEADERS = \
	retro-gobject.h \
	$(NULL)

# Add unstable API guard
$(outdir)/retro-gobject.h:
	sed -i "s/#define __RETRO_GOBJECT_H__/#define __RETRO_GOBJECT_H__\n\n#ifndef RETRO_GOBJECT_USE_UNSTABLE_API\n#error    Retro is unstable API. You must define RETRO_GOBJECT_USE_UNSTABLE_API before including retro-gobject.h\n#endif/" $@

# Introspection
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

INTROSPECTION_GIRS += Retro-0.8.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

introspection_sources = $(retro_gobjectinclude_HEADERS)

BUILT_SOURCES += libretro-gobject.la Retro-0.8.gir

Retro-0.8.gir: $(INTROSPECTION_SCANNER) libretro-gobject.la
Retro_0_8_gir_INCLUDES = GLib-2.0 GObject-2.0 Gio-2.0
Retro_0_8_gir_SCANNERFLAGS = --c-include=retro-gobject.h --pkg-export=retro-gobject-0.8
Retro_0_8_gir_CFLAGS = -DRETRO_GOBJECT_USE_UNSTABLE_API $(RETRO_GOBJECT_CFLAGS)
Retro_0_8_gir_LIBS = libretro-gobject.la
Retro_0_8_gir_FILES = $(introspection_sources)
Retro_0_8_gir_EXPORT_PACKAGES = retro-gobject-0.8

CLEANFILES = $(gir_DATA) $(typelib_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = retro-gobject-0.8.pc

EXTRA_DIST = \
	retro-gobject-0.8.pc.in \
	$(NULL)

-include $(top_srcdir)/git.mk
