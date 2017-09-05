// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-mouse-id.h"

GType
retro_mouse_id_get_type (void)
{
  static volatile gsize retro_mouse_id_type = 0;

  if (g_once_init_enter (&retro_mouse_id_type)) {
    static const GEnumValue values[] = {
      { RETRO_MOUSE_ID_X, "RETRO_MOUSE_ID_X", "x" },
      { RETRO_MOUSE_ID_Y, "RETRO_MOUSE_ID_Y", "y" },
      { RETRO_MOUSE_ID_LEFT, "RETRO_MOUSE_ID_LEFT", "left" },
      { RETRO_MOUSE_ID_RIGHT, "RETRO_MOUSE_ID_RIGHT", "right" },
      { RETRO_MOUSE_ID_WHEELUP, "RETRO_MOUSE_ID_WHEELUP", "wheelup" },
      { RETRO_MOUSE_ID_WHEELDOWN, "RETRO_MOUSE_ID_WHEELDOWN", "wheeldown" },
      { RETRO_MOUSE_ID_MIDDLE, "RETRO_MOUSE_ID_MIDDLE", "middle" },
      { RETRO_MOUSE_ID_HORIZ_WHEELUP, "RETRO_MOUSE_ID_HORIZ_WHEELUP", "horiz-wheelup" },
      { RETRO_MOUSE_ID_HORIZ_WHEELDOWN, "RETRO_MOUSE_ID_HORIZ_WHEELDOWN", "horiz-wheeldown" },
      { 0, NULL, NULL },
    };
    GType type;

    type = g_enum_register_static ("RetroMouseId", values);

    g_once_init_leave (&retro_mouse_id_type, type);
  }

  return retro_mouse_id_type;
}
