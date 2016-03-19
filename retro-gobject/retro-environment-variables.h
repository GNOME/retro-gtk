// This file is part of Retro. License: GPLv3

#ifndef __RETRO_ENVIRONMENT_VARIABLES_H__
#define __RETRO_ENVIRONMENT_VARIABLES_H__

#include "retro-gobject-internal.h"
#include "libretro-environment.h"

inline gboolean get_variable (RetroVariables *self, RetroVariable *variable) {
	gchar *result = retro_variables_get_variable (self, variable->key);
	variable->value = result ? result : "";
	return result ? TRUE : FALSE;
}

inline gboolean set_variables (RetroVariables *self, RetroVariable *variables) {
	int length;
	for (length = 0 ; variables[length].key && variables[length].value ; length++);
	retro_variables_set_variable (self, variables, length);
	return TRUE;
}

inline gboolean get_variable_update (RetroVariables *self, gboolean *update) {
	*update = retro_variables_get_variable_update (self);
	return TRUE;
}

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

#endif

