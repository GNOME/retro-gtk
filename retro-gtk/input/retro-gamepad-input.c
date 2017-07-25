// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-gamepad-input.h"

RetroJoypadId
retro_gamepad_button_converter (guint16 button)
{
  switch (button) {
  case BTN_A:
    return RETRO_JOYPAD_ID_B;
  case BTN_Y:
    return RETRO_JOYPAD_ID_Y;
  case BTN_SELECT:
    return RETRO_JOYPAD_ID_SELECT;
  case BTN_START:
    return RETRO_JOYPAD_ID_START;
  case BTN_DPAD_UP:
    return RETRO_JOYPAD_ID_UP;
  case BTN_DPAD_DOWN:
    return RETRO_JOYPAD_ID_DOWN;
  case BTN_DPAD_LEFT:
    return RETRO_JOYPAD_ID_LEFT;
  case BTN_DPAD_RIGHT:
    return RETRO_JOYPAD_ID_RIGHT;
  case BTN_B:
    return RETRO_JOYPAD_ID_A;
  case BTN_X:
    return RETRO_JOYPAD_ID_X;
  case BTN_TL:
    return RETRO_JOYPAD_ID_L;
  case BTN_TR:
    return RETRO_JOYPAD_ID_R;
  case BTN_TL2:
    return RETRO_JOYPAD_ID_L2;
  case BTN_TR2:
    return RETRO_JOYPAD_ID_R2;
  case BTN_THUMBL:
    return RETRO_JOYPAD_ID_L3;
  case BTN_THUMBR:
    return RETRO_JOYPAD_ID_R3;
  default:
    return RETRO_JOYPAD_ID_COUNT;
  }

  return RETRO_JOYPAD_ID_COUNT;
}
