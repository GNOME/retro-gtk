#ifndef __LOL_H__
#define __LOL_H__

#include <gtk/gtk.h>
#include "libretro.h"

guchar *xrgb1555_to_rgb888 (const uint16_t *data, unsigned width, unsigned height, size_t pitch);
guchar *xrgb8888_to_rgb888 (const guchar *data, unsigned width, unsigned height, size_t pitch);
guchar *rgb565_to_rgb888   (const uint16_t *data, unsigned width, unsigned height, size_t pitch);

GdkPixbuf* video_to_pixbuf (void* data, guint width, guint height, gsize pitch, gint video_format);

void *set_game_info (struct retro_game_info *game, const char *path, bool need_fullpath);

#endif

