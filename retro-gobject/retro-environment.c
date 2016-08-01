// This file is part of Retro. License: GPLv3

#include "retro-gobject-internal.h"
#include "libretro-environment.h"

#include "retro-environment-video.h"
#include "retro-environment-input.h"
#include "retro-environment-variables.h"
#include "retro-environment-interfaces.h"

typedef struct {
	const gchar *msg;
	guint frames;
} RetroMessage;

static gboolean get_content_directory (RetroCore *self, const gchar* *content_directory) {
	*(content_directory) = retro_core_get_content_directory (self);

	return TRUE;
}

static gboolean get_libretro_path (RetroCore *self, const gchar* *libretro_directory) {
	*(libretro_directory) = retro_core_get_libretro_path (self);

	return TRUE;
}

static gboolean get_save_directory (RetroCore *self, const gchar* *save_directory) {
	*(save_directory) = retro_core_get_save_directory (self);

	return TRUE;
}

static gboolean get_system_directory (RetroCore *self, const gchar* *system_directory) {
	*(system_directory) = retro_core_get_system_directory (self);

	return TRUE;
}

static gboolean set_audio_callback (RetroCore *self, RetroAudioCallback *callback) {
	retro_core_set_audio_callback (self, callback);

	return TRUE;
}

static gboolean set_disk_control_interface (RetroCore *self, RetroDiskControlCallback *callback) {
	retro_core_set_disk_control_interface (self, RETRO_DISK_CONTROL (retro_disk_control_new (self, callback)));

	return TRUE;
}

static gboolean set_frame_time_callback (RetroCore *self, RetroCoreFrameTimeCallback *callback) {
	retro_core_set_frame_time_callback (self, RETRO_FRAME_TIME (retro_core_frame_time_new (callback)));

	return TRUE;
}

static gboolean set_keyboard_callback (RetroCore *self, RetroKeyboardCallback *callback) {
	retro_core_set_keyboard_callback (self, callback);

	return TRUE;
}

static gboolean set_message (RetroCore *self, const RetroMessage *message) {
	gboolean result = FALSE;
	g_signal_emit_by_name (self, "message", message->msg, message->frames, &result);

	return result;
}

static gboolean set_performance_level (RetroCore *self, RetroPerfLevel *performance_level) {
	retro_core_set_performance_level (self, *performance_level);

	return TRUE;
}

static gboolean set_support_no_game (RetroCore *self, gboolean *support_no_game) {
	retro_core_set_support_no_game (self, *support_no_game);

	return TRUE;
}

static gboolean set_system_av_info (RetroCore *self, RetroSystemAvInfo *system_av_info) {
	retro_core_set_av_info (self, retro_av_info_new (system_av_info));

	return TRUE;
}

static gboolean shutdown (RetroCore *self) {
	gboolean result = FALSE;
	g_signal_emit_by_name (self, "shutdown", &result);

	return result;
}

static gboolean environment_core_command (RetroCore *self, unsigned cmd, gpointer data) {
	if (!self)
		return FALSE;

	switch (cmd) {
	case RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY:
		return get_content_directory (self, (const gchar* *) data);

	case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
		return get_libretro_path (self, (const gchar* *) data);

	case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
		return get_save_directory (self, (const gchar* *) data);

	case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
		return get_system_directory (self, (const gchar* *) data);

	case RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK:
		return set_audio_callback (self, (RetroAudioCallback *) data);

	case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
		set_disk_control_interface (self, (RetroDiskControlCallback *) data);

	case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK:
		return set_frame_time_callback (self, (RetroCoreFrameTimeCallback *) data);

	case RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK:
		return set_keyboard_callback (self, (RetroKeyboardCallback *) data);

	case RETRO_ENVIRONMENT_SET_MESSAGE:
		return set_message (self, (RetroMessage *) data);

	case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
		return set_performance_level (self, (RetroPerfLevel *) data);

	case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
		return set_support_no_game (self, (gboolean *) data);

	case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
		return set_system_av_info (self, (RetroSystemAvInfo *) data);

	case RETRO_ENVIRONMENT_SHUTDOWN:
		return shutdown (self);

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
	void real_cb (guint8* data, guint width, guint height, gsize pitch) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_if_reached ();

		RetroVideo *handler = retro_core_get_video_interface (cb_data);

		if (!handler) g_return_if_reached ();

		retro_video_render (handler, data, pitch * height, width, height, pitch);
	}

	return real_cb;
}

gpointer retro_core_get_module_audio_sample_cb (RetroCore *self) {
	void real_cb (gint16 left, gint16 right) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_if_reached ();

		RetroAudio *handler = retro_core_get_audio_interface (cb_data);

		if (!handler) g_return_if_reached ();

		retro_audio_play_sample (handler, left, right);
	}

	return real_cb;
}

gpointer retro_core_get_module_audio_sample_batch_cb (RetroCore *self) {
	gsize real_cb (gint16* data, int frames) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_val_if_reached (0);

		RetroAudio *handler = retro_core_get_audio_interface (cb_data);

		if (!handler) g_return_val_if_reached (0);

		return retro_audio_play_batch (handler, data, frames * 2, frames);
	}

	return real_cb;
}

gpointer retro_core_get_module_input_poll_cb (RetroCore *self) {
	void real_cb () {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_if_reached ();

		RetroInput *handler = retro_core_get_input_interface (cb_data);

		if (!handler) g_return_if_reached ();

		retro_input_poll (handler);
	}

	return real_cb;
}

gpointer retro_core_get_module_input_state_cb (RetroCore *self) {
	gint16 real_cb (guint port, guint device, guint index, guint id) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_return_val_if_reached (0);

		RetroInput *handler = retro_core_get_input_interface (cb_data);

		if (!handler) g_return_val_if_reached (0);

		return retro_input_get_state (handler, port, device, index, id);
	}

	return real_cb;
}

