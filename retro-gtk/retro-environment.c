// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-core-private.h"

#include <stdbool.h>
#include "retro-input-private.h"
#include "retro-pixdata-private.h"

#define AUDIO_SAMPLES_LENGTH 512

#define RETRO_ENVIRONMENT_EXPERIMENTAL 0x10000
#define RETRO_ENVIRONMENT_PRIVATE 0x20000
#define RETRO_ENVIRONMENT_SET_ROTATION 1
#define RETRO_ENVIRONMENT_GET_OVERSCAN 2
#define RETRO_ENVIRONMENT_GET_CAN_DUPE 3
#define RETRO_ENVIRONMENT_SET_MESSAGE 6
#define RETRO_ENVIRONMENT_SHUTDOWN 7
#define RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL 8
#define RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY 9
#define RETRO_ENVIRONMENT_SET_PIXEL_FORMAT 10
#define RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS 11
#define RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK 12
#define RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE 13
#define RETRO_ENVIRONMENT_SET_HW_RENDER 14
#define RETRO_ENVIRONMENT_GET_VARIABLE 15
#define RETRO_ENVIRONMENT_SET_VARIABLES 16
#define RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE 17
#define RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME 18
#define RETRO_ENVIRONMENT_GET_LIBRETRO_PATH 19
#define RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK 22
#define RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK 21
#define RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE 23
#define RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES 24
#define RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE (25 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_GET_CAMERA_INTERFACE (26 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_GET_LOG_INTERFACE 27
#define RETRO_ENVIRONMENT_GET_PERF_INTERFACE 28
#define RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE 29
#define RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY 30 /* Old name, kept for compatibility. */
#define RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY 30
#define RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY 31
#define RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO 32
#define RETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK 33
#define RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO 34
#define RETRO_ENVIRONMENT_SET_CONTROLLER_INFO 35
#define RETRO_ENVIRONMENT_SET_MEMORY_MAPS (36 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_SET_GEOMETRY 37
#define RETRO_ENVIRONMENT_GET_USERNAME 38
#define RETRO_ENVIRONMENT_GET_LANGUAGE 39
#define RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER (40 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE (41 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS (42 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_SET_HW_RENDER_CONTEXT_NEGOTIATION_INTERFACE (43 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_SET_SERIALIZATION_QUIRKS 44

enum RetroLanguage {
  RETRO_LANGUAGE_ENGLISH = 0,
  RETRO_LANGUAGE_JAPANESE,
  RETRO_LANGUAGE_FRENCH,
  RETRO_LANGUAGE_SPANISH,
  RETRO_LANGUAGE_GERMAN,
  RETRO_LANGUAGE_ITALIAN,
  RETRO_LANGUAGE_DUTCH,
  RETRO_LANGUAGE_PORTUGUESE_BRAZIL,
  RETRO_LANGUAGE_PORTUGUESE_PORTUGAL,
  RETRO_LANGUAGE_RUSSIAN,
  RETRO_LANGUAGE_KOREAN,
  RETRO_LANGUAGE_CHINESE_TRADITIONAL,
  RETRO_LANGUAGE_CHINESE_SIMPLIFIED,
  RETRO_LANGUAGE_ESPERANTO,
  RETRO_LANGUAGE_POLISH,
  RETRO_LANGUAGE_VIETNAMESE,
  RETRO_LANGUAGE_ARABIC,
  RETRO_LANGUAGE_DEFAULT = RETRO_LANGUAGE_ENGLISH,
};

enum RetroLogLevel {
  RETRO_LOG_LEVEL_DEBUG = 0,
  RETRO_LOG_LEVEL_INFO,
  RETRO_LOG_LEVEL_WARN,
  RETRO_LOG_LEVEL_ERROR,
};

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

static gboolean
rumble_callback_set_rumble_state (guint             port,
                                  RetroRumbleEffect effect,
                                  guint16           strength)
{
  RetroCore *self;
  RetroController *controller;

  self = retro_core_get_cb_data ();

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  if (!g_hash_table_contains (self->controllers, &port))
    return FALSE;

  controller = g_hash_table_lookup (self->controllers, &port);

  if (controller == NULL)
    return FALSE;

  if (!retro_controller_get_supports_rumble (controller))
    return FALSE;

  retro_controller_set_rumble_state (controller, effect, strength);

  return TRUE;
}

static void
on_log (guint level, const gchar *format, ...)
{
  RetroCore *self;
  const gchar *log_domain;
  GLogLevelFlags log_level;
  gchar *message;
  va_list args;

  self = retro_core_get_cb_data ();
  if (!self)
    g_return_if_reached ();

  switch (level) {
  case RETRO_LOG_LEVEL_DEBUG:
    log_level = G_LOG_LEVEL_DEBUG;

    break;
  case RETRO_LOG_LEVEL_INFO:
    log_level = G_LOG_LEVEL_MESSAGE;

    break;
  case RETRO_LOG_LEVEL_WARN:
    log_level = G_LOG_LEVEL_WARNING;

    break;
  case RETRO_LOG_LEVEL_ERROR:
    log_level = G_LOG_LEVEL_CRITICAL;

    break;
  default:
    g_debug ("Unexpected log level: %d", level);

    return;
  }

  // Get the arguments, set up the formatted message, pass it to the logging
  // method and free it.
  va_start (args, format);
  message = g_strdup_vprintf (format, args);

  log_domain = retro_core_get_name (self);
  g_signal_emit_by_name (self, "log", log_domain, log_level, message);

  g_free (message);
}

/* Environment commands */

static gboolean
get_can_dupe (RetroCore *self,
              gboolean  *can_dupe)
{
  *can_dupe = TRUE;

  return TRUE;
}

static gboolean
get_content_directory (RetroCore    *self,
                       const gchar **content_directory)
{
  *(content_directory) = retro_core_get_content_directory (self);

  return TRUE;
}

static gboolean
get_input_device_capabilities (RetroCore *self,
                               guint64   *capabilities)
{
  *capabilities = retro_core_get_controller_capabilities (self);

  return TRUE;
}

static gboolean
get_language (RetroCore *self,
              unsigned  *language)
{
  static const struct { const gchar *locale; enum RetroLanguage language; } values[] = {
    { "ar", RETRO_LANGUAGE_ARABIC },
    { "de", RETRO_LANGUAGE_GERMAN },
    { "en", RETRO_LANGUAGE_ENGLISH },
    { "eo", RETRO_LANGUAGE_ESPERANTO },
    { "es", RETRO_LANGUAGE_SPANISH },
    { "fr", RETRO_LANGUAGE_FRENCH },
    { "it", RETRO_LANGUAGE_ITALIAN },
    { "jp", RETRO_LANGUAGE_JAPANESE },
    { "ko", RETRO_LANGUAGE_KOREAN },
    { "nl", RETRO_LANGUAGE_DUTCH },
    { "pl", RETRO_LANGUAGE_POLISH },
    { "pt_BR", RETRO_LANGUAGE_PORTUGUESE_BRAZIL },
    { "pt_PT", RETRO_LANGUAGE_PORTUGUESE_PORTUGAL },
    { "ru", RETRO_LANGUAGE_RUSSIAN },
    { "vi", RETRO_LANGUAGE_VIETNAMESE },
    { "zh_CN", RETRO_LANGUAGE_CHINESE_SIMPLIFIED },
    { "zh_HK", RETRO_LANGUAGE_CHINESE_TRADITIONAL },
    { "zh_SG", RETRO_LANGUAGE_CHINESE_SIMPLIFIED },
    { "zh_TW", RETRO_LANGUAGE_CHINESE_TRADITIONAL },
    { "C", RETRO_LANGUAGE_DEFAULT },
  };

  const gchar * const *locales = g_get_language_names ();
  gsize locale_i, language_i = 0;

  for (locale_i = 0; locales[locale_i] != NULL; locale_i++) {
    for (language_i = 0;
         !g_str_equal (values[language_i].locale, "C") &&
         !g_str_equal (locales[locale_i], values[language_i].locale);
         language_i++);
    if (!g_str_equal (values[language_i].locale, "C"))
      break;
  }

  *language = values[language_i].language;

  return TRUE;
}

static gboolean
get_libretro_path (RetroCore    *self,
                   const gchar **libretro_directory)
{
  *(libretro_directory) = retro_core_get_libretro_path (self);

  return TRUE;
}

static gboolean
get_log_callback (RetroCore        *self,
                  RetroLogCallback *cb)
{
  cb->log = on_log;

  return TRUE;
}

static gboolean
get_overscan (RetroCore *self,
              gboolean  *overscan)
{
  *overscan = self->overscan;

  return TRUE;
}

static gboolean
get_rumble_callback (RetroCore           *self,
                     RetroRumbleCallback *cb)
{
  cb->set_rumble_state = rumble_callback_set_rumble_state;

  return TRUE;
}

static gboolean
get_save_directory (RetroCore    *self,
                    const gchar **save_directory)
{
  *(save_directory) = retro_core_get_save_directory (self);

  return TRUE;
}

static gboolean
get_system_directory (RetroCore    *self,
                      const gchar **system_directory)
{
  *(system_directory) = retro_core_get_system_directory (self);

  return TRUE;
}

static gboolean
get_variable (RetroCore     *self,
              RetroVariable *variable)
{
  RetroOption *option;
  const gchar *value;

  if (!retro_core_has_option (self, variable->key))
    return FALSE;

  option = retro_core_get_option (self, variable->key);
  value = retro_option_get_value (option);
  variable->value = value;

  return TRUE;
}

// The data must be bool, not gboolean, the sizes can be different.
static gboolean
get_variable_update (RetroCore *self,
                     bool      *update)
{
  *update = retro_core_get_variable_update (self);

  return TRUE;
}

static gboolean
set_disk_control_interface (RetroCore                *self,
                            RetroDiskControlCallback *callback)
{
  self->disk_control_callback = callback;

  return TRUE;
}

static gboolean
set_geometry (RetroCore         *self,
              RetroGameGeometry *geometry)
{
  retro_core_set_geometry (self, geometry);

  return TRUE;
}

static gboolean
set_input_descriptors (RetroCore            *self,
                       RetroInputDescriptor *descriptors)
{
  int length;

  for (length = 0 ; descriptors[length].description ; length++);
  retro_core_set_controller_descriptors (self, descriptors, length);

  return TRUE;
}

static gboolean
set_keyboard_callback (RetroCore             *self,
                       RetroKeyboardCallback *callback)
{
  self->keyboard_callback = *callback;

  return TRUE;
}

static gboolean
set_message (RetroCore          *self,
             const RetroMessage *message)
{
  g_signal_emit_by_name (self, "message", message->msg, message->frames);

  return TRUE;
}

static gboolean
set_pixel_format (RetroCore              *self,
                  const RetroPixelFormat *pixel_format)
{
  self->pixel_format = *pixel_format;

  return TRUE;
}

static gboolean
set_rotation (RetroCore           *self,
              const RetroRotation *rotation)
{
  self->rotation = *rotation;

  return TRUE;
}

static gboolean
set_support_no_game (RetroCore *self,
                     gboolean  *support_no_game)
{
  retro_core_set_support_no_game (self, *support_no_game);

  return TRUE;
}

static gboolean
set_system_av_info (RetroCore         *self,
                    RetroSystemAvInfo *system_av_info)
{
  retro_core_set_system_av_info (self, system_av_info);

  return TRUE;
}

static gboolean
set_variables (RetroCore     *self,
               RetroVariable *variable_array)
{
  int i;

  for (i = 0 ; variable_array[i].key && variable_array[i].value ; i++)
    retro_core_insert_variable (self, &variable_array[i]);

  return TRUE;
}

static gboolean
shutdown (RetroCore *self)
{
  g_signal_emit_by_name (self, "shutdown");

  return TRUE;
}

static gboolean
environment_core_command (RetroCore *self,
                          unsigned   cmd,
                          gpointer   data)
{
  if (!self)
    return FALSE;

  switch (cmd) {
  case RETRO_ENVIRONMENT_GET_CAN_DUPE:
    return get_can_dupe (self, (gboolean *) data);

  case RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY:
    return get_content_directory (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES:
    return get_input_device_capabilities (self, (guint64 *) data);

  case RETRO_ENVIRONMENT_GET_LANGUAGE:
    return get_language (self, (unsigned *) data);

  case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
    return get_libretro_path (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
    return get_log_callback (self, (RetroLogCallback *) data);

  case RETRO_ENVIRONMENT_GET_OVERSCAN:
    return get_overscan (self, (gboolean *) data);

  case RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE:
    return get_rumble_callback (self, (RetroRumbleCallback *) data);

  case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
    return get_save_directory (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
    return get_system_directory (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_VARIABLE:
    return get_variable (self, (RetroVariable *) data);

  case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
    return get_variable_update (self, (bool *) data);

  case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
    return set_disk_control_interface (self, (RetroDiskControlCallback *) data);

  case RETRO_ENVIRONMENT_SET_GEOMETRY:
    return set_geometry (self, (RetroGameGeometry *) data);

  case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
    return set_input_descriptors (self, (RetroInputDescriptor *) data);

  case RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK:
    return set_keyboard_callback (self, (RetroKeyboardCallback *) data);

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
  case RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER:
  case RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE:
  case RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE:
  case RETRO_ENVIRONMENT_GET_PERF_INTERFACE:
  case RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE:
  case RETRO_ENVIRONMENT_GET_USERNAME:
  case RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK:
  case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
  case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK:
  case RETRO_ENVIRONMENT_SET_HW_RENDER:
  case RETRO_ENVIRONMENT_SET_HW_RENDER_CONTEXT_NEGOTIATION_INTERFACE:
  case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
  case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
  case RETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK:
  case RETRO_ENVIRONMENT_SET_SERIALIZATION_QUIRKS:
  case RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO:
  case RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS:
  default:
    return FALSE;
  }
}

/* Core callbacks */

static gboolean
on_environment_interface (unsigned cmd,
                          gpointer data)
{
  RetroCore *self;

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_assert_not_reached ();

  return environment_core_command (self, cmd, data);
}

static void
on_video_refresh (guint8 *data,
                  guint   width,
                  guint   height,
                  gsize   pitch)
{
  RetroCore *self;
  RetroPixdata pixdata;

  if (data == NULL)
    return;

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_return_if_reached ();

  if (retro_core_is_running_ahead (self))
    return;

  retro_pixdata_init (&pixdata,
                      data, self->pixel_format,
                      pitch, width, height,
                      self->aspect_ratio);

  g_signal_emit_by_name (self, "video-output", &pixdata);
}

// TODO This is internal, make it private as soon as possible.
gpointer
retro_core_get_module_video_refresh_cb (RetroCore *self)
{
  return on_video_refresh;
}

static void
on_audio_sample (gint16 left,
                 gint16 right)
{
  RetroCore *self;

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_return_if_reached ();

  if (retro_core_is_running_ahead (self))
    return;

  if (self->sample_rate <= 0.0)
    return;

  if (self->audio_samples == NULL) {
    self->audio_samples = g_malloc (AUDIO_SAMPLES_LENGTH * sizeof (gint16));
    self->audio_samples_length = 0;
  }

  self->audio_samples[self->audio_samples_length] = left;
  self->audio_samples[self->audio_samples_length + 1] = right;
  self->audio_samples_length += 2;

  if (self->audio_samples_length == AUDIO_SAMPLES_LENGTH) {
    g_signal_emit_by_name (self, "audio_output", self->audio_samples, AUDIO_SAMPLES_LENGTH, self->sample_rate);
    self->audio_samples_length = 0;
  }
}

static gsize
on_audio_sample_batch (gint16 *data,
                       int     frames)
{
  RetroCore *self;

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_return_val_if_reached (0);

  if (retro_core_is_running_ahead (self))
    // FIXME What should be returned?
    return 0;

  if (self->sample_rate <= 0.0)
    return 0;

  g_signal_emit_by_name (self, "audio_output", data, frames * 2, self->sample_rate);

  // FIXME What should be returned?
  return 0;
}

static void
on_input_poll ()
{
  RetroCore *self;

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_return_if_reached ();

  retro_core_poll_controllers (self);
}

static gint16
on_input_state (guint port,
                guint device,
                guint index,
                guint id)
{
  RetroCore *self;
  RetroInput input;

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_return_val_if_reached (0);

  retro_input_init (&input, device, id, index);

  return retro_core_get_controller_input_state (self, port, &input);
}

// TODO This is internal, make it private as soon as possible.
void
retro_core_set_environment_interface (RetroCore *self)
{
  RetroModule *module;
  RetroCallbackSetter set_environment;

  module = self->module;
  set_environment = retro_module_get_set_environment (module);

  retro_core_push_cb_data (self);
  set_environment (on_environment_interface);
  retro_core_pop_cb_data ();
}

// TODO This is internal, make it private as soon as possible.
void
retro_core_set_callbacks (RetroCore *self)
{
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
