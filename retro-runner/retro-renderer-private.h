// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

#include "retro-framebuffer-private.h"
#include "retro-hw-render-callback-private.h"
#include "retro-pixel-format-private.h"

G_BEGIN_DECLS

#define RETRO_TYPE_RENDERER (retro_renderer_get_type())

G_DECLARE_INTERFACE (RetroRenderer, retro_renderer, RETRO, RENDERER, GObject)

struct _RetroRendererInterface
{
  GTypeInterface parent_iface;

  void (*realize) (RetroRenderer *self,
                   guint          width,
                   guint          height);
  RetroProcAddress (*get_proc_address) (RetroRenderer *self,
                                        const gchar   *sym);
  guintptr (*get_current_framebuffer) (RetroRenderer *self);
  void (*snapshot) (RetroRenderer    *self,
                    RetroPixelFormat  pixel_format,
                    guint             width,
                    guint             height,
                    gsize             rowstride,
                    guint8           *data);
};

void retro_renderer_realize (RetroRenderer *self,
                             guint          width,
                             guint          height);

RetroProcAddress retro_renderer_get_proc_address (RetroRenderer *self,
                                                  const gchar   *sym);

guintptr retro_renderer_get_current_framebuffer (RetroRenderer *self);

void retro_renderer_snapshot (RetroRenderer    *self,
                              RetroPixelFormat  pixel_format,
                              guint             width,
                              guint             height,
                              gsize             rowstride,
                              guint8           *data);

G_END_DECLS
