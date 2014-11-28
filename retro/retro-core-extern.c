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

#include "retro-core-cb-data.h"

#include "retro-gobject-internal.h"
#include "libretro-environment.h"
#include "retro-core-interfaces.h"
#include "retro-video-handler.h"
#include "retro-input-handler.h"
#include "retro-variables-handler.h"

gboolean retro_core_dispatch_environment_command (RetroCore *self, unsigned cmd, gpointer data);

gpointer retro_core_get_module_environment_interface (RetroCore *self) {
	gboolean real_cb (unsigned cmd, gpointer data) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (cb_data) {
			if (retro_core_set_callback_interfaces (cb_data, cmd, data)) return TRUE;

			if (video_handler_command (retro_core_get_video_handler (cb_data), cmd, data)) return TRUE;
			if (input_handler_command (retro_core_get_input_handler (cb_data), cmd, data)) return TRUE;
			if (variables_handler_command (retro_core_get_variables_handler (cb_data), cmd, data)) return TRUE;

			return retro_core_dispatch_environment_command (cb_data, cmd, data);
		}

		g_assert_not_reached ();
		return FALSE;
	}

	return real_cb;
}



gpointer retro_core_get_module_video_refresh_cb (RetroCore *self) {
	gboolean real_cb (guint8* data, guint width, guint height, gsize pitch) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_if_reached ();

		RetroVideoHandler *handler = retro_core_get_video_handler (cb_data);

		if (!handler) g_return_if_reached ();

		retro_video_handler_render (handler, data, pitch * height, width, height, pitch);
	}

	return real_cb;
}

gpointer retro_core_get_module_audio_sample_cb (RetroCore *self) {
	gboolean real_cb (gint16 left, gint16 right) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_if_reached ();

		RetroAudioHandler *handler = retro_core_get_audio_handler (cb_data);

		if (!handler) g_return_if_reached ();

		retro_audio_handler_play_sample (handler, left, right);
	}

	return real_cb;
}

gpointer retro_core_get_module_audio_sample_batch_cb (RetroCore *self) {
	gboolean real_cb (gint16* data, int frames) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_val_if_reached (0);

		RetroAudioHandler *handler = retro_core_get_audio_handler (cb_data);

		if (!handler) g_return_val_if_reached (0);

		return retro_audio_handler_play_batch (handler, data, frames * 2, frames);
	}

	return real_cb;
}

gpointer retro_core_get_module_input_poll_cb (RetroCore *self) {
	gboolean real_cb () {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_if_reached ();

		RetroInputHandler *handler = retro_core_get_input_handler (cb_data);

		if (!handler) g_return_if_reached ();

		retro_input_handler_poll (handler);
	}

	return real_cb;
}

gpointer retro_core_get_module_input_state_cb (RetroCore *self) {
	gboolean real_cb (guint port, guint device, guint index, guint id) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_val_if_reached (0);

		RetroInputHandler *handler = retro_core_get_input_handler (cb_data);

		if (!handler) g_return_val_if_reached (0);

		return retro_input_handler_get_state (handler, port, device, index, id);
	}

	return real_cb;
}

gboolean retro_core_dispatch_environment_command (RetroCore *self, unsigned cmd, gpointer data) {
	if (!self) return FALSE;

	switch (cmd) {
		case RETRO_ENVIRONMENT_SET_MESSAGE: {
			gboolean result = FALSE;
			g_signal_emit_by_name (self, "set-message", (RetroMessage *) data, &result);
			return result;
		}

		case RETRO_ENVIRONMENT_SHUTDOWN: {
			gboolean result = FALSE;
			g_signal_emit_by_name (self, "shutdown", &result);
			return result;
		}

		case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
			retro_core_set_performance_level (self, *((RetroPerfLevel *) data));
			return TRUE;

		case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY: {
			const gchar **directory = (const gchar **) data;
			*(directory) = retro_core_get_system_directory (self);
			return TRUE;
		}

		case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE: {
			RetroCoreDiskController* callback = retro_core_disk_controller_new ((RetroCoreDiskControllerCallback *) data);
			retro_core_set_disk_control_interface (self, RETRO_DISK_CONTROLLER (callback));
			return TRUE;
		}

		case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
			retro_core_set_support_no_game (self, *((gboolean *) data));
			return TRUE;

		case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH: {
			const gchar **directory = (const gchar **) data;
			*(directory) = retro_core_get_libretro_path (self);
			return TRUE;
		}

		case RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK: {
			RetroCoreAudioInput* callback = retro_core_audio_input_new ((RetroCoreAudioInputCallback *) data);
			retro_core_set_audio_input_callback (self, RETRO_AUDIO_INPUT (callback));
			return TRUE;
		}

		case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK: {
			RetroCoreFrameTime* callback = retro_core_frame_time_new ((RetroCoreFrameTimeCallback *) data);
			retro_core_set_frame_time_callback (self, RETRO_FRAME_TIME (callback));
			return TRUE;
		}

		case RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY: {
			const gchar **directory = (const gchar **) data;
			*(directory) = retro_core_get_content_directory (self);
			return TRUE;
		}

		case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY: {
			const gchar **directory = (const gchar **) data;
			*(directory) = retro_core_get_save_directory (self);
			return TRUE;
		}

		case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
			retro_core_set_av_info (self, retro_av_info_new ((RetroSystemAvInfo *) data));
			return TRUE;

		default:
			return FALSE;
	}
}
