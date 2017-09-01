// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-game-info.h"

G_DEFINE_BOXED_TYPE (RetroGameInfo, retro_game_info, retro_game_info_copy, retro_game_info_free)

RetroGameInfo *
retro_game_info_new (const gchar *file_name)
{
  RetroGameInfo *self;

  g_return_val_if_fail (file_name != NULL, NULL);

  self = g_slice_new0 (RetroGameInfo);

  self->path = g_strdup (file_name);
  self->data = g_new0 (guint8, 0);
  return self;
}

RetroGameInfo *
retro_game_info_new_with_data (const gchar  *file_name,
                               GError      **error)
{
  RetroGameInfo *self;

  g_return_val_if_fail (file_name != NULL, NULL);

  self = g_slice_new0 (RetroGameInfo);

  self->path = g_strdup (file_name);
  g_file_get_contents (file_name, (gchar **) &self->data, &self->size, error);
  return self;
}

RetroGameInfo *
retro_game_info_copy (RetroGameInfo *self)
{
  RetroGameInfo *copy;

  g_return_val_if_fail (self, NULL);

  copy = g_slice_new0 (RetroGameInfo);

  copy->path = g_strdup (self->path);
  copy->data = g_memdup (self, self->size);
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
