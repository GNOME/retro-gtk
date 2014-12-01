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

#ifndef __RETRO_ENVIRONMENT_INPUT_H__
#define __RETRO_ENVIRONMENT_INPUT_H__

#include "retro-gobject-internal.h"
#include "libretro-environment.h"

inline gboolean environment_input_command (RetroInput *self, unsigned cmd, gpointer data) {
	if (!self) return FALSE;

	switch (cmd) {
		case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
			return set_input_desciptors (self, (RetroInputDescriptor *) data);

		case RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES:
			return get_input_device_capabilities (self, (guint64 *) data);

		default:
			return FALSE;
	}
}

inline gboolean set_input_desciptors (RetroInput *self, const RetroInputDescriptor *descriptors) {
	int length;
	for (length = 0 ; descriptors[length].description ; length++);
	retro_input_set_descriptors (self, descriptors, length);
	return TRUE;
}

inline gboolean get_input_device_capabilities (RetroInput *self, guint64 *capabilities) {
	*capabilities = retro_input_get_device_capabilities (self);
	return TRUE;
}

#endif

