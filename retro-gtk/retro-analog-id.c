// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-analog-id.h"

GType
retro_analog_id_get_type (void)
{
  static volatile gsize retro_analog_id_type = 0;

  if (g_once_init_enter (&retro_analog_id_type)) {
    static const GEnumValue values[] = {
      { RETRO_ANALOG_ID_X, "RETRO_ANALOG_ID_X", "x" },
      { RETRO_ANALOG_ID_Y, "RETRO_ANALOG_ID_Y", "y" },
      { 0, NULL, NULL },
    };
    GType type;

    type = g_enum_register_static ("RetroAnalogId", values);

    g_once_init_leave (&retro_analog_id_type, type);
  }

  return retro_analog_id_type;
}
