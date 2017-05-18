// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_VARIABLE_H
#define RETRO_VARIABLE_H

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
