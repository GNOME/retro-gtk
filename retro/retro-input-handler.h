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

#ifndef __RETRO_INPUT_HANDLER_H__
#define __RETRO_INPUT_HANDLER_H__

#include "retro-gobject-internal.h"

inline gboolean retro_environment_set_input_desciptors (RetroInputHandler *self, const RetroInputDescriptor *descriptors) {
	if (!self) g_return_val_if_reached (FALSE);

	int length;
	for (length = 0 ; descriptors[length].description ; length++);

	retro_input_handler_set_descriptors (self, descriptors, length);

	return TRUE;
}

inline gboolean retro_environment_set_keyboard_callback (RetroInputHandler *self, const RetroKeyboardCallback *callback) {
	if (!self) g_return_val_if_reached (FALSE);

	retro_input_handler_set_keyboard_callback (self, callback);

	return TRUE;
}

inline gboolean retro_environment_get_input_device_capabilities (RetroInputHandler *self, guint64 *capabilities) {
	if (!self) g_return_val_if_reached (FALSE);

	*capabilities = retro_input_handler_get_device_capabilities (self);

	return TRUE;
}

#endif

