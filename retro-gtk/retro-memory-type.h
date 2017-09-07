// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_MEMORY_TYPE_H
#define RETRO_MEMORY_TYPE_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * RetroMemoryType:
 * @RETRO_MEMORY_TYPE_SAVE_RAM: the save RAM
 * @RETRO_MEMORY_TYPE_RTC: the real time clock
 * @RETRO_MEMORY_TYPE_SYSTEM_RAM: the system RAM
 * @RETRO_MEMORY_TYPE_VIDEO_RAM: the video RAM
 *
 * Represents the memory types for the Libretro core.
 */
typedef enum
{
  RETRO_MEMORY_TYPE_SAVE_RAM,
  RETRO_MEMORY_TYPE_RTC,
  RETRO_MEMORY_TYPE_SYSTEM_RAM,
  RETRO_MEMORY_TYPE_VIDEO_RAM,
} RetroMemoryType;

G_END_DECLS

#endif /* RETRO_MEMORY_TYPE_H */
