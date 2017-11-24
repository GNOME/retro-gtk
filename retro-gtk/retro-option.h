// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_OPTION_H
#define RETRO_OPTION_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-variable.h"

G_BEGIN_DECLS

#define RETRO_TYPE_OPTION (retro_option_get_type())

G_DECLARE_FINAL_TYPE (RetroOption, retro_option, RETRO, OPTION, GObject)

RetroOption *retro_option_new (const RetroVariable  *variable,
                               GError              **error);
const gchar *retro_option_get_key (RetroOption *self);
const gchar *retro_option_get_description (RetroOption *self);
const gchar **retro_option_get_values (RetroOption *self);
const gchar *retro_option_get_value (RetroOption *self);
void retro_option_set_value (RetroOption  *self,
                             const gchar  *value,
                             GError      **error);

G_END_DECLS

#endif /* RETRO_OPTION_H */
