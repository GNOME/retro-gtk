// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-pixel-format-private.h"

gboolean
retro_pixel_format_to_gl (RetroPixelFormat  pixel_format,
                          GLenum           *format,
                          GLenum           *type,
                          gint             *pixel_size)
{
  GLenum out_format, out_type;
  gint out_pixel_size;

  switch (pixel_format) {
  case RETRO_PIXEL_FORMAT_XRGB1555:
    out_format = GL_BGRA;
    out_type =  GL_UNSIGNED_SHORT_5_5_5_1;
    out_pixel_size = 2;

    break;
  case RETRO_PIXEL_FORMAT_XRGB8888:
    out_format = GL_BGRA;
    out_type = GL_UNSIGNED_BYTE;
    out_pixel_size = 4;

    break;
  case RETRO_PIXEL_FORMAT_RGB565:
    out_format = GL_RGB;
    out_type =  GL_UNSIGNED_SHORT_5_6_5;
    out_pixel_size = 2;

    break;
  default:
    return FALSE;
  }

  if (format)
    *format = out_format;

  if (type)
    *type = out_type;

  if (pixel_size)
    *pixel_size = out_pixel_size;

  return TRUE;
}
