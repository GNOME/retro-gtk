// This file is part of Retro. License: GPLv3

#ifndef __RETRO_ENVIRONMENT_INPUT_H__
#define __RETRO_ENVIRONMENT_INPUT_H__

#include "retro-gobject-internal.h"
#include "libretro-environment.h"

inline gboolean set_input_desciptors (RetroInput *self, RetroInputDescriptor *descriptors) {
	int length;
	for (length = 0 ; descriptors[length].description ; length++);
	retro_input_set_descriptors (self, descriptors, length);
	return TRUE;
}

inline gboolean get_input_device_capabilities (RetroInput *self, guint64 *capabilities) {
	*capabilities = retro_input_get_device_capabilities (self);
	return TRUE;
}

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

#endif

