// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-joypad-id.h"

GType
retro_joypad_id_get_type (void)
{
  static volatile gsize retro_joypad_id_type = 0;

  if (g_once_init_enter (&retro_joypad_id_type)) {
    static const GEnumValue values[] = {
      { RETRO_JOYPAD_ID_B, "RETRO_JOYPAD_ID_B", "b" },
      { RETRO_JOYPAD_ID_Y, "RETRO_JOYPAD_ID_Y", "y" },
      { RETRO_JOYPAD_ID_SELECT, "RETRO_JOYPAD_ID_SELECT", "select" },
      { RETRO_JOYPAD_ID_START, "RETRO_JOYPAD_ID_START", "start" },
      { RETRO_JOYPAD_ID_UP, "RETRO_JOYPAD_ID_UP", "up" },
      { RETRO_JOYPAD_ID_DOWN, "RETRO_JOYPAD_ID_DOWN", "down" },
      { RETRO_JOYPAD_ID_LEFT, "RETRO_JOYPAD_ID_LEFT", "left" },
      { RETRO_JOYPAD_ID_RIGHT, "RETRO_JOYPAD_ID_RIGHT", "right" },
      { RETRO_JOYPAD_ID_A, "RETRO_JOYPAD_ID_A", "a" },
      { RETRO_JOYPAD_ID_X, "RETRO_JOYPAD_ID_X", "x" },
      { RETRO_JOYPAD_ID_L, "RETRO_JOYPAD_ID_L", "l" },
      { RETRO_JOYPAD_ID_R, "RETRO_JOYPAD_ID_R", "r" },
      { RETRO_JOYPAD_ID_L2, "RETRO_JOYPAD_ID_L2", "l2" },
      { RETRO_JOYPAD_ID_R2, "RETRO_JOYPAD_ID_R2", "r2" },
      { RETRO_JOYPAD_ID_L3, "RETRO_JOYPAD_ID_L3", "l3" },
      { RETRO_JOYPAD_ID_R3, "RETRO_JOYPAD_ID_R3", "r3" },
      { RETRO_JOYPAD_ID_COUNT, "RETRO_JOYPAD_ID_COUNT", "count" },
      { 0, NULL, NULL },
    };
    GType type;

    type = g_enum_register_static ("RetroJoypadId", values);

    g_once_init_leave (&retro_joypad_id_type, type);
  }

  return retro_joypad_id_type;
}
