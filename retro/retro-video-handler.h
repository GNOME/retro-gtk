/* Copyright (C) 2014  Adrien Plazas
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

#ifndef __RETRO_VIDEO_HANDLER_H__
#define __RETRO_VIDEO_HANDLER_H__

#include "retro-gobject-internal.h"
#include "libretro-environment.h"

inline gboolean video_handler_command (RetroVideoHandler *self, unsigned cmd, gpointer data) {
	if (!self) return FALSE;

	switch (cmd) {
		case RETRO_ENVIRONMENT_SET_ROTATION:
			return set_rotation (self, (RetroRotation *) data);

		case RETRO_ENVIRONMENT_GET_OVERSCAN:
			return get_overscan (self, (gboolean *) data);

		case RETRO_ENVIRONMENT_GET_CAN_DUPE:
			return get_can_dupe (self, (gboolean *) data);

		case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
			return set_pixel_format (self, (RetroPixelFormat *) data);

		case RETRO_ENVIRONMENT_SET_HW_RENDER:
			return FALSE; // TODO

		default:
			return FALSE;
	}
}

inline gboolean set_rotation (RetroVideoHandler *self, const RetroRotation *rotation) {
	retro_video_handler_set_rotation (self, *rotation);
	return TRUE;
}

inline gboolean get_overscan (RetroVideoHandler *self, gboolean *overcan) {
	*overcan = retro_video_handler_get_overscan (self);
	return TRUE;
}

inline gboolean get_can_dupe (RetroVideoHandler *self, gboolean *can_dupe) {
	*can_dupe = retro_video_handler_get_can_dupe (self);
	return TRUE;
}

inline gboolean set_pixel_format (RetroVideoHandler *self, const RetroPixelFormat *pixel_format) {
	retro_video_handler_set_pixel_format (self, *pixel_format);
	return TRUE;
}

#endif

