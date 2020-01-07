// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_CONTROLLER_TYPE (retro_controller_type_get_type ())

GType retro_controller_type_get_type (void) G_GNUC_CONST;

/**
 * RetroControllerType:
 * @RETRO_CONTROLLER_TYPE_NONE: no controller
 * @RETRO_CONTROLLER_TYPE_JOYPAD: a classic gamepad
 * @RETRO_CONTROLLER_TYPE_MOUSE: a simple mouse
 * @RETRO_CONTROLLER_TYPE_KEYBOARD: a keyboard
 * @RETRO_CONTROLLER_TYPE_LIGHTGUN: a lightgun
 * @RETRO_CONTROLLER_TYPE_ANALOG: a gamepad with analog sticks
 * @RETRO_CONTROLLER_TYPE_POINTER: a screen pointer
 * @RETRO_CONTROLLER_TYPE_COUNT: the number of types
 * @RETRO_CONTROLLER_TYPE_TYPE_MASK: a mask to get the super type of a derived one
 *
 * Represents the base types for Libretro controllers.
 */
typedef enum
{
  RETRO_CONTROLLER_TYPE_NONE,
  RETRO_CONTROLLER_TYPE_JOYPAD,
  RETRO_CONTROLLER_TYPE_MOUSE,
  RETRO_CONTROLLER_TYPE_KEYBOARD,
  RETRO_CONTROLLER_TYPE_LIGHTGUN,
  RETRO_CONTROLLER_TYPE_ANALOG,
  RETRO_CONTROLLER_TYPE_POINTER,
  RETRO_CONTROLLER_TYPE_COUNT,
  RETRO_CONTROLLER_TYPE_TYPE_MASK = 0xff,
} RetroControllerType;

G_END_DECLS
