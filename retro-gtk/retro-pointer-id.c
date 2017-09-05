// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-pointer-id.h"

GType
retro_pointer_id_get_type (void)
{
  static volatile gsize retro_pointer_id_type = 0;

  if (g_once_init_enter (&retro_pointer_id_type)) {
    static const GEnumValue values[] = {
      { RETRO_POINTER_ID_X, "RETRO_POINTER_ID_X", "x" },
      { RETRO_POINTER_ID_Y, "RETRO_POINTER_ID_Y", "y" },
      { RETRO_POINTER_ID_PRESSED, "RETRO_POINTER_ID_PRESSED", "pressed" },
      { 0, NULL, NULL },
    };
    GType type;

    type = g_enum_register_static ("RetroPointerId", values);

    g_once_init_leave (&retro_pointer_id_type, type);
  }

  return retro_pointer_id_type;
}
