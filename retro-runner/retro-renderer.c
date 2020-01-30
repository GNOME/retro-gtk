// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-renderer-private.h"

G_DEFINE_INTERFACE (RetroRenderer, retro_renderer, G_TYPE_OBJECT);

static void
retro_renderer_default_init (RetroRendererInterface *iface)
{
}

void
retro_renderer_realize (RetroRenderer *self,
                        guint          width,
                        guint          height)
{
  RetroRendererInterface *iface;

  g_return_if_fail (RETRO_IS_RENDERER (self));

  iface = RETRO_RENDERER_GET_IFACE (self);

  g_return_if_fail (iface->realize != NULL);

  iface->realize (self, width, height);
}

RetroProcAddress
retro_renderer_get_proc_address (RetroRenderer *self,
                                 const gchar   *sym)
{
  RetroRendererInterface *iface;

  g_return_val_if_fail (RETRO_IS_RENDERER (self), NULL);

  iface = RETRO_RENDERER_GET_IFACE (self);

  g_return_val_if_fail (iface->get_proc_address != NULL, NULL);

  return iface->get_proc_address (self, sym);
}

guintptr
retro_renderer_get_current_framebuffer (RetroRenderer *self)
{
  RetroRendererInterface *iface;

  g_return_val_if_fail (RETRO_IS_RENDERER (self), 0);

  iface = RETRO_RENDERER_GET_IFACE (self);

  g_return_val_if_fail (iface->get_current_framebuffer != NULL, 0);

  return iface->get_current_framebuffer (self);
}

void
retro_renderer_snapshot (RetroRenderer    *self,
                         RetroPixelFormat  pixel_format,
                         guint             width,
                         guint             height,
                         gsize             rowstride,
                         guint8           *data)
{
  RetroRendererInterface *iface;

  g_return_if_fail (RETRO_IS_RENDERER (self));

  iface = RETRO_RENDERER_GET_IFACE (self);

  g_return_if_fail (iface->snapshot != NULL);

  iface->snapshot (self, pixel_format, width, height, rowstride, data);
}
