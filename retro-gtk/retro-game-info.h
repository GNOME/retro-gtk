// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_GAME_INFO_H
#define RETRO_GAME_INFO_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_GAME_INFO (retro_game_info_get_type())

typedef struct _RetroGameInfo RetroGameInfo;

struct _RetroGameInfo
{
  gchar *path;
  gpointer data;
  gsize size;
  gchar *meta;
};

RetroGameInfo *retro_game_info_new (const gchar *file_name);
RetroGameInfo * retro_game_info_new_with_data (const gchar  *file_name,
                                               GError      **error);
RetroGameInfo *retro_game_info_copy (RetroGameInfo *self);
void retro_game_info_free (RetroGameInfo *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (RetroGameInfo, retro_game_info_free)

G_END_DECLS

#endif /* RETRO_GAME_INFO_H */
