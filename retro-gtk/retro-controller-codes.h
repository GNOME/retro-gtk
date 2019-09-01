// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_JOYPAD_ID (retro_joypad_id_get_type ())

GType retro_joypad_id_get_type (void) G_GNUC_CONST;

/**
 * RetroJoypadId:
 * @RETRO_JOYPAD_ID_B: the bottom action button
 * @RETRO_JOYPAD_ID_Y: the left action button
 * @RETRO_JOYPAD_ID_SELECT: the Select button
 * @RETRO_JOYPAD_ID_START: the Start button
 * @RETRO_JOYPAD_ID_UP: the up directional button
 * @RETRO_JOYPAD_ID_DOWN: the down directional button
 * @RETRO_JOYPAD_ID_LEFT: the left directional button
 * @RETRO_JOYPAD_ID_RIGHT: the right directional button
 * @RETRO_JOYPAD_ID_A: the right action button
 * @RETRO_JOYPAD_ID_X: the top action button
 * @RETRO_JOYPAD_ID_L: the first left shoulder button
 * @RETRO_JOYPAD_ID_R: the first right shoulder button
 * @RETRO_JOYPAD_ID_L2: the second left shoulder button
 * @RETRO_JOYPAD_ID_R2: the second right shoulder button
 * @RETRO_JOYPAD_ID_L3: the left thumb button
 * @RETRO_JOYPAD_ID_R3: the right thumb button
 * @RETRO_JOYPAD_ID_COUNT: the number of ids
 *
 * Represents the buttons for the Libretro gamepad. The available buttons are
 * the same as the W3C Standard Gamepad, the SDL GameController and many other
 * modern standard gamepads, but the name of the buttons may differ. See the
 * <ulink url="https://w3c.github.io/gamepad/">W3C Gamepad Specification</ulink>
 * and the <ulink url="https://wiki.libsdl.org/CategoryGameController">SDL
 * GamepadController definition</ulink> for more information.
 */
typedef enum
{
  RETRO_JOYPAD_ID_B,
  RETRO_JOYPAD_ID_Y,
  RETRO_JOYPAD_ID_SELECT,
  RETRO_JOYPAD_ID_START,
  RETRO_JOYPAD_ID_UP,
  RETRO_JOYPAD_ID_DOWN,
  RETRO_JOYPAD_ID_LEFT,
  RETRO_JOYPAD_ID_RIGHT,
  RETRO_JOYPAD_ID_A,
  RETRO_JOYPAD_ID_X,
  RETRO_JOYPAD_ID_L,
  RETRO_JOYPAD_ID_R,
  RETRO_JOYPAD_ID_L2,
  RETRO_JOYPAD_ID_R2,
  RETRO_JOYPAD_ID_L3,
  RETRO_JOYPAD_ID_R3,
  RETRO_JOYPAD_ID_COUNT,
} RetroJoypadId;

guint16 retro_joypad_id_to_button_code (RetroJoypadId joypad_id);
RetroJoypadId retro_joypad_id_from_button_code (guint16 button_code);

#define RETRO_TYPE_MOUSE_ID (retro_mouse_id_get_type ())

GType retro_mouse_id_get_type (void) G_GNUC_CONST;

/**
 * RetroMouseId:
 * @RETRO_MOUSE_ID_X: the X axis of a mouse
 * @RETRO_MOUSE_ID_Y: the Y axis of a mouse
 * @RETRO_MOUSE_ID_LEFT: the left button of a mouse
 * @RETRO_MOUSE_ID_RIGHT: the right button of a mouse
 * @RETRO_MOUSE_ID_WHEELUP: the up direction of a mouse wheel
 * @RETRO_MOUSE_ID_WHEELDOWN: the down direction of a mouse wheel
 * @RETRO_MOUSE_ID_MIDDLE: the middle button of a mouse
 * @RETRO_MOUSE_ID_HORIZ_WHEELUP: the horizontal up direction of a mouse wheel
 * @RETRO_MOUSE_ID_HORIZ_WHEELDOWN: the horizontal down direction of a mouse wheel
 * @RETRO_MOUSE_ID_COUNT: the number of ids
 *
 * Represents the inputs for the Libretro mouse.
 */
typedef enum
{
  RETRO_MOUSE_ID_X,
  RETRO_MOUSE_ID_Y,
  RETRO_MOUSE_ID_LEFT,
  RETRO_MOUSE_ID_RIGHT,
  RETRO_MOUSE_ID_WHEELUP,
  RETRO_MOUSE_ID_WHEELDOWN,
  RETRO_MOUSE_ID_MIDDLE,
  RETRO_MOUSE_ID_HORIZ_WHEELUP,
  RETRO_MOUSE_ID_HORIZ_WHEELDOWN,
  RETRO_MOUSE_ID_COUNT,
} RetroMouseId;

#define RETRO_TYPE_LIGHTGUN_ID (retro_lightgun_id_get_type ())

GType retro_lightgun_id_get_type (void) G_GNUC_CONST;

/**
 * RetroLightgunId:
 * @RETRO_LIGHTGUN_ID_X: the X axis of a lightgun
 * @RETRO_LIGHTGUN_ID_Y: the Y axis of a lightgun
 * @RETRO_LIGHTGUN_ID_TRIGGER: the trigger of a lightgun
 * @RETRO_LIGHTGUN_ID_CURSOR: the cursor of a lightgun
 * @RETRO_LIGHTGUN_ID_TURBO: the turbo button of a lightgun
 * @RETRO_LIGHTGUN_ID_PAUSE: the pause button of a lightgun
 * @RETRO_LIGHTGUN_ID_START: the start button of a lightgun
 * @RETRO_LIGHTGUN_ID_COUNT: the number of ids
 *
 * Represents the inputs for the Libretro lightgun.
 */
typedef enum
{
  RETRO_LIGHTGUN_ID_X,
  RETRO_LIGHTGUN_ID_Y,
  RETRO_LIGHTGUN_ID_TRIGGER,
  RETRO_LIGHTGUN_ID_CURSOR,
  RETRO_LIGHTGUN_ID_TURBO,
  RETRO_LIGHTGUN_ID_PAUSE,
  RETRO_LIGHTGUN_ID_START,
  RETRO_LIGHTGUN_ID_COUNT,
} RetroLightgunId;

#define RETRO_TYPE_ANALOG_ID (retro_analog_id_get_type ())

GType retro_analog_id_get_type (void) G_GNUC_CONST;

/**
 * RetroAnalogId:
 * @RETRO_ANALOG_ID_X: the X axis of an analog stick
 * @RETRO_ANALOG_ID_Y: the Y axis of an analog stick
 * @RETRO_ANALOG_ID_COUNT: the number of ids
 *
 * Represents the axes of the analog sticks for the Libretro gamepad.
 */
typedef enum
{
  RETRO_ANALOG_ID_X,
  RETRO_ANALOG_ID_Y,
  RETRO_ANALOG_ID_COUNT,
} RetroAnalogId;

#define RETRO_TYPE_ANALOG_INDEX (retro_analog_index_get_type ())

GType retro_analog_index_get_type (void) G_GNUC_CONST;

/**
 * RetroAnalogIndex:
 * @RETRO_ANALOG_INDEX_LEFT: the left analog stick
 * @RETRO_ANALOG_INDEX_RIGHT: the right analog stick
 * @RETRO_ANALOG_INDEX_COUNT: the number of indexes
 *
 * Represents the analog sticks for the Libretro gamepad.
 */
typedef enum
{
  RETRO_ANALOG_INDEX_LEFT,
  RETRO_ANALOG_INDEX_RIGHT,
  RETRO_ANALOG_INDEX_COUNT,
} RetroAnalogIndex;

#define RETRO_TYPE_POINTER_ID (retro_pointer_id_get_type ())

GType retro_pointer_id_get_type (void) G_GNUC_CONST;

/**
 * RetroPointerId:
 * @RETRO_POINTER_ID_X: the X axis of a pointer
 * @RETRO_POINTER_ID_Y: the Y axis of a pointer
 * @RETRO_POINTER_ID_PRESSED: the pression of a pointer
 * @RETRO_POINTER_ID_COUNT: the number of ids
 *
 * Represents the inputs for the Libretro pointer.
 */
typedef enum
{
  RETRO_POINTER_ID_X,
  RETRO_POINTER_ID_Y,
  RETRO_POINTER_ID_PRESSED,
  RETRO_POINTER_ID_COUNT,
} RetroPointerId;

G_END_DECLS
