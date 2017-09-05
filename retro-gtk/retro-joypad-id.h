// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_JOYPAD_ID_H
#define RETRO_JOYPAD_ID_H

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
 * @RETRO_JOYPAD_ID_COUNT: the number of buttons
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

G_END_DECLS

#endif /* RETRO_JOYPAD_ID_H */
