// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-controller.h"

G_DEFINE_INTERFACE (RetroController, retro_controller, G_TYPE_OBJECT);

static void
retro_controller_default_init (RetroControllerInterface *iface)
{
}

/**
 * retro_controller_poll:
 * @self: a #RetroController
 *
 * Polls the pending input events for @self.
 */
void
retro_controller_poll (RetroController *self)
{
  RetroControllerInterface *iface;

  g_return_if_fail (RETRO_IS_CONTROLLER (self));

  iface = RETRO_CONTROLLER_GET_IFACE (self);

  g_return_if_fail (iface->poll != NULL);

  iface->poll (self);
}

/**
 * retro_controller_get_input_state:
 * @self: a #RetroController
 * @controller_type: a #RetroControllerType to query @self
 * @index: an input index to interpret depending on @controller_type
 * @id: an input id to interpret depending on @controller_type
 *
 * Gets the state of an input of @self.
 *
 * Returns: the input's state
 */
gint16
retro_controller_get_input_state (RetroController     *self,
                                  RetroControllerType  controller_type,
                                  guint                index,
                                  guint                id)
{
  RetroControllerInterface *iface;

  g_return_val_if_fail (RETRO_IS_CONTROLLER (self), 0);

  iface = RETRO_CONTROLLER_GET_IFACE (self);

  g_return_val_if_fail (iface->get_input_state != NULL, 0);

  return iface->get_input_state (self, controller_type, index, id);
}

/**
 * retro_controller_get_controller_type:
 * @self: a #RetroController
 *
 * Gets the main type of the controller.
 *
 * Returns: the controller type of @self
 */
RetroControllerType
retro_controller_get_controller_type (RetroController *self)
{

  RetroControllerInterface *iface;

  g_return_val_if_fail (RETRO_IS_CONTROLLER (self), RETRO_CONTROLLER_TYPE_NONE);

  iface = RETRO_CONTROLLER_GET_IFACE (self);

  g_return_val_if_fail (iface->get_controller_type != NULL, RETRO_CONTROLLER_TYPE_NONE);

  return iface->get_controller_type (self);
}

/**
 * retro_controller_get_capabilities:
 * @self: a #RetroController
 *
 * Gets a flag representing the capabilities of @self. Each bit index matches
 * the #RetroControllerType of same number.
 *
 * For example, if @self is an analog gamepad, the value would be: (1 <<
 * RETRO_CONTROLLER_TYPE_JOYPAD) | (1 << RETRO_CONTROLLER_TYPE_ANALOG).
 *
 * Returns: the capabilities flag of @self
 */
guint64
retro_controller_get_capabilities (RetroController *self)
{

  RetroControllerInterface *iface;

  g_return_val_if_fail (RETRO_IS_CONTROLLER (self), 0);

  iface = RETRO_CONTROLLER_GET_IFACE (self);

  g_return_val_if_fail (iface->get_capabilities != NULL, 0);

  return iface->get_capabilities (self);
}

/**
 * retro_controller_set_rumble_state:
 * @self: a #RetroController
 * @effect: the rumble effect
 * @strength: the rumble effect strength
 *
 * Sets the rumble state of @self.
 *
 * Returns: whether the rumble state has been successfully set.
 */
gboolean
retro_controller_set_rumble_state (RetroController   *self,
                                   RetroRumbleEffect  effect,
                                   guint16            strength)
{
  g_return_val_if_fail (RETRO_IS_CONTROLLER (self), FALSE);

  // TODO

  return FALSE;
}
