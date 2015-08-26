// This file is part of RetroGtk. License: GPLv3

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

