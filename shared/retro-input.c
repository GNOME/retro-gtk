// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-input-private.h"

G_DEFINE_BOXED_TYPE (RetroInput, retro_input, retro_input_copy, retro_input_free)

/* Private */

/**
 * retro_input_init:
 * @self: a #RetroInput
 * @controller_type: the controller type
 * @id: the id
 * @index: the index
 *
 * Initializes @self with the given parameters.
 */
void
retro_input_init (RetroInput          *self,
                  RetroControllerType  controller_type,
                  guint                id,
                  guint                index)
{
  g_return_if_fail (self != NULL);

  self->any.type = controller_type;
  self->any.id = id;
  self->any.index = index;
}

/* Public */

/**
 * retro_input_new:
 *
 * Creates a new #RetroInput.
 *
 * Returns: (transfer full): a new #RetroInput, use retro_input_free() to free
 * it
 */
RetroInput *
retro_input_new (void)
{
  RetroInput *self;

  self = g_slice_new0 (RetroInput);

  return self;
}

/**
 * retro_input_copy:
 * @self: a #RetroInput
 *
 * Copies @self into a new #RetroInput.
 *
 * Returns: (transfer full): a new #RetroInput, use retro_input_free() to free
 * it
 */
RetroInput *
retro_input_copy (RetroInput *self)
{
  RetroInput *copy;

  g_return_val_if_fail (self != NULL, NULL);

  copy = retro_input_new ();
  copy->any.type = self->any.type;
  copy->any.id = self->any.id;
  copy->any.index = self->any.index;

  return copy;
}

/**
 * retro_input_free:
 * @self: a #RetroInput
 *
 * Frees the given #RetroInput.
 */
void
retro_input_free (RetroInput *self)
{
  g_return_if_fail (self != NULL);

  g_slice_free (RetroInput, self);
}

/**
 * retro_input_get_controller_type:
 * @self: a #RetroInput
 *
 * Gets the controller type of @self.
 *
 * Returns: the controller type of @self
 */
RetroControllerType
retro_input_get_controller_type (RetroInput *self)
{
  g_return_val_if_fail (self != NULL, RETRO_CONTROLLER_TYPE_NONE);

  return self->any.type;
}

/**
 * retro_input_get_joypad:
 * @self: a #RetroInput
 * @id: (out): return location for the id
 *
 * Gets the joypad id of @self, if any.
 *
 * Returns: whether the id was retrieved
 */
gboolean
retro_input_get_joypad (RetroInput    *self,
                        RetroJoypadId *id)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (id != NULL, FALSE);

  if (self->any.type != RETRO_CONTROLLER_TYPE_JOYPAD)
    return FALSE;

  if (self->joypad.id >= RETRO_JOYPAD_ID_COUNT)
    return FALSE;

  *id = self->joypad.id;

  return TRUE;
}

/**
 * retro_input_get_mouse:
 * @self: a #RetroInput
 * @id: (out): return location for the id
 *
 * Gets the mouse id of @self, if any.
 *
 * Returns: whether the id was retrieved
 */
gboolean
retro_input_get_mouse (RetroInput   *self,
                       RetroMouseId *id)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (id != NULL, FALSE);

  if (self->any.type != RETRO_CONTROLLER_TYPE_MOUSE)
    return FALSE;

  if (self->mouse.id >= RETRO_MOUSE_ID_COUNT)
    return FALSE;

  *id = self->mouse.id;

  return TRUE;
}

/**
 * retro_input_get_keyboard:
 * @self: a #RetroInput
 * @key: (out): return location for the key
 *
 * Gets the keyboard key of @self, if any.
 *
 * Returns: whether the key was retrieved
 */
gboolean
retro_input_get_keyboard (RetroInput       *self,
                          RetroKeyboardKey *key)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (key != NULL, FALSE);

  if (self->any.type != RETRO_CONTROLLER_TYPE_KEYBOARD)
    return FALSE;

  if (self->keyboard.key >= RETRO_KEYBOARD_KEY_LAST)
    return FALSE;

  *key = self->keyboard.key;

  return TRUE;
}

/**
 * retro_input_get_lightgun:
 * @self: a #RetroInput
 * @id: (out): return location for the id
 *
 * Gets the lightgun id of @self, if any.
 *
 * Returns: whether the id was retrieved
 */
gboolean
retro_input_get_lightgun (RetroInput      *self,
                          RetroLightgunId *id)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (id != NULL, FALSE);

  if (self->any.type != RETRO_CONTROLLER_TYPE_LIGHTGUN)
    return FALSE;

  if (self->lightgun.id >= RETRO_LIGHTGUN_ID_COUNT)
    return FALSE;

  *id = self->lightgun.id;

  return TRUE;
}

/**
 * retro_input_get_analog:
 * @self: a #RetroInput
 * @id: (out): return location for the id
 * @index: (out): return location for the index
 *
 * Gets the analog id and index of @self, if any.
 *
 * Returns: whether the id and the index were retrieved
 */
gboolean
retro_input_get_analog (RetroInput       *self,
                        RetroAnalogId    *id,
                        RetroAnalogIndex *index)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (id != NULL, FALSE);
  g_return_val_if_fail (index != NULL, FALSE);

  if (self->any.type != RETRO_CONTROLLER_TYPE_ANALOG)
    return FALSE;

  if (self->analog.id >= RETRO_ANALOG_ID_COUNT)
    return FALSE;

  if (self->analog.index >= RETRO_ANALOG_INDEX_COUNT)
    return FALSE;

  *id = self->analog.id;
  *index = self->analog.index;

  return TRUE;
}

/**
 * retro_input_get_pointer:
 * @self: a #RetroInput
 * @id: (out): return location for the id
 *
 * Gets the pointer id of @self, if any.
 *
 * Returns: whether the id was retrieved
 */
gboolean
retro_input_get_pointer (RetroInput     *self,
                         RetroPointerId *id)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (id != NULL, FALSE);

  if (self->any.type != RETRO_CONTROLLER_TYPE_POINTER)
    return FALSE;

  if (self->pointer.id >= RETRO_POINTER_ID_COUNT)
    return FALSE;

  *id = self->pointer.id;

  return TRUE;
}
