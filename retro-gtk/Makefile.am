## Process this file with automake to produce Makefile.in

NULL =

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\""Retro"\" \
	-DRETRO_GTK_COMPILATION \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DPROJECT_NAME=\""retro"\" \
	-DPROJECT_API_VERSION=\""0.14"\" \
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
vapi_DATA = retro-gtk-0.14.vapi
dist_vapi_DATA = retro-gtk-0.14.deps

retro_gtk_public_h_sources = \
	retro-controller.h \
	retro-controller-codes.h \
	retro-controller-iterator.h \
	retro-controller-type.h \
	retro-core-descriptor.h \
	retro-core.h \
	retro-core-view.h \
	retro-gtk.h \
	retro-input.h \
	retro-log.h \
	retro-main-loop.h \
	retro-memory-type.h \
	retro-module-iterator.h \
	retro-module-query.h \
	retro-pixdata.h \
	retro-rumble-effect.h \
	retro-video-filter.h \
	$(NULL)

retro_gtk_private_h_sources = \
	libretro-environment.h \
	retro-cairo-display.h \
	retro-controller-iterator-private.h \
	retro-core-private.h \
	retro-core-view-controller.h \
	retro-disk-control-callback.h \
	retro-game-info.h \
	retro-input-descriptor.h \
	retro-input-private.h \
	retro-keyboard-key.h \
	retro-module.h \
	retro-option.h \
	retro-options.h \
	retro-pa-player.h \
	retro-pixdata-private.h \
	retro-pixel-format.h \
	retro-rotation.h \
	retro-system-av-info.h \
	retro-system-info.h \
	retro-variable.h \
	$(NULL)

libretro_gtk_la_SOURCES = \
	retro-cairo-display.c \
	retro-controller.c \
	retro-controller-codes.c \
	retro-controller-iterator.c \
	retro-controller-type.c \
	retro-core-descriptor.c \
	retro-core-view.c \
	retro-core-view-controller.c \
	retro-game-info.c \
	retro-input.c \
	retro-input-descriptor.c \
	retro-keyboard-key.c \
	retro-log.c \
	retro-main-loop.c \
	retro-memory-type.c \
	retro-module.c \
	retro-module-iterator.c \
	retro-module-query.c \
	retro-option.c \
	retro-options.c \
	retro-pa-player.c \
	retro-pixdata.c \
	retro-pixel-format.c \
	retro-rumble-effect.c \
	retro-video-filter.c \
	retro-core.c \
	retro-environment.c \
	$(NULL)

libretro_gtk_la_LDFLAGS =

libretro_gtk_la_LIBADD = $(RETRO_GTK_LIBS)

retro_gtkincludedir = $(includedir)/retro-gtk-0.14
retro_gtkinclude_HEADERS = \
	$(retro_gtk_public_h_sources) \
	$(NULL)

# Introspection
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

INTROSPECTION_GIRS += Retro-0.14.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

introspection_sources = $(retro_gtkinclude_HEADERS) $(libretro_gtk_la_SOURCES)

BUILT_SOURCES = libretro-gtk.la Retro-0.14.gir

Retro-0.14.gir: $(INTROSPECTION_SCANNER) libretro-gtk.la
Retro_0_14_gir_INCLUDES = GLib-2.0 GObject-2.0 Gio-2.0 Gtk-3.0 cairo-1.0
Retro_0_14_gir_SCANNERFLAGS = --c-include=retro-gtk.h --pkg-export=retro-gtk-0.14
Retro_0_14_gir_CFLAGS = -I$(srcdir) -DRETRO_GTK_USE_UNSTABLE_API -DRETRO_GTK_COMPILATION $(retro_gtk_CFLAGS)
Retro_0_14_gir_LIBS = libretro-gtk.la
Retro_0_14_gir_FILES = $(introspection_sources)
Retro_0_14_gir_EXPORT_PACKAGES = retro-gtk-0.14

retro-gtk-0.14.vapi: Retro-0.14.gir
	vapigen \
		--library retro-gtk-0.14 \
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
pkgconfig_DATA = retro-gtk-0.14.pc

EXTRA_DIST = \
	$(retro_gtk_private_h_sources) \
	retro-gtk-0.14.pc.in \
	update-from-retroarch.sh \
	$(NULL)

INPUTDIR=$(top_srcdir)/../RetroArch/libretro-common/include/
INPUTFILES = libretro.h
update-from-retroarch:
	SRCDIR="$(srcdir)" FILES="$(INPUTFILES)" DIR="$(INPUTDIR)" $(srcdir)/update-from-retroarch.sh && changed=true ; \
	cd $(srcdir) && git commit -m "retro-gtk: Update libretro header" $(INPUTFILES)


-include $(top_srcdir)/git.mk
