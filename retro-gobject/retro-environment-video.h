// This file is part of Retro. License: GPLv3

#ifndef __RETRO_ENVIRONMENT_VIDEO_H__
#define __RETRO_ENVIRONMENT_VIDEO_H__

#include "retro-gobject-internal.h"
#include "libretro-environment.h"

inline gboolean set_rotation (RetroVideo *self, const RetroRotation *rotation) {
	retro_video_set_rotation (self, *rotation);
	return TRUE;
}

inline gboolean get_overscan (RetroVideo *self, gboolean *overcan) {
	*overcan = retro_video_get_overscan (self);
	return TRUE;
}

inline gboolean get_can_dupe (RetroVideo *self, gboolean *can_dupe) {
	*can_dupe = retro_video_get_can_dupe (self);
	return TRUE;
}

inline gboolean set_pixel_format (RetroVideo *self, const RetroPixelFormat *pixel_format) {
	retro_video_set_pixel_format (self, *pixel_format);
	return TRUE;
}

inline gboolean environment_video_command (RetroVideo *self, unsigned cmd, gpointer data) {
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
		default:
			return FALSE;
	}
}

#endif

