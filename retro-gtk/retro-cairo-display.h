// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_CAIRO_DISPLAY_H
#define RETRO_CAIRO_DISPLAY_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include "retro-core.h"
#include "retro-video-filter.h"

G_BEGIN_DECLS

#define RETRO_TYPE_CAIRO_DISPLAY (retro_cairo_display_get_type())

G_DECLARE_FINAL_TYPE (RetroCairoDisplay, retro_cairo_display, RETRO, CAIRO_DISPLAY, GtkDrawingArea)

RetroCairoDisplay *retro_cairo_display_new (void);
GdkPixbuf *retro_cairo_display_get_pixbuf (RetroCairoDisplay *self);
void retro_cairo_display_set_pixbuf (RetroCairoDisplay *self,
                                     GdkPixbuf         *pixbuf);
void retro_cairo_display_set_core (RetroCairoDisplay *self,
                                   RetroCore         *core);
void retro_cairo_display_set_filter (RetroCairoDisplay *self,
                                     RetroVideoFilter   filter);
gboolean retro_cairo_display_get_coordinates_on_display (RetroCairoDisplay *self,
                                                         gdouble            widget_x,
                                                         gdouble            widget_y,
                                                         gdouble           *display_x,
                                                         gdouble           *display_y);
void retro_cairo_display_show_video (RetroCairoDisplay *self);
void retro_cairo_display_hide_video (RetroCairoDisplay *self);

G_END_DECLS

#endif /* RETRO_CAIRO_DISPLAY_H */

