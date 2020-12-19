// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

gboolean retro_is_debug (void);

#define retro_debug(...) \
  G_STMT_START { \
    if (G_UNLIKELY (retro_is_debug ())) \
      g_debug (__VA_ARGS__); \
  } G_STMT_END

G_END_DECLS
