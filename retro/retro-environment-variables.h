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

#ifndef __RETRO_ENVIRONMENT_VARIABLES_H__
#define __RETRO_ENVIRONMENT_VARIABLES_H__

#include "retro-gobject-internal.h"
#include "libretro-environment.h"

inline gboolean environment_variables_command (RetroVariables *self, unsigned cmd, gpointer data) {
	if (!self) return FALSE;

	switch (cmd) {
		case RETRO_ENVIRONMENT_GET_VARIABLE:
			return get_variable (self, (RetroVariable *) data);

		case RETRO_ENVIRONMENT_SET_VARIABLES:
			return set_variables (self, (RetroVariable *) data);

		case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
			return get_variable_update (self, (gboolean *) data);

		default:
			return FALSE;
	}
}

inline gboolean get_variable (RetroVariables *self, RetroVariable *variable) {
	gchar *result = retro_variables_get_variable (self, variable->key);
	variable->value = result ? result : "";
	return result ? TRUE : FALSE;
}

inline gboolean set_variables (RetroVariables *self, const RetroVariable *variables) {
	int length;
	for (length = 0 ; variables[length].key && variables[length].value ; length++);
	retro_variables_set_variable (self, variables, length);
	return TRUE;
}

inline gboolean get_variable_update (RetroVariables *self, gboolean *update) {
	*update = retro_variables_get_variable_update (self);
	return TRUE;
}

#endif

