// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_PIXDATA (retro_pixdata_get_type())

GType retro_pixdata_get_type (void) G_GNUC_CONST;

typedef struct _RetroPixdata RetroPixdata;

RetroPixdata *retro_pixdata_copy (RetroPixdata *self);
void retro_pixdata_free (RetroPixdata *self);
gint retro_pixdata_get_width (RetroPixdata *self);
gint retro_pixdata_get_height (RetroPixdata *self);
gfloat retro_pixdata_get_aspect_ratio (RetroPixdata *self);
GdkPixbuf *retro_pixdata_to_pixbuf (RetroPixdata *self);
gboolean retro_pixdata_load_gl_texture (RetroPixdata *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (RetroPixdata, retro_pixdata_free)

G_END_DECLS
