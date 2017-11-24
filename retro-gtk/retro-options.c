// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-options.h"

#include "retro-option.h"

struct _RetroOptions
{
  GObject parent_instance;
  GHashTable *options;
  gboolean updated;
};

G_DEFINE_TYPE (RetroOptions, retro_options, G_TYPE_OBJECT)

enum {
  SIG_VALUE_CHANGED,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

RetroOptions *
retro_options_new (void)
{
  return g_object_new (RETRO_TYPE_OPTIONS, NULL);
}

static void
retro_options_finalize (GObject *object)
{
  RetroOptions *self = (RetroOptions *)object;

  g_hash_table_unref (self->options);

  G_OBJECT_CLASS (retro_options_parent_class)->finalize (object);
}

static void
retro_options_class_init (RetroOptionsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_options_finalize;

  signals[SIG_VALUE_CHANGED] =
    g_signal_new ("value-changed", RETRO_TYPE_OPTIONS, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__STRING,
                  G_TYPE_NONE,
                  1,
                  G_TYPE_STRING);
}

static void
retro_options_init (RetroOptions *self)
{
  self->options = g_hash_table_new_full (g_str_hash, g_str_equal,
                                         g_free, g_object_unref);
}

/* Public */

gboolean
retro_options_contains (RetroOptions *self,
                        const gchar  *key)
{
  g_return_val_if_fail (RETRO_IS_OPTIONS (self), FALSE);
  g_return_val_if_fail (key != NULL, FALSE);

  return g_hash_table_contains (self->options, key);
}

const gchar *
retro_options_get_option_value (RetroOptions *self,
                                const gchar  *key)
{
  RetroOption *option;

  g_return_val_if_fail (RETRO_IS_OPTIONS (self), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  option = RETRO_OPTION (g_hash_table_lookup (self->options, key));

  return retro_option_get_value (option);
}

void
retro_options_set_option_value (RetroOptions *self,
                                const gchar  *key,
                                const gchar  *value)
{
  RetroOption *option;
  GError *tmp_error = NULL;

  g_return_if_fail (RETRO_IS_OPTIONS (self));
  g_return_if_fail (key != NULL);
  g_return_if_fail (value != NULL);

  option = RETRO_OPTION (g_hash_table_lookup (self->options, key));

  retro_option_set_value (option, value, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_clear_error (&tmp_error);

    return;
  }

  g_signal_emit (self, signals[SIG_VALUE_CHANGED], 0, key);
  self->updated = TRUE;
}

const gchar *
retro_options_get_option_description (RetroOptions *self,
                                      const gchar  *key)
{
  RetroOption *option;

  g_return_val_if_fail (RETRO_IS_OPTIONS (self), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  option = RETRO_OPTION (g_hash_table_lookup (self->options, key));

  return retro_option_get_description (option);
}

const gchar **
retro_options_get_option_values (RetroOptions *self,
                                 const gchar  *key)
{
  RetroOption *option;

  g_return_val_if_fail (RETRO_IS_OPTIONS (self), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  option = RETRO_OPTION (g_hash_table_lookup (self->options, key));

  return retro_option_get_values (option);
}

GList *
retro_options_get_keys (RetroOptions *self)
{
  g_return_val_if_fail (RETRO_IS_OPTIONS (self), NULL);

  return g_hash_table_get_keys (self->options);
}

void
retro_options_insert_variable (RetroOptions        *self,
                               const RetroVariable *variable)
{
  RetroOption *option;
  const gchar *key;
  GError *tmp_error = NULL;

  g_return_if_fail (RETRO_IS_OPTIONS (self));
  g_return_if_fail (variable != NULL);

  option = retro_option_new (variable, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_debug ("%s", tmp_error->message);
    g_clear_error (&tmp_error);

    return;
  }

  key = retro_option_get_key (option);

  g_hash_table_insert (self->options, g_strdup (key), option);
  g_signal_emit (self, signals[SIG_VALUE_CHANGED], 0, key);

  self->updated = TRUE;
}

gboolean
retro_options_get_variable_update (RetroOptions *self)
{
  g_return_val_if_fail (RETRO_IS_OPTIONS (self), FALSE);

  if (!self->updated)
    return FALSE;

  self->updated = FALSE;

  return TRUE;
}
