/* lol.h  Pixel format convertion functions.
 * Copyright (C) 2014  Adrien Plazas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef __LOL_H__
#define __LOL_H__

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <cairo.h>

#define RETRO_PIXEL_FORMAT_0RGB1555 0
#define RETRO_PIXEL_FORMAT_XRGB8888 1
#define RETRO_PIXEL_FORMAT_RGB565   2

guchar *xrgb1555_to_rgb888 (const guint16 *data, guint width, guint height, gsize pitch);
guchar *xrgb8888_to_rgb888 (const guchar *data, guint width, guint height, gsize pitch);
guchar *rgb565_to_rgb888   (const guint16 *data, guint width, guint height, gsize pitch);

GdkPixbuf* video_to_pixbuf (void* data, guint width, guint height, gsize pitch, gint video_format);

GdkPixbuf* video_to_argb8888_pixbuf (void* data, guint width, guint height, gsize pitch, gint video_format);

#endif

