// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-gtk-internal.h"

void
retro_g_log (RetroCore      *self,
             const gchar    *log_domain,
             GLogLevelFlags  log_level,
             const gchar    *message)
{
  g_log (log_domain, log_level, "%s", message);
}
