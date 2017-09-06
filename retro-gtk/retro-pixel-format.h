// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_PIXEL_FORMAT_H
#define RETRO_PIXEL_FORMAT_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_PIXEL_FORMAT (retro_pixel_format_get_type ())

GType retro_pixel_format_get_type (void) G_GNUC_CONST;

/**
 * RetroPixelFormat:
 * @RETRO_PIXEL_FORMAT_XRGB1555: native endian, the first bit is ignored
 * @RETRO_PIXEL_FORMAT_XRGB8888: native endian, the first 8 bits are ignored
 * @RETRO_PIXEL_FORMAT_RGB565: native endian
 * @RETRO_PIXEL_FORMAT_UNKNOWN: an unknown format
 *
 * Represents the pixel formats used by Libretro.
 */
typedef enum
{
  RETRO_PIXEL_FORMAT_XRGB1555,
  RETRO_PIXEL_FORMAT_XRGB8888,
  RETRO_PIXEL_FORMAT_RGB565,
  RETRO_PIXEL_FORMAT_UNKNOWN = -1,
} RetroPixelFormat;

G_END_DECLS

#endif /* RETRO_PIXEL_FORMAT_H */
