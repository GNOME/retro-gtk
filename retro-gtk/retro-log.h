// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_LOG_H
#define RETRO_LOG_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-core.h"

G_BEGIN_DECLS

void retro_g_log (RetroCore      *sender,
                  const gchar    *log_domain,
                  GLogLevelFlags  log_level,
                  const gchar    *message);

G_END_DECLS

#endif /* RETRO_LOG_H */
