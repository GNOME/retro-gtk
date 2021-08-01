// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include <epoxy/gl.h>

G_BEGIN_DECLS

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
  RETRO_PIXEL_FORMAT_UNKNOWN = G_MAXINT,
} RetroPixelFormat;

gboolean retro_pixel_format_to_gl (RetroPixelFormat  pixel_format,
                                   GLenum           *format,
                                   GLenum           *type,
                                   gint             *pixel_size);

G_END_DECLS
