// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_DEVICE_TYPE_H
#define RETRO_DEVICE_TYPE_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_DEVICE_TYPE (retro_device_type_get_type ())

GType retro_device_type_get_type (void) G_GNUC_CONST;

/**
 * RetroDeviceType:
 * @RETRO_DEVICE_TYPE_NONE: no controller
 * @RETRO_DEVICE_TYPE_JOYPAD: a classic gamepad
 * @RETRO_DEVICE_TYPE_MOUSE: a simple mouse
 * @RETRO_DEVICE_TYPE_KEYBOARD: a keyboard
 * @RETRO_DEVICE_TYPE_LIGHTGUN: a lightgun
 * @RETRO_DEVICE_TYPE_ANALOG: a gamepad with analog sticks
 * @RETRO_DEVICE_TYPE_POINTER: a screen pointer
 * @RETRO_DEVICE_TYPE_TYPE_MASK: a mask to get the super type of a derived one
 *
 * Represents the base types for Libretro controllers.
 */
typedef enum
{
  RETRO_DEVICE_TYPE_NONE,
  RETRO_DEVICE_TYPE_JOYPAD,
  RETRO_DEVICE_TYPE_MOUSE,
  RETRO_DEVICE_TYPE_KEYBOARD,
  RETRO_DEVICE_TYPE_LIGHTGUN,
  RETRO_DEVICE_TYPE_ANALOG,
  RETRO_DEVICE_TYPE_POINTER,
  RETRO_DEVICE_TYPE_TYPE_MASK = 0xff,
} RetroDeviceType;

G_END_DECLS

#endif /* RETRO_DEVICE_TYPE_H */
