// This file is part of retro-gtk. License: GPL-3.0+.

#include "../retro-gtk-internal.h"
#include "retro-gamepad-input.h"

typedef struct {
  gboolean analog;
  gint16 buttons[RETRO_JOYPAD_ID_COUNT];
  gint16 axes[RETRO_ANALOG_INDEX_COUNT][RETRO_ANALOG_ID_COUNT];
} RetroGamepadInternal;

#define RETRO_GAMEPAD_INTERNAL(self) ((RetroGamepadInternal *) ((self)->internal))

/* Public */

void
retro_gamepad_button_press_event (RetroGamepad *self,
                                  guint16       button)
{
  RetroJoypadId retro_button;

  g_return_if_fail (self != NULL);

  retro_button = retro_gamepad_button_converter (button);

  if (retro_button == RETRO_JOYPAD_ID_COUNT)
    return;

  RETRO_GAMEPAD_INTERNAL (self)->buttons[retro_button] = G_MAXINT16;
}

void
retro_gamepad_button_release_event (RetroGamepad *self,
                                    guint16       button)
{
  RetroJoypadId retro_button;

  g_return_if_fail (self != NULL);

  retro_button = retro_gamepad_button_converter (button);

  if (retro_button == RETRO_JOYPAD_ID_COUNT)
    return;

  RETRO_GAMEPAD_INTERNAL (self)->buttons[retro_button] = 0;
}

void
retro_gamepad_axis_event (RetroGamepad *self,
                          guint16       axis,
                          gdouble       value)
{
  RetroAnalogIndex retro_index;
  RetroAnalogId retro_id;

  g_return_if_fail (self != NULL);

  if (!retro_gamepad_axis_converter (axis, &retro_index, &retro_id))
    return;

  RETRO_GAMEPAD_INTERNAL (self)->axes[retro_index][retro_id] = value * G_MAXINT16;
}

gint16
retro_gamepad_internal_get_input_state (RetroInputDevice *base,
                                        RetroDeviceType   device,
                                        guint             index,
                                        guint             id)
{
  RetroGamepad *self = RETRO_GAMEPAD (base);

    g_return_val_if_fail (self != NULL, 0);

  switch (device) {
  case RETRO_DEVICE_TYPE_JOYPAD:
    g_return_val_if_fail (id < RETRO_JOYPAD_ID_COUNT, 0);

    return RETRO_GAMEPAD_INTERNAL (self)->buttons[id];
  case RETRO_DEVICE_TYPE_ANALOG:
    g_return_val_if_fail (index < RETRO_ANALOG_INDEX_COUNT, 0);
    g_return_val_if_fail (id < RETRO_ANALOG_ID_COUNT, 0);

    return RETRO_GAMEPAD_INTERNAL (self)->axes[index][id];
  default:
    return 0;
  }

  return 0;
}

RetroDeviceType
retro_gamepad_internal_get_device_type (RetroInputDevice *base)
{
  RetroGamepad *self = RETRO_GAMEPAD (base);

  g_return_val_if_fail (self != NULL, RETRO_DEVICE_TYPE_NONE);

  if (RETRO_GAMEPAD_INTERNAL (self)->analog)
    return RETRO_DEVICE_TYPE_ANALOG;

  return RETRO_DEVICE_TYPE_JOYPAD;
}

guint64
retro_gamepad_internal_get_device_capabilities (RetroInputDevice *base) {
  return (1 << RETRO_DEVICE_TYPE_JOYPAD) | (1 << RETRO_DEVICE_TYPE_ANALOG);
}

void
retro_gamepad_internal_init (RetroGamepad *self, gboolean analog) {
  g_return_if_fail (self != NULL);

  self->internal = g_new0 (RetroGamepadInternal, 1);

  RETRO_GAMEPAD_INTERNAL (self)->analog = analog;
}

void
retro_gamepad_internal_finalize (RetroGamepad *self)
{
  g_return_if_fail (self != NULL);

  g_free (self->internal);
}
