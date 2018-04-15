// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-key-joypad-mapping.h"

#include <gdk/gdkkeysyms.h>

struct _RetroKeyJoypadMapping
{
  GObject parent_instance;
  guint joypad_keys[RETRO_JOYPAD_ID_COUNT];
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
  memset (self->joypad_keys, GDK_KEY_VoidSymbol, RETRO_JOYPAD_ID_COUNT * sizeof (guint));
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
                                         guint                  key)
{
  g_return_if_fail (RETRO_IS_KEY_JOYPAD_MAPPING (self));
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
guint
retro_key_joypad_mapping_get_button_key (RetroKeyJoypadMapping *self,
                                         RetroJoypadId          button)
{
  g_return_val_if_fail (RETRO_IS_KEY_JOYPAD_MAPPING (self), GDK_KEY_VoidSymbol);
  g_return_val_if_fail (button >= 0, GDK_KEY_VoidSymbol);
  g_return_val_if_fail (button < RETRO_JOYPAD_ID_COUNT, GDK_KEY_VoidSymbol);

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
