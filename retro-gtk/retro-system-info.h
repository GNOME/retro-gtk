// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_SYSTEM_INFO_H
#define RETRO_SYSTEM_INFO_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef struct _RetroSystemInfo RetroSystemInfo;

struct _RetroSystemInfo
{
  gchar *library_name;
  gchar *library_version;
  gchar *valid_extensions;
  gboolean need_fullpath;
  gboolean block_extract;
};

G_END_DECLS

#endif /* RETRO_SYSTEM_INFO_H */
