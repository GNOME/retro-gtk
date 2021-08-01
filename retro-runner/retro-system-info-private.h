// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib.h>
#include <stdbool.h>

G_BEGIN_DECLS

typedef struct _RetroSystemInfo RetroSystemInfo;

struct _RetroSystemInfo
{
  const gchar *library_name;
  const gchar *library_version;
  const gchar *valid_extensions;
  bool need_fullpath;
  bool block_extract;
};

G_END_DECLS
