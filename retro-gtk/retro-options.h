// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_OPTIONS_H
#define RETRO_OPTIONS_H

#include <glib-object.h>
#include "retro-variable.h"

G_BEGIN_DECLS

#define RETRO_TYPE_OPTIONS (retro_options_get_type())

G_DECLARE_FINAL_TYPE (RetroOptions, retro_options, RETRO, OPTIONS, GObject)

RetroOptions *retro_options_new (void);

gboolean retro_options_contains (RetroOptions *self,
                                 const gchar  *key);
const gchar *retro_options_get_option_value (RetroOptions *self,
                                             const gchar  *key);
void retro_options_set_option_value (RetroOptions *self,
                                     const gchar  *key,
                                     const gchar  *value);
const gchar *retro_options_get_option_description (RetroOptions *self,
                                                   const gchar  *key);
const gchar **retro_options_get_option_values (RetroOptions *self,
                                               const gchar  *key);
GList *retro_options_get_keys (RetroOptions *self);
void retro_options_insert_variable (RetroOptions        *self,
                                    const RetroVariable *variable);
gboolean retro_options_get_variable_update (RetroOptions *self);

G_END_DECLS

#endif /* RETRO_OPTIONS_H */
