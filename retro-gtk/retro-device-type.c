// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-device-type.h"

GType
retro_device_type_get_type (void)
{
  static volatile gsize retro_device_type_type = 0;

  if (g_once_init_enter (&retro_device_type_type)) {
    static const GEnumValue values[] = {
      { RETRO_DEVICE_TYPE_NONE, "RETRO_DEVICE_TYPE_NONE", "none" },
      { RETRO_DEVICE_TYPE_JOYPAD, "RETRO_DEVICE_TYPE_JOYPAD", "joypad" },
      { RETRO_DEVICE_TYPE_MOUSE, "RETRO_DEVICE_TYPE_MOUSE", "mouse" },
      { RETRO_DEVICE_TYPE_KEYBOARD, "RETRO_DEVICE_TYPE_KEYBOARD", "keyboard" },
      { RETRO_DEVICE_TYPE_LIGHTGUN, "RETRO_DEVICE_TYPE_LIGHTGUN", "lightgun" },
      { RETRO_DEVICE_TYPE_ANALOG, "RETRO_DEVICE_TYPE_ANALOG", "analog" },
      { RETRO_DEVICE_TYPE_POINTER, "RETRO_DEVICE_TYPE_POINTER", "pointer" },
      { RETRO_DEVICE_TYPE_TYPE_MASK, "RETRO_DEVICE_TYPE_TYPE_MASK", "type-mask" },
      { 0, NULL, NULL },
    };
    GType type;

    type = g_enum_register_static ("RetroDeviceType", values);

    g_once_init_leave (&retro_device_type_type, type);
  }

  return retro_device_type_type;
}
