// This file is part of retro-gtk. License: GPL-3.0+.

/**
 * SECTION:retro-controller-type
 * @short_description: A controller type enumeration
 * @title: RetroControllerType
 * @See_also: #RetroController
 */

#include "retro-controller-type.h"

#include "retro-controller-codes-private.h"
#include "retro-keyboard-key-private.h"

int
retro_controller_type_get_id_count (RetroControllerType type)
{
  switch (type) {
  case RETRO_CONTROLLER_TYPE_JOYPAD:
    return RETRO_JOYPAD_ID_COUNT;

  case RETRO_CONTROLLER_TYPE_MOUSE:
    return RETRO_MOUSE_ID_COUNT;

  case RETRO_CONTROLLER_TYPE_KEYBOARD:
    return RETRO_KEYBOARD_KEY_LAST;

  case RETRO_CONTROLLER_TYPE_LIGHTGUN:
    return RETRO_LIGHTGUN_ID_COUNT;

  case RETRO_CONTROLLER_TYPE_ANALOG:
    return RETRO_ANALOG_ID_COUNT;

  case RETRO_CONTROLLER_TYPE_POINTER:
    return RETRO_POINTER_ID_COUNT;

  default:
    g_assert_not_reached ();
  }
}

int
retro_controller_type_get_index_count (RetroControllerType type)
{
  switch (type) {
  case RETRO_CONTROLLER_TYPE_ANALOG:
    return RETRO_ANALOG_INDEX_COUNT;

  case RETRO_CONTROLLER_TYPE_JOYPAD:
  case RETRO_CONTROLLER_TYPE_MOUSE:
  case RETRO_CONTROLLER_TYPE_KEYBOARD:
  case RETRO_CONTROLLER_TYPE_LIGHTGUN:
  case RETRO_CONTROLLER_TYPE_POINTER:
    return 1;

  default:
    g_assert_not_reached ();
  }
}
