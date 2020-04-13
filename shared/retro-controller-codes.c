// This file is part of retro-gtk. License: GPL-3.0+.

/**
 * SECTION:retro-controller-codes
 * @short_description: Controller input codes and Linux convertion functions
 * @title: RetroController Codes
 * @See_also: #RetroController
 */

#include <linux/input-event-codes.h>
#include "retro-controller-codes.h"

static guint16 RETRO_JOYPAD_ID_EVENT_CODE_MAPPING[RETRO_JOYPAD_ID_COUNT] = {
  BTN_A,
  BTN_Y,
  BTN_SELECT,
  BTN_START,
  BTN_DPAD_UP,
  BTN_DPAD_DOWN,
  BTN_DPAD_LEFT,
  BTN_DPAD_RIGHT,
  BTN_B,
  BTN_X,
  BTN_TL,
  BTN_TR,
  BTN_TL2,
  BTN_TR2,
  BTN_THUMBL,
  BTN_THUMBR,
};

/**
 * retro_joypad_id_to_button_code:
 * @joypad_id: a #RetroJoypadId
 *
 * Returns: the corresponding button code
 */
guint16
retro_joypad_id_to_button_code (RetroJoypadId joypad_id)
{
  g_return_val_if_fail (joypad_id >= 0, EV_MAX);
  g_return_val_if_fail (joypad_id < RETRO_JOYPAD_ID_COUNT, EV_MAX);

  return RETRO_JOYPAD_ID_EVENT_CODE_MAPPING[joypad_id];
}

/**
 * retro_joypad_id_from_button_code:
 * @button_code: a button code
 *
 * Returns: the corresponding joypad id
 */
RetroJoypadId
retro_joypad_id_from_button_code (guint16 button_code)
{
  for (RetroJoypadId id = 0; id < RETRO_JOYPAD_ID_COUNT; id++)
    if (button_code == RETRO_JOYPAD_ID_EVENT_CODE_MAPPING[id])
      return id;

  return RETRO_JOYPAD_ID_COUNT;
}
