// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-lightgun-id.h"

GType
retro_lightgun_id_get_type (void)
{
  static volatile gsize retro_lightgun_id_type = 0;

  if (g_once_init_enter (&retro_lightgun_id_type)) {
    static const GEnumValue values[] = {
      { RETRO_LIGHTGUN_ID_X, "RETRO_LIGHTGUN_ID_X", "x" },
      { RETRO_LIGHTGUN_ID_Y, "RETRO_LIGHTGUN_ID_Y", "y" },
      { RETRO_LIGHTGUN_ID_TRIGGER, "RETRO_LIGHTGUN_ID_TRIGGER", "trigger" },
      { RETRO_LIGHTGUN_ID_CURSOR, "RETRO_LIGHTGUN_ID_CURSOR", "cursor" },
      { RETRO_LIGHTGUN_ID_TURBO, "RETRO_LIGHTGUN_ID_TURBO", "turbo" },
      { RETRO_LIGHTGUN_ID_PAUSE, "RETRO_LIGHTGUN_ID_PAUSE", "pause" },
      { RETRO_LIGHTGUN_ID_START, "RETRO_LIGHTGUN_ID_START", "start" },
      { 0, NULL, NULL },
    };
    GType type;

    type = g_enum_register_static ("RetroLightgunId", values);

    g_once_init_leave (&retro_lightgun_id_type, type);
  }

  return retro_lightgun_id_type;
}
