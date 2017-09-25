## Process this file with automake to produce Makefile.in

NULL =

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\""Retro"\" \
	-DRETRO_GTK_COMPILATION \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DPROJECT_NAME=\""retro"\" \
	-DPROJECT_API_VERSION=\""0.12"\" \
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
vapi_DATA = retro-gtk-0.12.vapi
dist_vapi_DATA = retro-gtk-0.12.deps

BUILT_SOURCES = retro-gtk-internal.h

retro_gtk_public_h_sources = \
	retro-analog-id.h \
	retro-analog-index.h \
	retro-controller-iterator.h \
	retro-core-descriptor.h \
	retro-core-view.h \
	retro-device-type.h \
	retro-gtk.h \
	retro-gtk-vala.h \
	retro-input-device.h \
	retro-joypad-id.h \
	retro-lightgun-id.h \
	retro-log.h \
	retro-main-loop.h \
	retro-memory-type.h \
	retro-module-iterator.h \
	retro-module-query.h \
	retro-mouse-id.h \
	retro-pixel-format.h \
	retro-pointer-id.h \
	retro-rumble-effect.h \
	retro-video-filter.h \
	$(NULL)

retro_gtk_private_h_sources = \
	input/retro-keyboard-key.h \
	libretro-environment.h \
	retro-cairo-display.h \
	retro-controller-iterator-private.h \
	retro-core-private.h \
	retro-core-view-input-device.h \
	retro-disk-control-callback.h \
	retro-game-info.h \
	retro-input-descriptor.h \
	retro-module.h \
	retro-option.h \
	retro-options.h \
	retro-pa-player.h \
	retro-rotation.h \
	retro-system-av-info.h \
	retro-system-info.h \
	retro-variable.h \
	$(NULL)

libretro_gtk_la_SOURCES = \
	input/retro-keyboard-key.c \
	\
	core.vala \
	core-error.vala \
	retro.vala \
	retro-analog-id.c \
	retro-analog-index.c \
	retro-cairo-display.c \
	retro-controller-iterator.c \
	retro-core-descriptor.c \
	retro-core-view.c \
	retro-core-view-input-device.c \
	retro-device-type.c \
	retro-game-info.c \
	retro-input-descriptor.c \
	retro-input-device.c \
	retro-joypad-id.c \
	retro-lightgun-id.c \
	retro-log.c \
	retro-main-loop.c \
	retro-memory-type.c \
	retro-module.c \
	retro-module-iterator.c \
	retro-module-query.c \
	retro-mouse-id.c \
	retro-option.c \
	retro-options.c \
	retro-pa-player.c \
	retro-pixel-format.c \
	retro-pointer-id.c \
	retro-rumble-effect.c \
	retro-video-filter.c \
	retro-core.c \
	retro-environment.c \
	\
	video/retro-video-converter.c \
	$(NULL)

video/retro-video-converter.c: retro-gtk-internal.h

retro-core.c: retro-gtk-internal.h

# retro-core-descriptor.c: retro-gtk-internal.h

retro-core-view.c: retro-gtk-internal.h

retro-core-view-input-device.c: retro-gtk-internal.h

retro-environment.c: retro-gtk-internal.h libretro-environment.h

retro-log.c: retro-gtk-internal.h

retro-main-loop.c: retro-gtk-internal.h

retro-module.c: retro-gtk-internal.h

retro-module-iterator.c: retro-gtk-internal.h

retro-module-query.c: retro-gtk-internal.h

retro-pa-player.c: retro-gtk-internal.h

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
	--pkg retro-gtk-c \
	--vapidir=vapi \
	--thread \
	--target-glib 2.32 \
	--library retro-gtk-0.12 \
	-H retro-gtk-vala.h \
	-h retro-gtk-internal.h \
	-X -fPIC -X -shared \
	$(NULL)

libretro_gtk_la_LIBADD = $(RETRO_GTK_LIBS)

retro_gtkincludedir = $(includedir)/retro-gtk-0.12
retro_gtkinclude_HEADERS = \
	$(retro_gtk_public_h_sources) \
	$(NULL)

# Introspection
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

INTROSPECTION_GIRS += Retro-0.12.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

introspection_sources = $(retro_gtkinclude_HEADERS) $(libretro_gtk_la_SOURCES)

BUILT_SOURCES += libretro-gtk.la Retro-0.12.gir

Retro-0.12.gir: $(INTROSPECTION_SCANNER) libretro-gtk.la
Retro_0_12_gir_INCLUDES = GLib-2.0 GObject-2.0 Gio-2.0 Gtk-3.0 cairo-1.0
Retro_0_12_gir_SCANNERFLAGS = --c-include=retro-gtk.h --pkg-export=retro-gtk-0.12
Retro_0_12_gir_CFLAGS = -I$(srcdir) -DRETRO_GTK_USE_UNSTABLE_API -DRETRO_GTK_COMPILATION $(retro_gtk_CFLAGS)
Retro_0_12_gir_LIBS = libretro-gtk.la
Retro_0_12_gir_FILES = $(introspection_sources)
Retro_0_12_gir_EXPORT_PACKAGES = retro-gtk-0.12

retro-gtk-0.12.vapi: Retro-0.12.gir
	vapigen \
		--library retro-gtk-0.12 \
		--pkg cairo \
		--pkg gio-2.0  \
		--pkg glib-2.0 \
		--pkg gmodule-2.0 \
		--pkg gobject-2.0 \
		--pkg gtk+-3.0 \
		--pkg libpulse \
		--pkg libpulse-simple \
		$< \
		$(NULL)

CLEANFILES = $(gir_DATA) $(typelib_DATA) $(vapi_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = retro-gtk-0.12.pc

EXTRA_DIST = \
	$(retro_gtk_private_h_sources) \
	retro-gtk-0.12.pc.in \
	update-from-retroarch.sh \
	vapi/retro-gtk-c.vapi \
	$(NULL)

INPUTDIR=$(top_srcdir)/../RetroArch/libretro-common/include/
INPUTFILES = libretro.h
update-from-retroarch:
	SRCDIR="$(srcdir)" FILES="$(INPUTFILES)" DIR="$(INPUTDIR)" $(srcdir)/update-from-retroarch.sh && changed=true ; \
	cd $(srcdir) && git commit -m "retro-gtk: Update libretro header" $(INPUTFILES)


-include $(top_srcdir)/git.mk
