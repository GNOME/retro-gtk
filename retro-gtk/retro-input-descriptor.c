// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-input-descriptor-private.h"

G_DEFINE_BOXED_TYPE (RetroInputDescriptor, retro_input_descriptor, retro_input_descriptor_copy, retro_input_descriptor_free)

/**
 * retro_input_descriptor_new:
 *
 * Creates a new #RetroInputDescriptor.
 *
 * Returns: (transfer full): a new #RetroInputDescriptor, use
 * retro_input_descriptor_free() to free it
 */
RetroInputDescriptor *
retro_input_descriptor_new (void)
{
  RetroInputDescriptor *self;

  self = g_slice_new0 (RetroInputDescriptor);

  return self;
}

/**
 * retro_input_descriptor_copy:
 * @self: a #RetroInputDescriptor
 *
 * Copies @self into a new #RetroInputDescriptor.
 *
 * Returns: (transfer full): a new #RetroInputDescriptor, use
 * retro_input_descriptor_free() to free it
 */
RetroInputDescriptor *
retro_input_descriptor_copy (RetroInputDescriptor *self)
{
  RetroInputDescriptor *copy;

  g_return_val_if_fail (self, NULL);

  copy = retro_input_descriptor_new ();
  copy->port = self->port;
  copy->controller_type = self->controller_type;
  copy->index = self->index;
  copy->id = self->id;
  copy->description = g_strdup (self->description);

  return copy;
}

/**
 * retro_input_descriptor_free:
 * @self: a #RetroInputDescriptor
 *
 * Frees the given #RetroInputDescriptor object.
 */
void
retro_input_descriptor_free (RetroInputDescriptor *self)
{
  g_return_if_fail (self);

  g_free (self->description);

  g_slice_free (RetroInputDescriptor, self);
}
