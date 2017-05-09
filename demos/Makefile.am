NULL =

bin_PROGRAMS = retro-demo

retro_demo_SOURCES = \
	retro-demo.c \
	$(NULL)

retro_demo_CFLAGS = \
	$(RETRO_GTK_CFLAGS) \
	$(GAMEPADS_CFLAGS) \
	$(UDEV_CFLAGS) \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\" \
	-DGAMES_PLUGINS_DIR=\"$(libdir)/gnome-games/plugins\" \
	$(NULL)

retro_demo_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)/retro-gtk \
	-I$(top_srcdir)/retro-gtk \
	-DRETRO_GTK_USE_UNSTABLE_API \
	$(NULL)

retro_demo_DEPENDENCIES = \
	$(top_builddir)/retro-gtk/libretro-gtk.la \
	$(NULL)

retro_demo_LDADD = \
	$(RETRO_GTK_LIBS) \
	$(top_builddir)/retro-gtk/libretro-gtk.la \
	$(NULL)

-include $(top_srcdir)/git.mk
