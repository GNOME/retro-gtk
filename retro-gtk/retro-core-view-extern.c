// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-gtk-internal.h"
#include "retro-core-view-input-device.h"

/* Private */

// FIXME Make static as soon as possible.
gint16
retro_core_view_get_input_state (RetroCoreView   *self,
                                 RetroDeviceType  device,
                                 guint            index,
                                 guint            id)
{
  g_return_val_if_fail (self != NULL, 0);

  switch (device) {
  default:
    return 0;
  }
}

// FIXME Make static as soon as possible.
guint64
retro_core_view_get_device_capabilities (RetroCoreView *self)
{
  g_return_val_if_fail (self != NULL, 0);

  return 0;
}

/* Public */

RetroInputDevice *
retro_core_view_as_input_device (RetroCoreView   *self,
                                 RetroDeviceType  device_type)
{
  g_return_val_if_fail (self != NULL, NULL);

  return RETRO_INPUT_DEVICE (retro_core_view_input_device_new (self, device_type));
}
