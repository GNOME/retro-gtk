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

#include "retro-gobject-internal.h"

inline gboolean retro_environment_set_rotation (RetroVideoHandler *self, const RetroRotation *rotation) {
	if (!self) g_assert_not_reached ();

	retro_video_handler_set_rotation (self, *rotation);

	return TRUE;
}

inline gboolean retro_environment_get_overscan (RetroVariablesHandler *self, gboolean *overcan) {
	if (!self) g_assert_not_reached ();

	*overcan = retro_video_handler_get_overscan (self);

	return TRUE;
}

inline gboolean retro_environment_get_can_dupe (RetroVariablesHandler *self, gboolean *can_dupe) {
	if (!self) g_assert_not_reached ();

	*can_dupe = retro_video_handler_get_can_dupe (self);

	return TRUE;
}

inline gboolean retro_environment_set_pixel_format (RetroVideoHandler *self, const RetroPixelFormat *pixel_format) {
	if (!self) g_assert_not_reached ();

	retro_video_handler_set_pixel_format (self, *pixel_format);

	return TRUE;
}

