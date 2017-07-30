// This file is part of retro-gtk. License: GPL-3.0+.

#include <linux/input-event-codes.h>
#include "retro-gtk-internal.h"
#include "retro-core-view-input-device.h"

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

/* Private */

static void
set_input_pressed (GHashTable *table,
                   guint       input)
{
  static gboolean value = TRUE;

  if (g_hash_table_contains (table, &input))
    g_hash_table_replace (table,
                          g_memdup (&input, sizeof (guint)),
                          g_memdup (&value, sizeof (gboolean)));
  else
    g_hash_table_insert (table,
                         g_memdup (&input, sizeof (guint)),
                         g_memdup (&value, sizeof (gboolean)));
}

static void
set_input_released (GHashTable *table,
                    guint       input)
{
  g_hash_table_remove (table, &input);
}

static gboolean
get_input_state (GHashTable *table,
                 guint       input)
{
  return g_hash_table_contains (table, &input);
}

// FIXME Make static as soon as possible.
gboolean
retro_core_view_on_key_press_event (RetroCoreView *self,
                                    GtkWidget     *source,
                                    GdkEventKey   *event)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  set_input_pressed (self->key_state, event->hardware_keycode);

  return FALSE;
}

// FIXME Make static as soon as possible.
gboolean
retro_core_view_on_key_release_event (RetroCoreView *self,
                                      GtkWidget     *source,
                                      GdkEventKey   *event)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  set_input_released (self->key_state, event->hardware_keycode);

  return FALSE;
}

static gboolean
retro_core_view_get_key_state (RetroCoreView *self,
                               guint16        hardware_keycode)
{
  g_return_val_if_fail (self != NULL, FALSE);

  return get_input_state (self->key_state, hardware_keycode);
}

static gboolean
retro_core_view_get_joypad_button_state (RetroCoreView *self,
                                         RetroJoypadId  button)
{
  guint16 hardware_keycode;

  g_return_val_if_fail (self != NULL, FALSE);

  if (button >= RETRO_JOYPAD_ID_COUNT)
    return 0;

  // GDK adds 8 to the Linux input event codes to create the hardware keycode.
  hardware_keycode = DEFAULT_KEY_JOYPAD_BUTTON_MAPPING[button] + 8;

  return retro_core_view_get_key_state (self, hardware_keycode);
}

// FIXME Make static as soon as possible.
gint16
retro_core_view_get_input_state (RetroCoreView   *self,
                                 RetroDeviceType  device,
                                 guint            index,
                                 guint            id)
{
  g_return_val_if_fail (self != NULL, 0);

  switch (device) {
  case RETRO_DEVICE_TYPE_JOYPAD:
    if (id >= RETRO_JOYPAD_ID_COUNT)
      return 0;

    return retro_core_view_get_joypad_button_state (self, id) ? G_MAXINT16 : 0;
  default:
    return 0;
  }
}

// FIXME Make static as soon as possible.
guint64
retro_core_view_get_device_capabilities (RetroCoreView *self)
{
  g_return_val_if_fail (self != NULL, 0);

  return 1 << RETRO_DEVICE_TYPE_JOYPAD;
}

/* Public */

RetroInputDevice *
retro_core_view_as_input_device (RetroCoreView   *self,
                                 RetroDeviceType  device_type)
{
  g_return_val_if_fail (self != NULL, NULL);

  return RETRO_INPUT_DEVICE (retro_core_view_input_device_new (self, device_type));
}
