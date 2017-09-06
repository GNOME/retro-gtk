// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-pixel-format.h"

GType
retro_pixel_format_get_type (void)
{
  static volatile gsize retro_pixel_format_type = 0;

  if (g_once_init_enter (&retro_pixel_format_type)) {
    static const GEnumValue values[] = {
      { RETRO_PIXEL_FORMAT_XRGB1555, "RETRO_PIXEL_FORMAT_XRGB1555", "xrgb1555" },
      { RETRO_PIXEL_FORMAT_XRGB8888, "RETRO_PIXEL_FORMAT_XRGB8888", "xrgb8888" },
      { RETRO_PIXEL_FORMAT_RGB565, "RETRO_PIXEL_FORMAT_RGB565", "rgb565" },
      { RETRO_PIXEL_FORMAT_UNKNOWN, "RETRO_PIXEL_FORMAT_UNKNOWN", "unknown" },
      { 0, NULL, NULL },
    };
    GType type;

    type = g_enum_register_static ("RetroPixelFormat", values);

    g_once_init_leave (&retro_pixel_format_type, type);
  }

  return retro_pixel_format_type;
}
