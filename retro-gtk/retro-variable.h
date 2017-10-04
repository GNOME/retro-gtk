// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_VARIABLE_H
#define RETRO_VARIABLE_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef struct _RetroVariable RetroVariable;

struct _RetroVariable
{
  const gchar *key;
  const gchar *value;
};

G_END_DECLS

#endif /* RETRO_VARIABLE_H */
