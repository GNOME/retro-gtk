// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_PIXDATA_PRIVATE_H
#define RETRO_PIXDATA_PRIVATE_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-pixdata.h"

#include "retro-pixel-format.h"

G_BEGIN_DECLS

struct _RetroPixdata
{
  guint8 *data;
  RetroPixelFormat pixel_format;
  gsize rowstride;
  gsize width;
  gsize height;
  gfloat aspect_ratio;
};

void retro_pixdata_init (RetroPixdata     *self,
                         gconstpointer     data,
                         RetroPixelFormat  pixel_format,
                         gsize             rowstride,
                         gsize             width,
                         gsize             height,
                         gfloat            aspect_ratio);

G_END_DECLS

#endif /* RETRO_PIXDATA_PRIVATE_H */
