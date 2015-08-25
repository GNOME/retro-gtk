// This file is part of RetroGtk. License: GPLv3

#include "video-converter.h"
#include <stdlib.h>
#include <fcntl.h>

typedef guchar (*pixel_get_color_t) (const void *pixel);
typedef guchar (*pixel16_get_color_t) (guint16 pixel);
typedef guchar (*pixel32_get_color_t) (guint32 pixel);

guchar *video_to_rgba8888 (const void *data,
                         size_t pixel_size,
                         guint width,
                         guint height,
                         gsize pitch,
                         pixel_get_color_t get_r,
                         pixel_get_color_t get_g,
                         pixel_get_color_t get_b) {
	gsize rgba_pitch = width * 4;
	guchar *rgba = (guchar *) malloc (height * rgba_pitch);

	gsize i, j;
	for (i = 0 ; i < height ; i++) {
		gsize xrgb_row = i * pitch;
		gsize rgba_row = i * rgba_pitch;

		for (j = 0 ; j < width ; j++) {
			gsize xrgb_col = j * pixel_size;
			gsize rgba_col = j * 4;

			const void *xrgb_pixel = xrgb_row + xrgb_col + data;
			gsize rgba_pixel = rgba_row + rgba_col;

			const gsize R = 0;
			const gsize G = 1;
			const gsize B = 2;
			const gsize A = 3;

			rgba[rgba_pixel + R] = get_r (xrgb_pixel);
			rgba[rgba_pixel + G] = get_g (xrgb_pixel);
			rgba[rgba_pixel + B] = get_b (xrgb_pixel);
			rgba[rgba_pixel + A] = 0xff;
		}
	}

	return rgba;
}

guchar *xrgb1555_to_rgba8888 (const void *data, guint width, guint height, gsize pitch) {
	guchar get_r (const void *data) {
		const guint16 *pixel = (const guint16 *) data;

		return ((*pixel) >> 10) << 3;
	}

	guchar get_g (const void *data) {
		const guint16 *pixel = (const guint16 *) data;

		return ((*pixel) >> 5) << 3;
	}

	guchar get_b (const void *data) {
		const guint16 *pixel = (const guint16 *) data;

		return (*pixel) << 3;
	}

	return video_to_rgba8888 (data, sizeof (guint16), width, height, pitch, get_r, get_g, get_b);
}

guchar *xrgb8888_to_rgba8888 (const void *data, guint width, guint height, gsize pitch) {
	guchar get_r (const void *data) {
		const guint32 *pixel = (const guint32 *) data;

		return (*pixel) >> 16;
	}

	guchar get_g (const void *data) {
		const guint32 *pixel = (const guint32 *) data;

		return (*pixel) >> 8;
	}

	guchar get_b (const void *data) {
		const guint32 *pixel = (const guint32 *) data;

		return (*pixel);
	}

	return video_to_rgba8888 (data, sizeof (guint32), width, height, pitch, get_r, get_g, get_b);
}

guchar *rgb565_to_rgba8888 (const void *data, guint width, guint height, gsize pitch) {
	guchar get_r (const void *data) {
		const guint16 *pixel = (const guint16 *) data;

		return ((*pixel) >> 11) << 3;
	}

	guchar get_g (const void *data) {
		const guint16 *pixel = (const guint16 *) data;

		return ((*pixel) >> 5) << 2;
	}

	guchar get_b (const void *data) {
		const guint16 *pixel = (const guint16 *) data;

		return (*pixel) << 3;
	}

	return video_to_rgba8888 (data, sizeof (guint16), width, height, pitch, get_r, get_g, get_b);
}

GdkPixbuf* video_to_pixbuf (void* data, guint width, guint height, gsize pitch, gint pixel_format) {
	guchar *rgba = NULL;

	switch (pixel_format) {
		case RETRO_PIXEL_FORMAT_0RGB1555:
			rgba = xrgb1555_to_rgba8888 (data, width, height, pitch);

			break;
		case RETRO_PIXEL_FORMAT_XRGB8888:
			rgba = xrgb8888_to_rgba8888 (data, width, height, pitch);

			break;
		case RETRO_PIXEL_FORMAT_RGB565:
			rgba = rgb565_to_rgba8888 (data, width, height, pitch);

			break;
		default:
			return NULL;
	}

	gsize rgba_pitch = width * 4;

	GdkColorspace colorspace = GDK_COLORSPACE_RGB;
	gboolean has_alpha = TRUE;
	gint bits_per_sample = 8;
	gint rowstride = rgba_pitch;

	void rgba_free (guchar *pixels, gpointer data) {
		free (pixels);
	}

	GdkPixbuf *pb = gdk_pixbuf_new_from_data (rgba, colorspace, has_alpha, bits_per_sample, width, height, rowstride, rgba_free, NULL);

	return pb;
}

