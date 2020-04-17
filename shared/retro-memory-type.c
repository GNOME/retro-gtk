// This file is part of retro-gtk. License: GPL-3.0+.

/**
 * SECTION:retro-memory-type
 * @short_description: A memory type enumeration
 * @title: RetroMemoryType
 * @See_also: #RetroCore
 */

#include "retro-memory-type.h"

GType
retro_memory_type_get_type (void)
{
  static volatile gsize retro_memory_type_type = 0;

  if (g_once_init_enter (&retro_memory_type_type)) {
    static const GEnumValue values[] = {
      { RETRO_MEMORY_TYPE_SAVE_RAM, "RETRO_MEMORY_TYPE_SAVE_RAM", "save-ram" },
      { RETRO_MEMORY_TYPE_RTC, "RETRO_MEMORY_TYPE_RTC", "rtc" },
      { RETRO_MEMORY_TYPE_SYSTEM_RAM, "RETRO_MEMORY_TYPE_SYSTEM_RAM", "system-ram" },
      { RETRO_MEMORY_TYPE_VIDEO_RAM, "RETRO_MEMORY_TYPE_VIDEO_RAM", "video-ram" },
      { 0, NULL, NULL },
    };
    GType type;

    type = g_enum_register_static ("RetroMemoryType", values);

    g_once_init_leave (&retro_memory_type_type, type);
  }

  return retro_memory_type_type;
}
