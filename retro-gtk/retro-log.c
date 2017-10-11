// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-core.h"

/**
 * retro_g_log:
 * @sender: a #RetroCore
 * @log_domain: (nullable): the log domain, usually #G_LOG_DOMAIN, or %NULL
 * @log_level: the log level, either from #GLogLevelFlags or a user-defined
 * level
 * @message: the message to log
 *
 * Logs an error or debugging message.
 *
 * This is a convenience function to forward the log signal of #RetroCore to
 * g_log(), see it for more information.
 */
void
retro_g_log (RetroCore      *sender,
             const gchar    *log_domain,
             GLogLevelFlags  log_level,
             const gchar    *message)
{
  g_log (log_domain, log_level, "%s", message);
}
