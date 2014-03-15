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

#include "retro-core-global.h"

#include "retro-internal.h"
#include "retro-core-interfaces.h"
#include "retro-environment-command.h"

gboolean retro_core_dispatch_environment_command (RetroCore *self, RetroEnvironment *interface, RetroEnvironmentCommand cmd, gpointer data);

gpointer retro_core_get_module_environment_interface (RetroCore *self) {
	gboolean real_cb (RetroEnvironmentCommand cmd, gpointer data) {
		RetroCore *global_self = retro_core_get_global_self ();
		
		if (global_self) {
			if (retro_core_set_callback_interfaces (global_self, cmd, data)) return TRUE;
			
			RetroEnvironment *interface = retro_core_get_environment_interface (global_self);
			return retro_core_dispatch_environment_command (global_self, interface, cmd, data);
		}
	
		g_assert_not_reached ();
		return FALSE;
	}
	
	return real_cb;
}

gpointer retro_core_get_module_video_refresh_cb (RetroCore *self) {
	gboolean real_cb (guint8* data, guint width, guint height, gsize pitch) {
		RetroCore *global_self = retro_core_get_global_self ();
		if (global_self) {
			void *result;
			RetroCoreVideoRefresh cb = retro_core_get_video_refresh_cb (global_self, &result);
			cb (data, pitch * height, width, height, pitch, result);
			return;
		}
	
		g_assert_not_reached ();
	}
	
	return real_cb;
}

gpointer retro_core_get_module_audio_sample_cb (RetroCore *self) {
	gboolean real_cb (gint16 left, gint16 right) {
		RetroCore *global_self = retro_core_get_global_self ();
		if (global_self) {
			void *result;
			RetroCoreAudioSample cb = retro_core_get_audio_sample_cb (global_self, &result);
			cb(left, right, result);
			return;
		}
	
		g_assert_not_reached ();
	}
	
	return real_cb;
}

gpointer retro_core_get_module_audio_sample_batch_cb (RetroCore *self) {
	gboolean real_cb (gint16* data, int frames) {
		RetroCore *global_self = retro_core_get_global_self ();
		if (global_self) {
			void *result;
			RetroCoreAudioSampleBatch cb = retro_core_get_audio_sample_batch_cb (global_self, &result);
			return cb (data, frames * 2, frames, result);
		}
	
		g_assert_not_reached ();
		return 0;
	}
	
	return real_cb;
}

gpointer retro_core_get_module_input_poll_cb (RetroCore *self) {
	gboolean real_cb () {
		RetroCore *global_self = retro_core_get_global_self ();
		if (global_self) {
			void *result;
			RetroCoreInputPoll cb = retro_core_get_input_poll_cb (global_self, &result);
			cb (result);
			return;
		}
	
		g_assert_not_reached ();
	}
	
	return real_cb;
}

gpointer retro_core_get_module_input_state_cb (RetroCore *self) {
	gboolean real_cb (guint port, guint device, guint index, guint id) {
		RetroCore *global_self = retro_core_get_global_self ();
		if (global_self) {
			void *result;
			RetroCoreInputState cb = retro_core_get_input_state_cb (global_self, &result);
			return cb (port, device, index, id, result);
		}
	
		g_assert_not_reached ();
		return 0;
	}
	
	return real_cb;
}

gboolean retro_core_dispatch_environment_command (RetroCore *self, RetroEnvironment *interface, RetroEnvironmentCommand cmd, gpointer data) {
	if (!self || !interface || !data) return FALSE;
	
	switch (cmd) {
		case RETRO_ENVIRONMENT_COMMAND_SET_ROTATION:
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_rotation (interface, *((RetroRotation *) data));
			return TRUE;
		
		case RETRO_ENVIRONMENT_COMMAND_GET_OVERSCAN: {
			gboolean *overscan = (gboolean *) data;
			*(overscan) = RETRO_ENVIRONMENT_GET_INTERFACE (interface)->get_overscan (interface);
			return TRUE;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_GET_CAN_DUPE: {
			gboolean *can_dupe = (gboolean *) data;
			*(can_dupe) = RETRO_ENVIRONMENT_GET_INTERFACE (interface)->get_can_dupe (interface);
			return TRUE;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_SET_MESSAGE: {
			gboolean result = FALSE;
			g_signal_emit_by_name ((RetroEnvironment*) interface, "set_message", (RetroMessage *) data, &result);
			return result;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_SHUTDOWN: {
			gboolean result = FALSE;
			g_signal_emit_by_name ((RetroEnvironment*) interface, "shutdown", &result);
			return result;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_SET_PERFORMANCE_LEVEL:
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_performance_level (interface, *((RetroPerformanceLevel *) data));
			return TRUE;
		
		case RETRO_ENVIRONMENT_COMMAND_GET_SYSTEM_DIRECTORY: {
			const gchar **directory = (const gchar **) data;
			*(directory) = RETRO_ENVIRONMENT_GET_INTERFACE (interface)->get_system_directory (interface);
			return TRUE;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_SET_PIXEL_FORMAT:
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_pixel_format (interface, *((RetroPixelFormat *) data));
			return TRUE;
		
		case RETRO_ENVIRONMENT_COMMAND_SET_INPUT_DESCRIPTORS: {
			RetroDeviceInputDescriptor *array = (RetroDeviceInputDescriptor *) data;
			
			int length;
			for (length = 0 ; array[length].description ; length++);
			
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_input_descriptors (interface, array, length);
			return TRUE;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_SET_KEYBOARD_CALLBACK:
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_keyboard_callback (interface, (RetroKeyboardCallback *) data);
			return TRUE;
		
		case RETRO_ENVIRONMENT_COMMAND_SET_DISK_CONTROL_INTERFACE:
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_disk_control_interface (interface, (RetroDiskControlCallback *) data);
			return TRUE;
		
		case RETRO_ENVIRONMENT_COMMAND_SET_HW_RENDER:
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_hw_render (interface, (RetroHardwareRenderCallback *) data);
			return TRUE;
		
		case RETRO_ENVIRONMENT_COMMAND_GET_VARIABLE: {
			RetroVariable *variable = (RetroVariable *) data;
			variable->value = RETRO_ENVIRONMENT_GET_INTERFACE (interface)->get_variable (interface, variable->key);
			return (gboolean) variable->value;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_SET_VARIABLES: {
			RetroVariable *array = (RetroVariable *) data;
			
			int length;
			for (length = 0 ; array[length].key && array[length].value ; length++);
			
			return RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_variables (interface, array, length);
		}
		
		case RETRO_ENVIRONMENT_COMMAND_GET_VARIABLE_UPDATE: {
			gboolean *variable_update = (gboolean *) data;
			*(variable_update) = RETRO_ENVIRONMENT_GET_INTERFACE (interface)->get_variable_update (interface);
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_variable_update (interface, FALSE);
			return TRUE;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_SET_SUPPORT_NO_GAME:
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_support_no_game (interface, *((gboolean *) data));
			return TRUE;
		
		case RETRO_ENVIRONMENT_COMMAND_GET_LIBRETRO_PATH: {
			const gchar **directory = (const gchar **) data;
			*(directory) = RETRO_ENVIRONMENT_GET_INTERFACE (interface)->get_libretro_path (interface);
			return TRUE;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_SET_AUDIO_CALLBACK:
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_audio_callback (interface, (RetroAudioCallback *) data);
			return TRUE;
		
		case RETRO_ENVIRONMENT_COMMAND_SET_FRAME_TIME_CALLBACK:
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_frame_time_callback (interface, (RetroFrameTimeCallback *) data);
			return TRUE;
		
		case RETRO_ENVIRONMENT_COMMAND_GET_INPUT_DEVICE_CAPABILITIES: {
			guint64 *input_device_capabilities = (guint64 *) data;
			*(input_device_capabilities) = RETRO_ENVIRONMENT_GET_INTERFACE (interface)->get_input_device_capabilities (interface);
			return TRUE;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_GET_CONTENT_DIRECTORY: {
			const gchar **directory = (const gchar **) data;
			*(directory) = RETRO_ENVIRONMENT_GET_INTERFACE (interface)->get_content_directory (interface);
			return TRUE;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_GET_SAVE_DIRECTORY: {
			const gchar **directory = (const gchar **) data;
			*(directory) = RETRO_ENVIRONMENT_GET_INTERFACE (interface)->get_save_directory (interface);
			return TRUE;
		}
		
		case RETRO_ENVIRONMENT_COMMAND_SET_SYSTEM_AV_INFO:
			RETRO_ENVIRONMENT_GET_INTERFACE (interface)->set_system_av_info (interface, (RetroSystemAvInfo *) data);
			return TRUE;
		
		default:
			return FALSE;
	}
}
