// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-key-joypad-mapping.h"

#include <linux/input-event-codes.h>

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
  memset (self->joypad_keys, 0, RETRO_JOYPAD_ID_COUNT * sizeof (guint16));
}

/* Public */

/**
 * retro_key_joypad_mapping_set_button_key:
 * @self: a #RetroKeyJoypadMapping
 * @button: joypad button id
 * @key: mapping key
 *
 * Maps the joypad button id to mapping key.
 */
void
retro_key_joypad_mapping_set_button_key (RetroKeyJoypadMapping *self,
                                         RetroJoypadId          button,
                                         guint16                key)
{
  g_return_val_if_fail (RETRO_IS_KEY_JOYPAD_MAPPING (self), KEY_MAX);
  g_return_if_fail (button >= 0);
  g_return_if_fail (button < RETRO_JOYPAD_ID_COUNT);

  self->joypad_keys[button] = key;
}

/**
 * retro_key_joypad_mapping_get_button_key:
 * @self: a #RetroKeyJoypadMapping
 * @button: joypad button id
 *
 * Gets the mapping key for the joypad button id.
 *
 * Returns: mapping key
 */
guint16
retro_key_joypad_mapping_get_button_key (RetroKeyJoypadMapping *self,
                                         RetroJoypadId          button)
{
  g_return_val_if_fail (RETRO_IS_KEY_JOYPAD_MAPPING (self), KEY_MAX);
  g_return_val_if_fail (button >= 0, KEY_MAX);
  g_return_val_if_fail (button < RETRO_JOYPAD_ID_COUNT, KEY_MAX);

  return self->joypad_keys[button];
}

/**
 * retro_key_joypad_mapping_set_to_defaults:
 * @self: a #RetroKeyJoypadMapping
 *
 * Sets the mapping to default values.
 */
void
retro_key_joypad_mapping_set_to_defaults (RetroKeyJoypadMapping *self)
{
  self->joypad_keys[RETRO_JOYPAD_ID_B]      = KEY_S;
  self->joypad_keys[RETRO_JOYPAD_ID_Y]      = KEY_A;
  self->joypad_keys[RETRO_JOYPAD_ID_SELECT] = KEY_BACKSPACE;
  self->joypad_keys[RETRO_JOYPAD_ID_START]  = KEY_ENTER;
  self->joypad_keys[RETRO_JOYPAD_ID_UP]     = KEY_UP;
  self->joypad_keys[RETRO_JOYPAD_ID_DOWN]   = KEY_DOWN;
  self->joypad_keys[RETRO_JOYPAD_ID_LEFT]   = KEY_LEFT;
  self->joypad_keys[RETRO_JOYPAD_ID_RIGHT]  = KEY_RIGHT;
  self->joypad_keys[RETRO_JOYPAD_ID_A]      = KEY_D;
  self->joypad_keys[RETRO_JOYPAD_ID_X]      = KEY_W;
  self->joypad_keys[RETRO_JOYPAD_ID_L]      = KEY_Q;
  self->joypad_keys[RETRO_JOYPAD_ID_R]      = KEY_E;
  self->joypad_keys[RETRO_JOYPAD_ID_L2]     = KEY_Z;
  self->joypad_keys[RETRO_JOYPAD_ID_R2]     = KEY_C;
  self->joypad_keys[RETRO_JOYPAD_ID_L3]     = KEY_1;
  self->joypad_keys[RETRO_JOYPAD_ID_R3]     = KEY_3;
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
