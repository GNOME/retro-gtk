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

#include "retro-core-cb-data.h"

#include "retro-gobject-internal.h"
#include "libretro-environment.h"

#include "retro-environment-core.h"
#include "retro-environment-video.h"
#include "retro-environment-input.h"
#include "retro-environment-variables.h"
#include "retro-environment-interfaces.h"

gpointer retro_core_get_module_environment_interface (RetroCore *self) {
	gboolean real_cb (unsigned cmd, gpointer data) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_assert_not_reached ();

		if (environment_video_command (retro_core_get_video_interface (cb_data), cmd, data))
			return TRUE;

		if (environment_input_command (retro_core_get_input_interface (cb_data), cmd, data))
			return TRUE;

		if (environment_variables_command (retro_core_get_variables_interface (cb_data), cmd, data))
			return TRUE;

		if (environment_interfaces_command (cb_data, cmd, data))
			return TRUE;

		return environment_core_command (cb_data, cmd, data);
	}

	return real_cb;
}



gpointer retro_core_get_module_video_refresh_cb (RetroCore *self) {
	gboolean real_cb (guint8* data, guint width, guint height, gsize pitch) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_if_reached ();

		RetroVideo *handler = retro_core_get_video_interface (cb_data);

		if (!handler) g_return_if_reached ();

		retro_video_render (handler, data, pitch * height, width, height, pitch);
	}

	return real_cb;
}

gpointer retro_core_get_module_audio_sample_cb (RetroCore *self) {
	gboolean real_cb (gint16 left, gint16 right) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_if_reached ();

		RetroAudio *handler = retro_core_get_audio_interface (cb_data);

		if (!handler) g_return_if_reached ();

		retro_audio_play_sample (handler, left, right);
	}

	return real_cb;
}

gpointer retro_core_get_module_audio_sample_batch_cb (RetroCore *self) {
	gboolean real_cb (gint16* data, int frames) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_val_if_reached (0);

		RetroAudio *handler = retro_core_get_audio_interface (cb_data);

		if (!handler) g_return_val_if_reached (0);

		return retro_audio_play_batch (handler, data, frames * 2, frames);
	}

	return real_cb;
}

gpointer retro_core_get_module_input_poll_cb (RetroCore *self) {
	gboolean real_cb () {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_if_reached ();

		RetroInputInterface *handler = retro_core_get_input_interface (cb_data);

		if (!handler) g_return_if_reached ();

		retro_input_interface_poll (handler);
	}

	return real_cb;
}

gpointer retro_core_get_module_input_state_cb (RetroCore *self) {
	gboolean real_cb (guint port, guint device, guint index, guint id) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_val_if_reached (0);

		RetroInputInterface *handler = retro_core_get_input_interface (cb_data);

		if (!handler) g_return_val_if_reached (0);

		return retro_input_interface_get_state (handler, port, device, index, id);
	}

	return real_cb;
}

