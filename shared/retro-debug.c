// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-debug-private.h"

gboolean
retro_is_debug (void)
{
  static gsize init = 0;
  gboolean debug = FALSE;

  if (g_once_init_enter (&init)) {
    g_auto(GStrv) envp = g_get_environ ();
    const gchar *env_value = g_environ_getenv (envp, "RETRO_DEBUG");

    debug = (g_strcmp0 ("1", env_value) == 0);

    g_once_init_leave (&init, 1);
  }

  return debug;
}
