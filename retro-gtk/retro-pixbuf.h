// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_PIXBUF_H
#define RETRO_PIXBUF_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib-object.h>

G_BEGIN_DECLS

gfloat retro_pixbuf_get_aspect_ratio (GdkPixbuf *pixbuf);
void   retro_pixbuf_set_aspect_ratio (GdkPixbuf *pixbuf,
                                      gfloat     aspect_ratio);

G_END_DECLS

#endif /* RETRO_PIXBUF_H */
