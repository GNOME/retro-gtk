// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-input-device.h"

G_DEFINE_INTERFACE (RetroInputDevice, retro_input_device, G_TYPE_OBJECT);

static void
retro_input_device_default_init (RetroInputDeviceInterface *iface)
{
}

/**
 * retro_input_device_poll:
 * @self: a #RetroInputDevice
 *
 * Polls the pending input events for @self.
 */
void
retro_input_device_poll (RetroInputDevice *self)
{
  RetroInputDeviceInterface *iface;

  g_return_if_fail (RETRO_IS_INPUT_DEVICE (self));

  iface = RETRO_INPUT_DEVICE_GET_IFACE (self);

  g_return_if_fail (iface->poll != NULL);

  iface->poll (self);
}

/**
 * retro_input_device_get_input_state:
 * @self: a #RetroInputDevice
 * @device: a #RetroDeviceType to query @self
 * @index: an input index to interpret depending on @device
 * @id: an input id to interpret depending on @device
 *
 * Gets the state of an input of @self.
 *
 * Returns: the input's state
 */
gint16
retro_input_device_get_input_state (RetroInputDevice *self,
                                    RetroDeviceType   device,
                                    guint             index,
                                    guint             id)
{
  RetroInputDeviceInterface *iface;

  g_return_val_if_fail (RETRO_IS_INPUT_DEVICE (self), 0);

  iface = RETRO_INPUT_DEVICE_GET_IFACE (self);

  g_return_val_if_fail (iface->get_input_state != NULL, 0);

  return iface->get_input_state (self, device, index, id);
}

/**
 * retro_input_device_get_device_type:
 * @self: a #RetroInputDevice
 *
 * Gets the main type of the controller.
 *
 * Returns: the type of @self
 */
RetroDeviceType
retro_input_device_get_device_type (RetroInputDevice *self)
{

  RetroInputDeviceInterface *iface;

  g_return_val_if_fail (RETRO_IS_INPUT_DEVICE (self), RETRO_DEVICE_TYPE_NONE);

  iface = RETRO_INPUT_DEVICE_GET_IFACE (self);

  g_return_val_if_fail (iface->get_device_type != NULL, RETRO_DEVICE_TYPE_NONE);

  return iface->get_device_type (self);
}

/**
 * retro_input_device_get_device_capabilities:
 * @self: a #RetroInputDevice
 *
 * Gets a flag representing the capabilities of @self. Each bit index matches
 * the #RetroDeviceType of same number.
 *
 * For example, if @self is an analog gamepad, the value would be: (1 <<
 * RETRO_DEVICE_TYPE_JOYPAD) | (1 << RETRO_DEVICE_TYPE_ANALOG).
 *
 * Returns: the capabilities flag of @self
 */
guint64
retro_input_device_get_device_capabilities (RetroInputDevice *self)
{

  RetroInputDeviceInterface *iface;

  g_return_val_if_fail (RETRO_IS_INPUT_DEVICE (self), 0);

  iface = RETRO_INPUT_DEVICE_GET_IFACE (self);

  g_return_val_if_fail (iface->get_device_capabilities != NULL, 0);

  return iface->get_device_capabilities (self);
}
