// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-pixel-format-private.h"

G_BEGIN_DECLS

#define RETRO_TYPE_FRAMEBUFFER (retro_framebuffer_get_type())

G_DECLARE_FINAL_TYPE (RetroFramebuffer, retro_framebuffer, RETRO, FRAMEBUFFER, GObject)

RetroFramebuffer *retro_framebuffer_new (gint fd);

gint retro_framebuffer_get_fd (RetroFramebuffer *self);

void retro_framebuffer_lock (RetroFramebuffer *self);
void retro_framebuffer_unlock (RetroFramebuffer *self);

#ifdef RETRO_RUNNER_COMPILATION

void retro_framebuffer_set_data (RetroFramebuffer *self,
                                 RetroPixelFormat  format,
                                 gsize             rowstride,
                                 guint             width,
                                 guint             height,
                                 gfloat            aspect_ratio,
                                 gpointer          data);

#else

gboolean retro_framebuffer_get_is_dirty (RetroFramebuffer *self);
RetroPixelFormat retro_framebuffer_get_format (RetroFramebuffer *self);
gsize retro_framebuffer_get_rowstride (RetroFramebuffer *self);
guint retro_framebuffer_get_width (RetroFramebuffer *self);
guint retro_framebuffer_get_height (RetroFramebuffer *self);
gdouble retro_framebuffer_get_aspect_ratio (RetroFramebuffer *self);
gconstpointer retro_framebuffer_get_pixels (RetroFramebuffer *self);

#endif

G_END_DECLS
