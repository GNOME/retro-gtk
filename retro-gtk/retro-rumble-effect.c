// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-rumble-effect.h"

GType
retro_rumble_effect_get_type (void)
{
  static volatile gsize retro_rumble_effect_type = 0;

  if (g_once_init_enter (&retro_rumble_effect_type)) {
    static const GEnumValue values[] = {
      { RETRO_RUMBLE_EFFECT_STRONG, "RETRO_RUMBLE_EFFECT_STRONG", "strong" },
      { RETRO_RUMBLE_EFFECT_WEAK, "RETRO_RUMBLE_EFFECT_WEAK", "weak" },
      { RETRO_RUMBLE_EFFECT_COUNT, "RETRO_RUMBLE_EFFECT_COUNT", "count" },
      { 0, NULL, NULL },
    };
    GType type;

    type = g_enum_register_static ("RetroRumbleEffect", values);

    g_once_init_leave (&retro_rumble_effect_type, type);
  }

  return retro_rumble_effect_type;
}
