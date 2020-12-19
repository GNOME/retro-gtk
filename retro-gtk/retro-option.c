// This file is part of retro-gtk. License: GPL-3.0+.

/**
 * SECTION:retro-option
 * @short_description: An object representing an option from a Libretro core
 * @title: RetroOption
 * @See_also: #RetroCore
 */

#include "retro-option-private.h"

struct _RetroOption
{
  GObject parent_instance;
  gchar *key;
  gchar *description;
  gchar **values;
  gsize value;
};

G_DEFINE_TYPE (RetroOption, retro_option, G_TYPE_OBJECT)

enum {
  SIGNAL_VALUE_CHANGED,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

#define RETRO_OPTION_ERROR (retro_option_error_quark ())

enum {
  RETRO_OPTION_ERROR_NO_DESCRIPTION_SEPARATOR,
  RETRO_OPTION_ERROR_NO_VALUES,
  RETRO_OPTION_ERROR_INVALID_VALUE,
};

G_DEFINE_QUARK (retro-option-error, retro_option_error)

/* Private */

static void
retro_option_finalize (GObject *object)
{
  RetroOption *self = (RetroOption *)object;

  g_free (self->key);
  g_free (self->description);
  g_strfreev (self->values);

  G_OBJECT_CLASS (retro_option_parent_class)->finalize (object);
}

static void
retro_option_class_init (RetroOptionClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_option_finalize;

  signals[SIGNAL_VALUE_CHANGED] =
    g_signal_new ("value-changed", RETRO_TYPE_OPTION, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE,
                  0);
}

static void
retro_option_init (RetroOption *self)
{
}

/* Public */

/**
 * retro_option_get_key:
 * @self: a #RetroOption
 *
 * Gets the key of @self.
 *
 * Returns: (transfer none): the key of @self
 */
const gchar *
retro_option_get_key (RetroOption *self)
{
  g_return_val_if_fail (RETRO_IS_OPTION (self), NULL);

  return self->key;
}

/**
 * retro_option_get_description:
 * @self: a #RetroOption
 *
 * Gets the description of @self.
 *
 * Returns: (transfer none): the description of @self
 */
const gchar *
retro_option_get_description (RetroOption *self)
{
  g_return_val_if_fail (RETRO_IS_OPTION (self), NULL);

  return self->description;
}

/**
 * retro_option_get_values:
 * @self: a #RetroOption
 *
 * Gets the value of @self.
 *
 * Returns: (array zero-terminated=1) (element-type utf8) (transfer none): the
 * value of @self
 */
const gchar **
retro_option_get_values (RetroOption *self)
{
  g_return_val_if_fail (RETRO_IS_OPTION (self), NULL);

  return (const gchar **) self->values;
}

/**
 * retro_option_get_value:
 * @self: a #RetroOption
 *
 * Gets the value of @self.
 *
 * Returns: (transfer none): the value of @self
 */
const gchar *
retro_option_get_value (RetroOption *self)
{
  g_return_val_if_fail (RETRO_IS_OPTION (self), NULL);

  return self->values[self->value];
}

/**
 * retro_option_set_value:
 * @self: a #RetroOption
 * @value: the value
 * @error: return location for a #GError, or %NULL
 *
 * Sets the value for @self. An error will be thrown if the value is invalid for
 * @self.
 */
void
retro_option_set_value (RetroOption  *self,
                        const gchar  *value,
                        GError      **error)
{
  gsize i;

  g_return_if_fail (RETRO_IS_OPTION (self));
  g_return_if_fail (value != NULL);

  if (g_strcmp0 (self->values[self->value], value) == 0)
    return;

  for (i = 0; self->values[i] != NULL; i++)
    if (g_strcmp0 (self->values[i], value) == 0)
      break;

  if (G_UNLIKELY (self->values[i] == NULL)) {
    g_set_error (error,
                 RETRO_OPTION_ERROR,
                 RETRO_OPTION_ERROR_INVALID_VALUE,
                 "Unexpected option value: option %s doesn’t have value %s.",
                 self->key,
                 value);

    return;
  }

  self->value = i;

  g_signal_emit (self, signals[SIGNAL_VALUE_CHANGED], 0);
}

RetroOption *
retro_option_new (const gchar  *key,
                  const gchar  *definition,
                  GError      **error)
{
  g_autoptr (RetroOption) self;
  gchar *description_separator;
  g_auto(GStrv) values = NULL;

  g_return_val_if_fail (key != NULL, NULL);
  g_return_val_if_fail (definition != NULL, NULL);

  description_separator = g_strstr_len (definition, -1, "; ");
  if (G_UNLIKELY (description_separator == NULL)) {
    g_set_error_literal (error,
                         RETRO_OPTION_ERROR,
                         RETRO_OPTION_ERROR_NO_DESCRIPTION_SEPARATOR,
                         "Unexpected variable format: no description separator found.");

    return NULL;
  }

  values = g_strsplit (description_separator + 2, "|", 0);
  if (G_UNLIKELY (*values == NULL)) {
    g_set_error_literal (error,
                         RETRO_OPTION_ERROR,
                         RETRO_OPTION_ERROR_NO_VALUES,
                         "Unexpected variable format: no values.");

    return NULL;
  }

  self = g_object_new (RETRO_TYPE_OPTION, NULL);

  self->key = g_strdup (key);
  self->description = g_strndup (definition,
                                 description_separator - definition);
  self->values = g_steal_pointer (&values);

  return g_steal_pointer (&self);
}
