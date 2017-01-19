// This file is part of Retro. License: GPLv3

#include "retro-gobject-internal.h"
#include "libretro-environment.h"

typedef struct {
	gpointer log;
} RetroLogCallback;

typedef struct {
	const gchar *msg;
	guint frames;
} RetroMessage;

typedef struct {
	gpointer set_rumble_state;
} RetroRumbleCallback;

static gboolean rumble_callback_set_rumble_state (guint port, RetroRumbleEffect effect, guint16 strength) {
	RetroCore *cb_data = retro_core_get_cb_data ();
	if (!cb_data)
		g_return_val_if_reached (FALSE);

	RetroRumble *interface = retro_core_get_rumble_interface (cb_data);
	if (!interface)
		g_return_val_if_reached (FALSE);

	return RETRO_RUMBLE_GET_INTERFACE (interface)->set_rumble_state (interface, port, effect, strength);
}

static void log_callback_log (guint level, const char *format, ...) {
	RetroCore *cb_data = retro_core_get_cb_data ();
	if (!cb_data)
		g_return_if_reached ();

	RetroLog *interface = retro_core_get_log_interface (cb_data);
	if (!interface)
		g_return_if_reached ();

	// Get the arguments, set up the formatted message,
	// pass it to the logging method and free it.
	va_list args;
	va_start (args, format);
	char *message = g_strdup_vprintf (format, args);
	RETRO_LOG_GET_INTERFACE (interface)->log (interface, level, message);
	g_free (message);
}

static gboolean get_can_dupe (RetroCore *self, gboolean *can_dupe) {
	RetroVideo *video;

	video = retro_core_get_video_interface (self);

	g_return_val_if_fail (video != NULL, FALSE);

	*can_dupe = retro_video_get_can_dupe (video);

	return TRUE;
}

static gboolean get_content_directory (RetroCore *self, const gchar* *content_directory) {
	*(content_directory) = retro_core_get_content_directory (self);

	return TRUE;
}

static gboolean get_input_device_capabilities (RetroInput *self, guint64 *capabilities) {
	*capabilities = retro_input_get_device_capabilities (self);

	return TRUE;
}

static gboolean get_libretro_path (RetroCore *self, const gchar* *libretro_directory) {
	*(libretro_directory) = retro_core_get_libretro_path (self);

	return TRUE;
}

static gboolean get_log_callback (RetroCore *self, RetroLogCallback *cb) {
	void *interface_exists = retro_core_get_log_interface (self);
	if (!interface_exists)
		return FALSE;

	cb->log = log_callback_log;

	return TRUE;
}

static gboolean get_overscan (RetroCore *self, gboolean *overcan) {
	RetroVideo *video;

	video = retro_core_get_video_interface (self);

	g_return_val_if_fail (video != NULL, FALSE);

	*overcan = retro_video_get_overscan (video);

	return TRUE;
}

static gboolean get_rumble_callback (RetroCore *self, RetroRumbleCallback *cb) {
	void *interface_exists = retro_core_get_rumble_interface (self);
	if (!interface_exists)
		return FALSE;

	cb->set_rumble_state = rumble_callback_set_rumble_state;

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

static gboolean get_variable (RetroVariables *self, RetroVariable *variable) {
	gchar *result = retro_variables_get_variable (self, variable->key);
	variable->value = result ? result : "";

	return !!result;
}

static gboolean get_variable_update (RetroVariables *self, gboolean *update) {
	*update = retro_variables_get_variable_update (self);

	return TRUE;
}

static gboolean set_disk_control_interface (RetroCore *self, RetroDiskControlCallback *callback) {
	retro_core_set_disk_control_interface (self, RETRO_DISK_CONTROL (retro_disk_control_new (self, callback)));

	return TRUE;
}

static gboolean set_input_descriptors (RetroInput *self, RetroInputDescriptor *descriptors) {
	int length;
	for (length = 0 ; descriptors[length].description ; length++);
	retro_input_set_descriptors (self, descriptors, length);

	return TRUE;
}

static gboolean set_message (RetroCore *self, const RetroMessage *message) {
	gboolean result = FALSE;
	g_signal_emit_by_name (self, "message", message->msg, message->frames, &result);

	return result;
}

static gboolean set_pixel_format (RetroCore *self, const RetroPixelFormat *pixel_format) {
	RetroVideo *video;

	video = retro_core_get_video_interface (self);

	g_return_val_if_fail (video != NULL, FALSE);

	retro_video_set_pixel_format (video, *pixel_format);

	return TRUE;
}

static gboolean set_rotation (RetroCore *self, const RetroRotation *rotation) {
	RetroVideo *video;

	video = retro_core_get_video_interface (self);

	g_return_val_if_fail (video != NULL, FALSE);

	retro_video_set_rotation (video, *rotation);

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

static gboolean set_variables (RetroVariables *self, RetroVariable *variables) {
	int length;
	for (length = 0 ; variables[length].key && variables[length].value ; length++);
	retro_variables_set_variable (self, variables, length);

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
	case RETRO_ENVIRONMENT_GET_CAN_DUPE:
		return get_can_dupe (self, (gboolean *) data);

	case RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY:
		return get_content_directory (self, (const gchar* *) data);

	case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
		return get_libretro_path (self, (const gchar* *) data);

	case RETRO_ENVIRONMENT_GET_OVERSCAN:
		return get_overscan (self, (gboolean *) data);

	case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
		return get_save_directory (self, (const gchar* *) data);

	case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
		return get_system_directory (self, (const gchar* *) data);

	case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
		set_disk_control_interface (self, (RetroDiskControlCallback *) data);

	case RETRO_ENVIRONMENT_SET_MESSAGE:
		return set_message (self, (RetroMessage *) data);

	case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
		return set_pixel_format (self, (RetroPixelFormat *) data);

	case RETRO_ENVIRONMENT_SET_ROTATION:
		return set_rotation (self, (RetroRotation *) data);

	case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
		return set_support_no_game (self, (gboolean *) data);

	case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
		return set_system_av_info (self, (RetroSystemAvInfo *) data);

	case RETRO_ENVIRONMENT_SHUTDOWN:
		return shutdown (self);

	case RETRO_ENVIRONMENT_GET_LANGUAGE:
	case RETRO_ENVIRONMENT_GET_USERNAME:
	case RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK:
	case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
	case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK:
	case RETRO_ENVIRONMENT_SET_GEOMETRY:
	case RETRO_ENVIRONMENT_SET_HW_RENDER:
	case RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK:
	case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
	case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
	case RETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK:
	case RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO:
	default:
		return FALSE;
	}
}

static gboolean environment_input_command (RetroInput *self, unsigned cmd, gpointer data) {
	if (!self)
		return FALSE;

	switch (cmd) {
	case RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES:
		return get_input_device_capabilities (self, (guint64 *) data);

	case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
		return set_input_descriptors (self, (RetroInputDescriptor *) data);

	default:
		return FALSE;
	}
}

static gboolean environment_variables_command (RetroVariables *self, unsigned cmd, gpointer data) {
	if (!self) return FALSE;

	switch (cmd) {
		case RETRO_ENVIRONMENT_GET_VARIABLE:
			return get_variable (self, (RetroVariable *) data);

		case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
			return get_variable_update (self, (gboolean *) data);

		case RETRO_ENVIRONMENT_SET_VARIABLES:
			return set_variables (self, (RetroVariable *) data);

		default:
			return FALSE;
	}
}

static gboolean environment_interfaces_command (RetroCore *self, unsigned cmd, gpointer data) {
	if (!self)
		return FALSE;

	switch (cmd) {
	case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
		return get_log_callback (self, (RetroLogCallback *) data);

	case RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE:
		return get_rumble_callback (self, (RetroRumbleCallback *) data);

	case RETRO_ENVIRONMENT_GET_CAMERA_INTERFACE:
	case RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE:
	case RETRO_ENVIRONMENT_GET_PERF_INTERFACE:
	case RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE:
	default:
		return FALSE;
	}
}

gpointer retro_core_get_module_environment_interface (RetroCore *self) {
	gboolean real_cb (unsigned cmd, gpointer data) {
		RetroCore *cb_data = retro_core_get_cb_data ();

		if (!cb_data) g_assert_not_reached ();

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

