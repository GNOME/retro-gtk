// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-option.h"

struct _RetroOption
{
  GObject parent_instance;
  gchar *key;
  gchar *description;
  gchar **values;
  gchar *current;
};

G_DEFINE_TYPE (RetroOption, retro_option, G_TYPE_OBJECT)

#define RETRO_OPTION_ERROR (retro_option_error_quark ())

enum {
  RETRO_OPTION_ERROR_NO_DESCRIPTION_SEPARATOR,
  RETRO_OPTION_ERROR_NO_VALUES,
  RETRO_OPTION_ERROR_INVALID_VALUE,
};

/* Private */

static GQuark
retro_option_error_quark (void)
{
  return g_quark_from_static_string ("retro-option-error-quark");
}

static void
retro_option_finalize (GObject *object)
{
  RetroOption *self = (RetroOption *)object;

  g_free (self->key);
  g_free (self->description);
  g_strfreev (self->values);
  g_free (self->current);

  G_OBJECT_CLASS (retro_option_parent_class)->finalize (object);
}

static void
retro_option_class_init (RetroOptionClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_option_finalize;
}

static void
retro_option_init (RetroOption *self)
{
}

/* Public */

const gchar *
retro_option_get_key (RetroOption *self)
{
  g_return_val_if_fail (RETRO_IS_OPTION (self), NULL);

  return self->key;
}

const gchar *
retro_option_get_description (RetroOption *self)
{
  g_return_val_if_fail (RETRO_IS_OPTION (self), NULL);

  return self->description;
}

const gchar **
retro_option_get_values (RetroOption *self)
{
  g_return_val_if_fail (RETRO_IS_OPTION (self), NULL);

  return self->values;
}

const gchar *
retro_option_get_current (RetroOption *self)
{
  g_return_val_if_fail (RETRO_IS_OPTION (self), NULL);

  return self->current;
}

void
retro_option_set_current (RetroOption  *self,
                          const gchar  *current,
                          GError      **error)
{
  g_return_if_fail (RETRO_IS_OPTION (self));
  g_return_if_fail (current != NULL);

  g_message ("%s, %s", self->current, current);
  if (g_strcmp0 (self->current, current) == 0)
    return;

  if (G_UNLIKELY (g_strv_contains ((const gchar *const *) self->values, current))) {
    g_set_error_literal (error,
                         RETRO_OPTION_ERROR,
                         RETRO_OPTION_ERROR_INVALID_VALUE,
                         "Unexpected option value: option %s doesn’t have value %s.");

    return;
  }

  g_free (self->current);
  self->current = g_strdup (current);
}

RetroOption *
retro_option_new (const RetroVariable  *variable,
                  GError              **error)
{
  RetroOption *self;
  gchar *description_separator;
  gchar **values;

  g_return_val_if_fail (variable != NULL, NULL);
  g_return_val_if_fail (variable->key != NULL, NULL);
  g_return_val_if_fail (variable->value != NULL, NULL);

  description_separator = g_strstr_len (variable->value, -1, "; ");
  if (G_UNLIKELY (description_separator == NULL)) {
    g_set_error_literal (error,
                         RETRO_OPTION_ERROR,
                         RETRO_OPTION_ERROR_NO_DESCRIPTION_SEPARATOR,
                         "Unexpected variable format: no description separator found.");

    return NULL;
  }

  values = g_strsplit (description_separator + 2, "|", 0);
  if (G_UNLIKELY (*values == NULL)) {
    g_strfreev (values);

    g_set_error_literal (error,
                         RETRO_OPTION_ERROR,
                         RETRO_OPTION_ERROR_NO_VALUES,
                         "Unexpected variable format: no values.");

    return NULL;
  }

  self = g_object_new (RETRO_TYPE_OPTION, NULL);

  self->key = g_strdup (variable->key);
  self->description = g_strndup (variable->value,
                                 description_separator - variable->value);
  self->values = values;
  self->current = g_strdup (values[0]);

  return self;
}
