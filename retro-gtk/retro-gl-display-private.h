// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include "retro-core.h"
#include "retro-video-filter.h"

G_BEGIN_DECLS

#define RETRO_TYPE_GL_DISPLAY (retro_gl_display_get_type())

G_DECLARE_FINAL_TYPE (RetroGLDisplay, retro_gl_display, RETRO, GL_DISPLAY, GtkGLArea)

RetroGLDisplay *retro_gl_display_new (void) G_GNUC_WARN_UNUSED_RESULT;
GdkPixbuf *retro_gl_display_get_pixbuf (RetroGLDisplay *self);
void retro_gl_display_set_pixbuf (RetroGLDisplay *self,
                                  GdkPixbuf      *pixbuf);
void retro_gl_display_set_core (RetroGLDisplay *self,
                                RetroCore      *core);
void retro_gl_display_set_filter (RetroGLDisplay   *self,
                                  RetroVideoFilter  filter);
gboolean retro_gl_display_get_coordinates_on_display (RetroGLDisplay *self,
                                                      gdouble         widget_x,
                                                      gdouble         widget_y,
                                                      gdouble        *display_x,
                                                      gdouble        *display_y);

G_END_DECLS
