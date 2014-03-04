/* retro-core-extern.c  Functions helping to identify a Core.
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

#include <glib.h>

typedef void RetroCore;

typedef gboolean (*RetroEnvironmentCallback) (guint cmd, void* data, void* user_data);
typedef void (*RetroVideoRefresh) (guint8* data, guint width, guint height, gsize pitch, void* user_data);
typedef void (*RetroAudioSample) (gint16 left, gint16 right, void* user_data);
typedef gsize (*RetroAudioSampleBatch) (gint16* data, int size, gsize frames, void* user_data);
typedef void (*RetroInputPoll) (void* user_data);
typedef gint16 (*RetroInputState) (guint port, guint device, guint index, guint id, void* user_data);

RetroEnvironmentCallback retro_core_get_environment_cb (RetroCore* self, gpointer* result_target);
RetroVideoRefresh retro_core_get_video_refresh_cb (RetroCore* self, gpointer* result_target);
RetroAudioSample retro_core_get_audio_sample_cb (RetroCore* self, gpointer* result_target);
RetroAudioSampleBatch retro_core_get_audio_sample_batch_cb (RetroCore* self, gpointer* result_target);
RetroInputPoll retro_core_get_input_poll_cb (RetroCore* self, gpointer* result_target);
RetroInputState retro_core_get_input_state_cb (RetroCore* self, gpointer* result_target);

static __thread void *global_self;

void retro_core_set_global_self (RetroCore *self) {
	global_self = (void *) self;
}

gpointer retro_core_get_real_environment_cb (RetroCore *self) {
	gboolean real_cb (guint cmd, gpointer data) {
		if (global_self) {
			void *result;
			RetroEnvironmentCallback cb = retro_core_get_environment_cb (global_self, &result);
			return cb (cmd, data, result);
		}
	
		g_assert_not_reached ();
		return FALSE;
	}
	
	return real_cb;
}

gpointer retro_core_get_real_video_refresh_cb (RetroCore *self) {
	gboolean real_cb (guint8* data, guint width, guint height, gsize pitch) {
		if (global_self) {
			void *result;
			RetroVideoRefresh cb = retro_core_get_video_refresh_cb (global_self, &result);
			cb (data, width, height, pitch, result);
			return;
		}
	
		g_assert_not_reached ();
	}
	
	return real_cb;
}

gpointer retro_core_get_real_audio_sample_cb (RetroCore *self) {
	gboolean real_cb (gint16 left, gint16 right) {
		if (global_self) {
			void *result;
			RetroAudioSample cb = retro_core_get_audio_sample_cb (global_self, &result);
			cb(left, right, result);
			return;
		}
	
		g_assert_not_reached ();
	}
	
	return real_cb;
}

gpointer retro_core_get_real_audio_sample_batch_cb (RetroCore *self) {
	gboolean real_cb (gint16* data, int frames) {
		if (global_self) {
			void *result;
			RetroAudioSampleBatch cb = retro_core_get_audio_sample_batch_cb (global_self, &result);
			return cb (data, frames * 2, frames, result);
		}
	
		g_assert_not_reached ();
		return 0;
	}
	
	return real_cb;
}

gpointer retro_core_get_real_input_poll_cb (RetroCore *self) {
	gboolean real_cb () {
		if (global_self) {
			void *result;
			RetroInputPoll cb = retro_core_get_input_poll_cb (global_self, &result);
			cb (result);
			return;
		}
	
		g_assert_not_reached ();
	}
	
	return real_cb;
}

gpointer retro_core_get_real_input_state_cb (RetroCore *self) {
	gboolean real_cb (guint port, guint device, guint index, guint id) {
		if (global_self) {
			void *result;
			RetroInputState cb = retro_core_get_input_state_cb (global_self, &result);
			return cb (port, device, index, id, result);
		}
	
		g_assert_not_reached ();
		return 0;
	}
	
	return real_cb;
}

