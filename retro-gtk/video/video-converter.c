/* lol.c  Pixel format convertion functions.
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

#include "video-converter.h"
#include <stdlib.h>
#include <fcntl.h>

guchar *xrgb1555_to_rgb888 (const guint16 *data, guint width, guint height, gsize pitch) {
	guchar get_r (guint16 pixel) {
		guchar color = ((pixel >> 10) << 3);
		return color;
	}

	guchar get_g (guint16 pixel) {
		guchar color = ((pixel >> 5) << 3);
		return color;
	}

	guchar get_b (guint16 pixel) {
		guchar color = (pixel << 3);
		return color;
	}

	gsize rgb_pitch = width*3;
	guchar *rgb = (guchar *) malloc (height * rgb_pitch);

	gsize i, j;
	for (i = 0 ; i < height ; i++) {
		gsize xrgb_row = i* (pitch / sizeof (guint16));
		gsize rgb_row = i*rgb_pitch;

		for (j = 0 ; j < width ; j++) {
			gsize xrgb_col = j;
			gsize rgb_col = j*3;

			gsize rgb_pixel_i = rgb_row+rgb_col;

			// red
			rgb[rgb_pixel_i]   = get_r (data[xrgb_row+xrgb_col]);
			// green
			rgb[rgb_pixel_i+1] = get_g (data[xrgb_row+xrgb_col]);
			// blue
			rgb[rgb_pixel_i+2] = get_b (data[xrgb_row+xrgb_col]);
		}
	}

	return rgb;
}

guchar *xrgb8888_to_rgb888 (const guchar *data, guint width, guint height, gsize pitch) {
	gsize rgb_pitch = width*3;
	guchar *rgb = (guchar *) malloc (height * rgb_pitch);

	gsize i, j;
	for (i = 0 ; i < height ; i++) {
		gsize xrgb_row = i*pitch;
		gsize rgb_row = i*rgb_pitch;
		for (j = 0 ; j < width ; j++) {
			gsize xrgb_col = j*4;
			gsize rgb_col = j*3;

			rgb[rgb_row+rgb_col]   = data[xrgb_row+xrgb_col+2];
			rgb[rgb_row+rgb_col+1] = data[xrgb_row+xrgb_col+1];
			rgb[rgb_row+rgb_col+2] = data[xrgb_row+xrgb_col];
		}
	}

	return rgb;
}

guchar *rgb565_to_rgb888 (const guint16 *data, guint width, guint height, gsize pitch) {
	guchar get_r (guint16 pixel) {
		guchar color = ((pixel >> 11) << 3);
		return color;
	}

	guchar get_g (guint16 pixel) {
		guchar color = ((pixel >> 5) << 2);
		return color;
	}

	guchar get_b (guint16 pixel) {
		guchar color = (pixel << 3);
		return color;
	}

	gsize rgb_pitch = width*3;
	guchar *rgb = (guchar *) malloc (height * rgb_pitch);

	gsize i, j;
	for (i = 0 ; i < height ; i++) {
		gsize xrgb_row = i* (pitch / sizeof (guint16));
		gsize rgb_row = i*rgb_pitch;

		for (j = 0 ; j < width ; j++) {
			gsize xrgb_col = j;
			gsize rgb_col = j*3;

			gsize rgb_pixel_i = rgb_row+rgb_col;

			// red
			rgb[rgb_pixel_i]   = get_r (data[xrgb_row+xrgb_col]);
			// green
			rgb[rgb_pixel_i+1] = get_g (data[xrgb_row+xrgb_col]);
			// blue
			rgb[rgb_pixel_i+2] = get_b (data[xrgb_row+xrgb_col]);
		}
	}

	return rgb;
}


GdkPixbuf* video_to_pixbuf (void* data, guint width, guint height, gsize pitch, gint pixel_format) {
	guint8 *vid = (guint8 *) data;

	guchar *rgb = NULL;

	switch (pixel_format) {
		case RETRO_PIXEL_FORMAT_0RGB1555:
			rgb = xrgb1555_to_rgb888 (data, width, height, pitch);
			break;
		case RETRO_PIXEL_FORMAT_XRGB8888:
			rgb = xrgb8888_to_rgb888 (data, width, height, pitch);
			break;
		case RETRO_PIXEL_FORMAT_RGB565:
			rgb = rgb565_to_rgb888 (data, width, height, pitch);
			break;
		default:
			rgb = xrgb1555_to_rgb888 (data, width, height, pitch);
			break;
	}

	gsize rgb_pitch = 3 * width;

	GdkColorspace colorspace = GDK_COLORSPACE_RGB;
	gboolean has_alpha = FALSE;
	gint bits_per_sample = 8;
	gint rowstride = rgb_pitch;

	void rgb_free (guchar *pixels, gpointer data) {
		free (pixels);
	}

	GdkPixbuf *pb = gdk_pixbuf_new_from_data (rgb, colorspace, has_alpha, bits_per_sample, width, height, rowstride, rgb_free, NULL);

	return pb;
}

