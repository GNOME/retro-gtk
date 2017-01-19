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

static gboolean get_input_device_capabilities (RetroCore *self, guint64 *capabilities) {
	RetroInput *input;

	input = retro_core_get_input_interface (self);

	g_return_val_if_fail (input != NULL, FALSE);

	*capabilities = retro_input_get_device_capabilities (input);

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

static gboolean get_variable (RetroCore *self, RetroVariable *variable) {
	RetroVariables *variables;
	gchar *result;

	variables = retro_core_get_variables_interface (self);

	g_return_val_if_fail (variables != NULL, FALSE);

	result = retro_variables_get_variable (variables, variable->key);
	variable->value = result ? result : "";

	return !!result;
}

static gboolean get_variable_update (RetroCore *self, gboolean *update) {
	RetroVariables *variables;

	variables = retro_core_get_variables_interface (self);

	g_return_val_if_fail (variables != NULL, FALSE);

	*update = retro_variables_get_variable_update (variables);

	return TRUE;
}

static gboolean set_disk_control_interface (RetroCore *self, RetroDiskControlCallback *callback) {
	retro_core_set_disk_control_interface (self, RETRO_DISK_CONTROL (retro_disk_control_new (self, callback)));

	return TRUE;
}

static gboolean set_input_descriptors (RetroCore *self, RetroInputDescriptor *descriptors) {
	RetroInput *input;
	int length;

	input = retro_core_get_input_interface (self);

	g_return_val_if_fail (input != NULL, FALSE);

	for (length = 0 ; descriptors[length].description ; length++);
	retro_input_set_descriptors (input, descriptors, length);

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

static gboolean set_variables (RetroCore *self, RetroVariable *variable_array) {
	RetroVariables *variables;
	int length;

	variables = retro_core_get_variables_interface (self);

	g_return_val_if_fail (variables != NULL, FALSE);

	for (length = 0 ; variable_array[length].key && variable_array[length].value ; length++);
	retro_variables_set_variable (variables, variable_array, length);

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

	case RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES:
		return get_input_device_capabilities (self, (guint64 *) data);

	case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
		return get_libretro_path (self, (const gchar* *) data);

	case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
		return get_log_callback (self, (RetroLogCallback *) data);

	case RETRO_ENVIRONMENT_GET_OVERSCAN:
		return get_overscan (self, (gboolean *) data);

	case RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE:
		return get_rumble_callback (self, (RetroRumbleCallback *) data);

	case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
		return get_save_directory (self, (const gchar* *) data);

	case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
		return get_system_directory (self, (const gchar* *) data);

	case RETRO_ENVIRONMENT_GET_VARIABLE:
		return get_variable (self, (RetroVariable *) data);

	case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
		return get_variable_update (self, (gboolean *) data);

	case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
		set_disk_control_interface (self, (RetroDiskControlCallback *) data);

	case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
		return set_input_descriptors (self, (RetroInputDescriptor *) data);

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

	case RETRO_ENVIRONMENT_SET_VARIABLES:
		return set_variables (self, (RetroVariable *) data);

	case RETRO_ENVIRONMENT_SHUTDOWN:
		return shutdown (self);

	case RETRO_ENVIRONMENT_GET_CAMERA_INTERFACE:
	case RETRO_ENVIRONMENT_GET_LANGUAGE:
	case RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE:
	case RETRO_ENVIRONMENT_GET_PERF_INTERFACE:
	case RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE:
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

static gboolean on_environment_interface (unsigned cmd, gpointer data) {
	RetroCore *self;

	self = retro_core_get_cb_data ();

	if (self == NULL)
		g_assert_not_reached ();

	return environment_core_command (self, cmd, data);
}

static void on_video_refresh (guint8* data, guint width, guint height, gsize pitch) {
	RetroCore *self;
	RetroVideo *video;

	self = retro_core_get_cb_data ();

	if (self == NULL)
		g_return_if_reached ();

	video = retro_core_get_video_interface (self);

	if (video == NULL)
		g_return_if_reached ();

	retro_video_render (video, data, pitch * height, width, height, pitch);
}

gpointer retro_core_get_module_video_refresh_cb (RetroCore *self) {
	return on_video_refresh;
}

static void on_audio_sample (gint16 left, gint16 right) {
	RetroCore *self;
	RetroAudio *audio;

	self = retro_core_get_cb_data ();

	if (self == NULL)
		g_return_if_reached ();

	audio = retro_core_get_audio_interface (self);

	if (audio == NULL)
		g_return_if_reached ();

	retro_audio_play_sample (audio, left, right);
}

static gsize on_audio_sample_batch (gint16* data, int frames) {
	RetroCore *self;
	RetroAudio *audio;

	self = retro_core_get_cb_data ();

	if (self == NULL)
		g_return_val_if_reached (0);

	audio = retro_core_get_audio_interface (self);

	if (audio == NULL)
		g_return_val_if_reached (0);

	return retro_audio_play_batch (audio, data, frames * 2, frames);
}

static void on_input_poll () {
	RetroCore *self;
	RetroInput *input;

	self = retro_core_get_cb_data ();

	if (self == NULL)
		g_return_if_reached ();

	input = retro_core_get_input_interface (self);

	if (input == NULL)
		g_return_if_reached ();

	retro_input_poll (input);
}

static gint16 on_input_state (guint port, guint device, guint index, guint id) {
	RetroCore *self;
	RetroInput *input;

	self = retro_core_get_cb_data ();

	if (self == NULL)
		g_return_val_if_reached (0);

	input = retro_core_get_input_interface (self);

	if (input == NULL)
		g_return_val_if_reached (0);

	return retro_input_get_state (input, port, device, index, id);
}

void retro_core_set_environment_interface (RetroCore *self) {
	RetroModule *module;
	RetroCallbackSetter set_environment;

	module = self->module;
	set_environment = retro_module_get_set_environment (module);

	retro_core_push_cb_data (self);
	set_environment (on_environment_interface);
	retro_core_pop_cb_data ();
}

void retro_core_set_callbacks (RetroCore *self) {
	RetroModule *module;
	RetroCallbackSetter set_video_refresh;
	RetroCallbackSetter set_audio_sample;
	RetroCallbackSetter set_audio_sample_batch;
	RetroCallbackSetter set_input_poll;
	RetroCallbackSetter set_input_state;

	module = self->module;
	set_video_refresh = retro_module_get_set_video_refresh (module);
	set_audio_sample = retro_module_get_set_audio_sample (module);
	set_audio_sample_batch = retro_module_get_set_audio_sample_batch (module);
	set_input_poll = retro_module_get_set_input_poll (module);
	set_input_state = retro_module_get_set_input_state (module);

	retro_core_push_cb_data (self);
	set_video_refresh (on_video_refresh);
	set_audio_sample (on_audio_sample);
	set_audio_sample_batch (on_audio_sample_batch);
	set_input_poll (on_input_poll);
	set_input_state (on_input_state);
	retro_core_pop_cb_data ();
}
