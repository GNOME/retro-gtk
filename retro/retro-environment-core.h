/* Retro  GObject libretro wrapper.
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

#ifndef __RETRO_ENVIRONMENT_CORE_H__
#define __RETRO_ENVIRONMENT_CORE_H__

#include "retro-gobject-internal.h"
#include "libretro-environment.h"

inline gboolean environment_core_command (RetroCore *self, unsigned cmd, gpointer data) {
	if (!self) return FALSE;

	switch (cmd) {
		case RETRO_ENVIRONMENT_SET_MESSAGE:
			return set_message ((RetroMessage *) data);

		case RETRO_ENVIRONMENT_SHUTDOWN:
			return set_shutdown ();

		case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
			return set_performance_level (self, (RetroPerfLevel *) data);

		case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
			get_system_directory (self, (gchar **) data);

		case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
			set_disk_control_interface (self, (RetroCoreDiskControllerCallback *) data);

		case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
			return set_support_no_game (self, (gboolean *) data);

		case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
			return get_libretro_path (self, (gchar **) data);

		case RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK:
			return set_audio_callback (self, (RetroCoreAudioInputCallback *) data);

		case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK:
			return set_frame_time_callback (self, (RetroCoreFrameTimeCallback *) data);

		case RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY:
			return get_content_directory (self, (gchar **) data);

		case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
			return get_save_directory (self, (gchar **) data);

		case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
			return set_system_av_info (self, (RetroSystemAvInfo *) data);

		case RETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK:
		case RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO:
		case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
		case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
		case RETRO_ENVIRONMENT_SET_GEOMETRY:
		case RETRO_ENVIRONMENT_GET_USERNAME:
		case RETRO_ENVIRONMENT_GET_LANGUAGE:
		default:
			return FALSE;
	}
}

inline gboolean set_message (RetroCore *self, const RetroMessage *message) {
	gboolean result = FALSE;
	g_signal_emit_by_name (self, "message", message, &result);
	return result;
}

inline gboolean set_shutdown (RetroCore *self) {
	gboolean result = FALSE;
	g_signal_emit_by_name (self, "shutdown", &result);
	return result;
}

inline gboolean set_performance_level (RetroCore *self, RetroPerfLevel *performance_level) {
	retro_core_set_performance_level (self, *performance_level);
	return TRUE;
}

inline gboolean get_system_directory (RetroCore *self, const gchar **system_directory) {
	*(system_directory) = retro_core_get_system_directory (self);
	return TRUE;
}

inline gboolean set_disk_control_interface (RetroCore *self, RetroCoreDiskControllerCallback *callback) {
	retro_core_set_disk_control_interface (self, RETRO_DISK_CONTROLLER (retro_core_disk_controller_new (callback)));
	return TRUE;
}

inline gboolean set_support_no_game (RetroCore *self, gboolean *support_no_game) {
	retro_core_set_support_no_game (self, *support_no_game);
	return TRUE;
}

inline gboolean get_libretro_path (RetroCore *self, const gchar **libretro_directory) {
	*(libretro_directory) = retro_core_get_libretro_path (self);
	return TRUE;
}

inline gboolean set_audio_callback (RetroCore *self, RetroCoreAudioInputCallback *callback) {
	retro_core_set_audio_input_callback (self, RETRO_AUDIO_INPUT (retro_core_audio_input_new (callback)));
	return TRUE;
}

inline gboolean set_frame_time_callback (RetroCore *self, RetroCoreFrameTimeCallback *callback) {
	retro_core_set_frame_time_callback (self, RETRO_FRAME_TIME (retro_core_frame_time_new (callback)));
	return TRUE;
}

inline gboolean get_content_directory (RetroCore *self, const gchar **content_directory) {
	*(content_directory) = retro_core_get_content_directory (self);
	return TRUE;
}

inline gboolean get_save_directory (RetroCore *self, const gchar **save_directory) {
	*(save_directory) = retro_core_get_save_directory (self);
	return TRUE;
}

inline gboolean set_system_av_info (RetroCore *self, RetroSystemAvInfo *system_av_info) {
	retro_core_set_av_info (self, retro_av_info_new (system_av_info));
	return TRUE;
}

#endif

