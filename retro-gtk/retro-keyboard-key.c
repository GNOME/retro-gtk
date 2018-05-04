// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-keyboard-key.h"

RetroKeyboardModifierKey
retro_keyboard_modifier_key_converter (guint           keyval,
                                       GdkModifierType modifiers)
{
  RetroKeyboardModifierKey retro_modifiers = RETRO_KEYBOARD_MODIFIER_KEY_NONE;

  if (modifiers & GDK_SHIFT_MASK)
    retro_modifiers |= RETRO_KEYBOARD_MODIFIER_KEY_SHIFT;
  if (modifiers & GDK_CONTROL_MASK)
    retro_modifiers |= RETRO_KEYBOARD_MODIFIER_KEY_CTRL;
  if (modifiers & GDK_MOD1_MASK)
    retro_modifiers |= RETRO_KEYBOARD_MODIFIER_KEY_ALT;
  if (modifiers & GDK_META_MASK)
    retro_modifiers |= RETRO_KEYBOARD_MODIFIER_KEY_META;
  if (keyval == GDK_KEY_Num_Lock)
    retro_modifiers |= RETRO_KEYBOARD_MODIFIER_KEY_NUMLOCK;
  if (modifiers & GDK_LOCK_MASK)
    retro_modifiers |= RETRO_KEYBOARD_MODIFIER_KEY_CAPSLOCK;
  if (keyval == GDK_KEY_Scroll_Lock)
    retro_modifiers |= RETRO_KEYBOARD_MODIFIER_KEY_SCROLLOCK;

  return retro_modifiers;
}

RetroKeyboardKey
retro_keyboard_key_converter (guint keyval)
{
  /*
   * US-ASCII codes
   *
   * Both GDK_KEY_* and RETRO_KEYBOARD_KEY_* start with the US-ASCII codes,
   * implementing common letters, digits and symbols.
   *
   * If the key is uppercase, turn it lower case as Libretro doesn't make a
   * distinction between these.
   */
  if (keyval < 0x80)
    return (keyval >= GDK_KEY_A && keyval <= GDK_KEY_Z) ?
      keyval + (GDK_KEY_a - GDK_KEY_A) : keyval;

  /*
   * Function keys
   *
   * Both GDK_KEY_F* and RETRO_KEYBOARD_KEY_F* implement function keys 1—15 in a
   * consecutive manner. GDK_KEY_F* actually implements more function keys but
   * RETRO_KEYBOARD_KEY_F* doesn't.
   */
  if (keyval >= GDK_KEY_F1 && keyval <= GDK_KEY_F15)
    return RETRO_KEYBOARD_KEY_F1 + (keyval - GDK_KEY_F1);

  /*
   * Keypad digits
   *
   * Both GDK_KEY_KP_* and RETRO_KEYBOARD_KEY_KP* implement keypad digits 0–9 in
   * a consecutive manner.
   */
  if (keyval >= GDK_KEY_KP_0 && keyval <= GDK_KEY_KP_9)
    return RETRO_KEYBOARD_KEY_KP0 + (keyval - GDK_KEY_KP_0);

  /*
   * Various keys
   *
   * FIXME: The following keys are unimplemented because I couldn't find the
   * right GDK_KEY_* equivalent:
   * - RETRO_KEYBOARD_KEY_MODE
   * - RETRO_KEYBOARD_KEY_COMPOSE
   * - RETRO_KEYBOARD_KEY_POWER
   */
  switch (keyval) {
  case GDK_KEY_BackSpace:
    return RETRO_KEYBOARD_KEY_BACKSPACE;
  case GDK_KEY_Tab:
    return RETRO_KEYBOARD_KEY_TAB;
  case GDK_KEY_Clear:
    return RETRO_KEYBOARD_KEY_CLEAR;
  case GDK_KEY_Return:
    return RETRO_KEYBOARD_KEY_RETURN;
  case GDK_KEY_Pause:
    return RETRO_KEYBOARD_KEY_PAUSE;
  case GDK_KEY_Escape:
    return RETRO_KEYBOARD_KEY_ESCAPE;
  case GDK_KEY_Delete:
    return RETRO_KEYBOARD_KEY_DELETE;

  case GDK_KEY_Up:
    return RETRO_KEYBOARD_KEY_UP;
  case GDK_KEY_Down:
    return RETRO_KEYBOARD_KEY_DOWN;
  case GDK_KEY_Left:
    return RETRO_KEYBOARD_KEY_LEFT;
  case GDK_KEY_Right:
    return RETRO_KEYBOARD_KEY_RIGHT;
  case GDK_KEY_Insert:
    return RETRO_KEYBOARD_KEY_INSERT;
  case GDK_KEY_Home:
    return RETRO_KEYBOARD_KEY_HOME;
  case GDK_KEY_End:
    return RETRO_KEYBOARD_KEY_END;
  case GDK_KEY_Page_Up:
    return RETRO_KEYBOARD_KEY_PAGEUP;
  case GDK_KEY_Page_Down:
    return RETRO_KEYBOARD_KEY_PAGEDOWN;

  case GDK_KEY_KP_Decimal:
    return RETRO_KEYBOARD_KEY_KP_PERIOD;
  case GDK_KEY_KP_Divide:
    return RETRO_KEYBOARD_KEY_KP_DIVIDE;
  case GDK_KEY_KP_Multiply:
    return RETRO_KEYBOARD_KEY_KP_MULTIPLY;
  case GDK_KEY_KP_Subtract:
    return RETRO_KEYBOARD_KEY_KP_MINUS;
  case GDK_KEY_KP_Add:
    return RETRO_KEYBOARD_KEY_KP_PLUS;
  case GDK_KEY_KP_Enter:
    return RETRO_KEYBOARD_KEY_KP_ENTER;
  case GDK_KEY_KP_Equal:
    return RETRO_KEYBOARD_KEY_KP_EQUALS;

  case GDK_KEY_Num_Lock:
    return RETRO_KEYBOARD_KEY_NUMLOCK;
  case GDK_KEY_Caps_Lock:
    return RETRO_KEYBOARD_KEY_CAPSLOCK;
  case GDK_KEY_Scroll_Lock:
    return RETRO_KEYBOARD_KEY_SCROLLOCK;
  case GDK_KEY_Shift_R:
    return RETRO_KEYBOARD_KEY_RSHIFT;
  case GDK_KEY_Shift_L:
    return RETRO_KEYBOARD_KEY_LSHIFT;
  case GDK_KEY_Control_R:
    return RETRO_KEYBOARD_KEY_RCTRL;
  case GDK_KEY_Control_L:
    return RETRO_KEYBOARD_KEY_LCTRL;
  case GDK_KEY_Alt_R:
    return RETRO_KEYBOARD_KEY_RALT;
  case GDK_KEY_Alt_L:
    return RETRO_KEYBOARD_KEY_LALT;
  case GDK_KEY_Meta_R:
    return RETRO_KEYBOARD_KEY_RMETA;
  case GDK_KEY_Meta_L:
    return RETRO_KEYBOARD_KEY_LMETA;
  case GDK_KEY_Super_R:
    return RETRO_KEYBOARD_KEY_RSUPER;
  case GDK_KEY_Super_L:
    return RETRO_KEYBOARD_KEY_LSUPER;

  case GDK_KEY_Help:
    return RETRO_KEYBOARD_KEY_HELP;
  case GDK_KEY_Print:
    return RETRO_KEYBOARD_KEY_PRINT;
  case GDK_KEY_Sys_Req:
    return RETRO_KEYBOARD_KEY_SYSREQ;
  case GDK_KEY_Break:
    return RETRO_KEYBOARD_KEY_BREAK;
  case GDK_KEY_Menu:
    return RETRO_KEYBOARD_KEY_MENU;
  case GDK_KEY_EuroSign:
    return RETRO_KEYBOARD_KEY_EURO;
  case GDK_KEY_Undo:
    return RETRO_KEYBOARD_KEY_UNDO;

  default:
    return RETRO_KEYBOARD_KEY_UNKNOWN;
  }
}
