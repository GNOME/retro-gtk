// This file is part of retro-gtk. License: GPL-3.0+.

/**
 * SECTION:retro-controller-iterator
 * @short_description: An object iterating through the controllers plugged into a RetroCore
 * @title: RetroControllerIterator
 * @See_also: #RetroController, #RetroCore
 */

#include "retro-controller-iterator-private.h"

struct _RetroControllerIterator
{
  GObject parent_instance;
  GHashTableIter iterator;
  RetroControllerIteratorGetController get_controller;
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
 * @controller: (out) (optional) (transfer none): return location for the controller
 *
 * Fetch the next #RetroController and the port number it is plugged into.
 *
 * Returns: %FALSE if it reached the end, %TRUE otherwise
 */
gboolean
retro_controller_iterator_next (RetroControllerIterator  *self,
                                guint                    *port,
                                RetroController         **controller)
{
  gpointer key, val;
  gboolean ret;

  g_return_val_if_fail (RETRO_IS_CONTROLLER_ITERATOR (self), FALSE);

  ret = g_hash_table_iter_next (&self->iterator, &key, &val);

  if (port)
    *port = GPOINTER_TO_UINT (key);

  if (controller)
    *controller = self->get_controller (val);

  return ret;
}

/**
 * retro_controller_iterator_new:
 * @controllers: (element-type guint RetroController): A #GHashTable
 * @func: a function to extract controllers from the hash table values
 *
 * Creates a new #RetroControllerIterator.
 *
 * Returns: (transfer full): a new #RetroControllerIterator
 */
RetroControllerIterator *
retro_controller_iterator_new (GHashTable                           *controllers,
                               RetroControllerIteratorGetController  func)
{
  RetroControllerIterator *self;

  g_return_val_if_fail (controllers != NULL, NULL);

  self = g_object_new (RETRO_TYPE_CONTROLLER_ITERATOR, NULL);
  g_hash_table_iter_init (&self->iterator, controllers);

  self->get_controller = func;

  return self;
}
