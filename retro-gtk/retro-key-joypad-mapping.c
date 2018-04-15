// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-key-joypad-mapping.h"

#include <linux/input-event-codes.h>

static guint16 DEFAULT_KEY_JOYPAD_BUTTON_MAPPING[RETRO_JOYPAD_ID_COUNT] = {
  KEY_S,
  KEY_A,
  KEY_BACKSPACE,
  KEY_ENTER,
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_D,
  KEY_W,
  KEY_Q,
  KEY_E,
  KEY_Z,
  KEY_C,
  KEY_1,
  KEY_3,
};

struct _RetroKeyJoypadMapping
{
  GObject parent_instance;

  guint16 joypad_keys[RETRO_JOYPAD_ID_COUNT];
};

G_DEFINE_TYPE (RetroKeyJoypadMapping, retro_key_joypad_mapping, G_TYPE_OBJECT)

/* Private */

static void
retro_key_joypad_mapping_finalize (GObject *object)
{
  G_OBJECT_CLASS (retro_key_joypad_mapping_parent_class)->finalize (object);
}

static void
retro_key_joypad_mapping_class_init (RetroKeyJoypadMappingClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_key_joypad_mapping_finalize;
}

static void
retro_key_joypad_mapping_init (RetroKeyJoypadMapping *self)
{
}

/* Public */

/**
 * retro_key_joypad_mapping_set_button_key:
 * @self: a #RetroKeyJoypadMapping
 * @button: joypad button id
 * @hardware_keycode: mapping key code
 *
 * Maps the joypad button id to mapping key code.
 */
void
retro_key_joypad_mapping_set_button_key (RetroKeyJoypadMapping *self,
                                         RetroJoypadId          button,
                                         guint16                hardware_keycode)
{
  g_return_if_fail (RETRO_IS_KEY_JOYPAD_MAPPING (self));
  g_return_if_fail (button >= 0);
  g_return_if_fail (button < RETRO_JOYPAD_ID_COUNT);

  self->joypad_keys[button] = hardware_keycode;
}

/**
 * retro_key_joypad_mapping_get_button_key:
 * @self: a #RetroKeyJoypadMapping
 * @button: joypad button id
 *
 * Gets the mapping key code for the joypad button id.
 *
 * Returns: mapping key code
 */
guint16
retro_key_joypad_mapping_get_button_key (RetroKeyJoypadMapping *self,
                                         RetroJoypadId          button)
{
  g_return_val_if_fail (RETRO_IS_KEY_JOYPAD_MAPPING (self), 0);
  g_return_val_if_fail (button >= 0, 0);
  g_return_val_if_fail (button < RETRO_JOYPAD_ID_COUNT, 0);

  return self->joypad_keys[button];
}

/**
 * retro_key_joypad_mapping_new:
 *
 * Creates a new #RetroKeyJoypadMapping.
 *
 * Returns: (transfer full): a new #RetroKeyJoypadMapping
 */
RetroKeyJoypadMapping *
retro_key_joypad_mapping_new ()
{
  RetroKeyJoypadMapping *self;

  self = g_object_new (RETRO_TYPE_KEY_JOYPAD_MAPPING, NULL);

  return self;
}

/**
 * retro_key_joypad_mapping_new_default:
 *
 * Creates a new #RetroKeyJoypadMapping with the default mapping.
 *
 * Returns: (transfer full): a new #RetroKeyJoypadMapping
 */
RetroKeyJoypadMapping *
retro_key_joypad_mapping_new_default ()
{
  RetroKeyJoypadMapping *self;
  RetroJoypadId button;

  self = g_object_new (RETRO_TYPE_KEY_JOYPAD_MAPPING, NULL);

  for (button = 0; button < RETRO_JOYPAD_ID_COUNT; button ++)
    /* GDK adds 8 to the Linux input event codes to create the hardware keycode.
     * These codes are the only ones not coming from GDK so lets standardize on
     * what GDK does.
     */
    self->joypad_keys[button] = DEFAULT_KEY_JOYPAD_BUTTON_MAPPING[button] + 8;

  return self;
}
