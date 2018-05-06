// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-option-iterator.h"

struct _RetroOptionIterator
{
  GObject parent_instance;
  GHashTableIter iterator;
};

G_DEFINE_TYPE (RetroOptionIterator, retro_option_iterator, G_TYPE_OBJECT)

/* Private */

static void
retro_option_iterator_finalize (GObject *object)
{
  G_OBJECT_CLASS (retro_option_iterator_parent_class)->finalize (object);
}

static void
retro_option_iterator_class_init (RetroOptionIteratorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_option_iterator_finalize;
}

static void
retro_option_iterator_init (RetroOptionIterator *self)
{
}

/* Public */

/**
 * retro_option_iterator_next:
 * @self: a #RetroOptionIterator
 * @key: (out) (optional): return location for the key
 * @option: (out) (optional) (nullable): return location for the option
 *
 * Fetch the next #RetroOption and its key.
 *
 * Returns: %FALSE if it reached the end, %TRUE otherwise
 */
gboolean
retro_option_iterator_next (RetroOptionIterator  *self,
                            guint               **key,
                            RetroOption         **option)
{
  g_return_val_if_fail (RETRO_IS_OPTION_ITERATOR (self), FALSE);
  g_return_val_if_fail (key != NULL, FALSE);
  g_return_val_if_fail (option != NULL, FALSE);

  return g_hash_table_iter_next (&self->iterator, (gpointer *) key, (gpointer *) option);
}

/**
 * retro_option_iterator_new:
 * @options: (element-type guint Retrooption): A #GHashTable
 *
 * Creates a new #RetroOptionIterator.
 *
 * Returns: (transfer full): a new #RetroOptionIterator
 */
RetroOptionIterator *
retro_option_iterator_new (GHashTable *options)
{
  RetroOptionIterator *self;

  g_return_val_if_fail (options != NULL, NULL);

  self = g_object_new (RETRO_TYPE_OPTION_ITERATOR, NULL);
  g_hash_table_iter_init (&self->iterator, options);

  return self;
}
