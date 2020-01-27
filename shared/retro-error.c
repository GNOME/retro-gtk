// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-error-private.h"

void
retro_error_ensure_free (RetroError *error)
{
  if (error == NULL)
    return;

  g_critical ("Unhandled error: %s", error->message);
  g_error_free (error);
}
