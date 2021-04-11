// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-game-info-private.h"

#include <gio/gio.h>

G_DEFINE_BOXED_TYPE (RetroGameInfo, retro_game_info, retro_game_info_copy, retro_game_info_free)

RetroGameInfo *
retro_game_info_new (const gchar  *uri,
                     gboolean      needs_full_path,
                     GError      **error)
{
  g_autoptr (RetroGameInfo) self = NULL;
  g_autoptr (GFile) file = NULL;

  g_return_val_if_fail (uri != NULL, NULL);

  self = g_slice_new0 (RetroGameInfo);

  file = g_file_new_for_uri (uri);
  self->path = g_file_get_path (file);
  if (needs_full_path)
    self->data = g_new0 (guint8, 0);
  else {
    GError *tmp_error = NULL;

    g_file_get_contents (self->path, (gchar **) &self->data, &self->size, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);

      return NULL;
    }
  }

  return g_steal_pointer (&self);
}

RetroGameInfo *
retro_game_info_copy (RetroGameInfo *self)
{
  RetroGameInfo *copy;

  g_return_val_if_fail (self, NULL);

  copy = g_slice_new0 (RetroGameInfo);

  copy->path = g_strdup (self->path);
  copy->data = g_memdup2 (self, self->size);
  copy->size = self->size;
  copy->meta = g_strdup (self->meta);

  return copy;
}

void
retro_game_info_free (RetroGameInfo *self)
{
  g_return_if_fail (self);

  g_free (self->path);
  g_free (self->data);
  g_free (self->meta);

  g_slice_free (RetroGameInfo, self);
}
