// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-gamepad-input.h"

void
retro_gamepad_configuration_set_to_default (RetroGamepadConfiguration *self)
{
  g_return_if_fail (self != NULL);

  retro_gamepad_configuration_set_button_key (self, BTN_A, KEY_S);
  retro_gamepad_configuration_set_button_key (self, BTN_Y, KEY_A);
  retro_gamepad_configuration_set_button_key (self, BTN_SELECT, KEY_BACKSPACE);
  retro_gamepad_configuration_set_button_key (self, BTN_START, KEY_ENTER);
  retro_gamepad_configuration_set_button_key (self, BTN_DPAD_UP, KEY_UP);
  retro_gamepad_configuration_set_button_key (self, BTN_DPAD_DOWN, KEY_DOWN);
  retro_gamepad_configuration_set_button_key (self, BTN_DPAD_LEFT, KEY_LEFT);
  retro_gamepad_configuration_set_button_key (self, BTN_DPAD_RIGHT, KEY_RIGHT);
  retro_gamepad_configuration_set_button_key (self, BTN_B, KEY_D);
  retro_gamepad_configuration_set_button_key (self, BTN_X, KEY_W);
  retro_gamepad_configuration_set_button_key (self, BTN_TL, KEY_Q);
  retro_gamepad_configuration_set_button_key (self, BTN_TR, KEY_E);
  retro_gamepad_configuration_set_button_key (self, BTN_TL2, KEY_Z);
  retro_gamepad_configuration_set_button_key (self, BTN_TR2, KEY_C);
  retro_gamepad_configuration_set_button_key (self, BTN_THUMBL, KEY_1);
  retro_gamepad_configuration_set_button_key (self, BTN_THUMBR, KEY_3);
}

void
retro_gamepad_configuration_set_button_key (RetroGamepadConfiguration *self,
                                            guint16                    button,
                                            guint16                    key)
{
  RetroJoypadId retro_button;

  g_return_if_fail (self != NULL);

  retro_button = retro_gamepad_button_converter (button);

  if (retro_button == RETRO_JOYPAD_ID_COUNT) {
    self->gamepad_keys[retro_button] = 0;

    return;
  }

  // GDK adds 8 to the Linux input event codes to create the hardware keycode.
  self->gamepad_keys[retro_button] = key + 8;
}

guint16
retro_gamepad_configuration_get_button_key (RetroGamepadConfiguration *self,
                                            guint16                    button)
{
  RetroJoypadId retro_button;

  g_return_val_if_fail (self != NULL, 0);

  retro_button = retro_gamepad_button_converter (button);

  if (retro_button == RETRO_JOYPAD_ID_COUNT)
    return 0;

  return self->gamepad_keys[retro_button];

}
