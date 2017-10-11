// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-controller-iterator.h"

struct _RetroControllerIterator
{
  GObject parent_instance;
  GHashTableIter iterator;
};

G_DEFINE_TYPE (RetroControllerIterator, retro_controller_iterator, G_TYPE_OBJECT)

/* Private */

static void
retro_controller_iterator_finalize (GObject *object)
{
  G_OBJECT_CLASS (retro_controller_iterator_parent_class)->finalize (object);
}

static void
retro_controller_iterator_class_init (RetroControllerIteratorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_controller_iterator_finalize;
}

static void
retro_controller_iterator_init (RetroControllerIterator *self)
{
}

/* Public */

/**
 * retro_controller_iterator_next:
 * @self: a #RetroControllerIterator
 * @port: (out) (optional): return location for the port
 * @controller: (out) (optional) (nullable): return location for the controller
 *
 * Fetch the next #RetroController and the port number it is plugged into.
 *
 * Returns: %FALSE if it reached the end, %TRUE otherwise
 */
gboolean
retro_controller_iterator_next (RetroControllerIterator  *self,
                                guint                   **port,
                                RetroController         **controller)
{
  g_return_val_if_fail (RETRO_IS_CONTROLLER_ITERATOR (self), FALSE);
  g_return_val_if_fail (port != NULL, FALSE);
  g_return_val_if_fail (controller != NULL, FALSE);

  return g_hash_table_iter_next (&self->iterator, (gpointer *) port, (gpointer *) controller);
}

/**
 * retro_controller_iterator_new:
 * @controllers: (element-type guint RetroController): A #GHashTable
 *
 * Creates a new #RetroControllerIterator.
 *
 * Returns: (transfer full): a new #RetroControllerIterator
 */
RetroControllerIterator *
retro_controller_iterator_new (GHashTable *controllers)
{
  RetroControllerIterator *self;

  g_return_val_if_fail (controllers != NULL, NULL);

  self = g_object_new (RETRO_TYPE_CONTROLLER_ITERATOR, NULL);
  g_hash_table_iter_init (&self->iterator, controllers);

  return self;
}
