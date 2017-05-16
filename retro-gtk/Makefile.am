## Process this file with automake to produce Makefile.in

NULL =

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DPROJECT_NAME=\""retro"\" \
	-DPROJECT_API_VERSION=\""0.10"\" \
	-DPROJECT_DIR_NAME=\""retro-1.0"\" \
	-DPROJECT_PLUGINS_DIR=\""$(libdir)/retro-1.0/plugins"\" \
	-DRETRO_PLUGIN_PATH=\""$(libdir)/retro-1.0/plugins:$(libdir)/libretro:$(libdir)"\" \
	$(RETRO_GTK_CFLAGS) \
	$(NULL)

AM_CFLAGS =\
	 -Wall\
	 -g \
	$(NULL)

lib_LTLIBRARIES = libretro-gtk.la

vapidir = $(datadir)/vala/vapi
dist_vapi_DATA = retro-gtk-0.10.vapi retro-gtk-0.10.deps

BUILT_SOURCES = retro-gtk-internal.h

libretro_gtk_la_SOURCES = \
	audio/pa-player.vala \
	\
	input/controller.vala \
	input/device.vala \
	input/gamepad-button-type.vala \
	input/gamepad-configuration.vala \
	input/input.vala \
	input/input-device.vala \
	input/input-device-manager.vala \
	input/keyboard.vala \
	input/keyboard-state.vala \
	input/mouse.vala \
	input/virtual-gamepad.vala \
	\
	log/file-stream-log.vala \
	log/log.vala \
	\
	loop/main-loop.vala \
	\
	variable/options.vala \
	variable/variables.vala \
	\
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
	retro-log.c \
	retro-module-query.vala \
	retro-module-iterator.vala \
	rumble.vala \
	system-info.vala \
	video.vala \
	retro-core.c \
	retro-environment.c \
	libretro-environment.h \
	\
	video/cairo-display.vala \
	video/retro-video-converter.c \
	video/video-filter.vala \
	$(NULL)

video/retro-video-converter.c: retro-gtk-internal.h

retro-core.c: retro-gtk-internal.h

retro-environment.c: retro-gtk-internal.h libretro-environment.h

retro-log.c: retro-gtk-internal.h

libretro_gtk_la_LDFLAGS =

libretro_gtk_la_VALAFLAGS = \
	--pkg cairo \
	--pkg config \
	--pkg gio-2.0  \
	--pkg glib-2.0 \
	--pkg gmodule-2.0 \
	--pkg gobject-2.0 \
	--pkg gtk+-3.0 \
	--pkg libpulse \
	--pkg libpulse-simple \
	--vapidir=vapi \
	--thread \
	--target-glib 2.32 \
	--library retro-gtk-0.10 \
	--vapi retro-gtk-0.10.vapi \
	-H retro-gtk.h \
	-h retro-gtk-internal.h \
	-X -fPIC -X -shared \
	$(NULL)

libretro_gtk_la_LIBADD = $(RETRO_GTK_LIBS)

retro_gtkincludedir = $(includedir)/retro-gtk-0.10
retro_gtkinclude_HEADERS = \
	retro-gtk.h \
	$(NULL)

# Add unstable API guard
$(outdir)/retro-gtk.h:
	sed -i "s/#define __RETRO_GTK_H__/#define __RETRO_GTK_H__\n\n#ifndef RETRO_GTK_USE_UNSTABLE_API\n#error    retro-gtk is unstable API. You must define RETRO_GTK_USE_UNSTABLE_API before including retro-gtk.h\n#endif/" $@

# Introspection
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

INTROSPECTION_GIRS += Retro-0.10.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

introspection_sources = $(retro_gtkinclude_HEADERS)

BUILT_SOURCES += libretro-gtk.la Retro-0.10.gir

Retro-0.10.gir: $(INTROSPECTION_SCANNER) libretro-gtk.la
Retro_0_10_gir_INCLUDES = GLib-2.0 GObject-2.0 Gio-2.0 Gtk-3.0 cairo-1.0
Retro_0_10_gir_SCANNERFLAGS = --c-include=retro-gtk.h --pkg-export=retro-gtk-0.10
Retro_0_10_gir_CFLAGS = -DRETRO_GTK_USE_UNSTABLE_API $(retro_gtk_CFLAGS)
Retro_0_10_gir_LIBS = libretro-gtk.la
Retro_0_10_gir_FILES = $(introspection_sources)
Retro_0_10_gir_EXPORT_PACKAGES = retro-gtk-0.10

CLEANFILES = $(gir_DATA) $(typelib_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = retro-gtk-0.10.pc

EXTRA_DIST = \
	retro-gtk-0.10.pc.in \
	update-from-retroarch.sh \
	$(NULL)

INPUTDIR=$(top_srcdir)/../RetroArch/libretro-common/include/
INPUTFILES = libretro.h
update-from-retroarch:
	SRCDIR="$(srcdir)" FILES="$(INPUTFILES)" DIR="$(INPUTDIR)" $(srcdir)/update-from-retroarch.sh && changed=true ; \
	cd $(srcdir) && git commit -m "retro-gtk: Update libretro header" $(INPUTFILES)


-include $(top_srcdir)/git.mk
