// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_DISPLAY_H
#define RETRO_DISPLAY_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include "retro-core.h"
#include "retro-video-filter.h"

G_BEGIN_DECLS

#define RETRO_TYPE_DISPLAY (retro_display_get_type())

G_DECLARE_INTERFACE (RetroDisplay, retro_display, RETRO, DISPLAY, GtkWidget)

struct _RetroDisplayInterface
{
  GTypeInterface parent_iface;

  GdkPixbuf *(*get_pixbuf) (RetroDisplay *self);
  void (*set_pixbuf) (RetroDisplay *self,
                      GdkPixbuf    *pixbuf);
  void (*set_core) (RetroDisplay *self,
                    RetroCore    *core);
  void (*set_filter) (RetroDisplay     *self,
                      RetroVideoFilter  filter);
  gboolean (*get_coordinates_on_display) (RetroDisplay *self,
                                          gdouble       widget_x,
                                          gdouble       widget_y,
                                          gdouble      *display_x,
                                          gdouble      *display_y);
};

GdkPixbuf *retro_display_get_pixbuf (RetroDisplay *self);
void retro_display_set_pixbuf (RetroDisplay *self,
                               GdkPixbuf    *pixbuf);
void retro_display_set_core (RetroDisplay *self,
                             RetroCore    *core);
void retro_display_set_filter (RetroDisplay     *self,
                               RetroVideoFilter  filter);
gboolean retro_display_get_coordinates_on_display (RetroDisplay *self,
                                                   gdouble       widget_x,
                                                   gdouble       widget_y,
                                                   gdouble      *display_x,
                                                   gdouble      *display_y);

G_END_DECLS

#endif /* RETRO_DISPLAY_H */
