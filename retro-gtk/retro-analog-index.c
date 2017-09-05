// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-analog-index.h"

GType
retro_analog_index_get_type (void)
{
  static volatile gsize retro_analog_index_type = 0;

  if (g_once_init_enter (&retro_analog_index_type)) {
    static const GEnumValue values[] = {
      { RETRO_ANALOG_INDEX_LEFT, "RETRO_ANALOG_INDEX_LEFT", "left" },
      { RETRO_ANALOG_INDEX_RIGHT, "RETRO_ANALOG_INDEX_RIGHT", "right" },
      { 0, NULL, NULL },
    };
    GType type;

    type = g_enum_register_static ("RetroAnalogIndex", values);

    g_once_init_leave (&retro_analog_index_type, type);
  }

  return retro_analog_index_type;
}
