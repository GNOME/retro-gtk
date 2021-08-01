// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-environment-private.h"

#include <stdbool.h>
#include "retro-core-private.h"
#include "retro-debug-private.h"
#include "retro-input-private.h"
#include "retro-gl-renderer-private.h"
#include "retro-hw-render-callback-private.h"
#include "retro-rumble-effect.h"

#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "RetroEnvironment"

#define TRUENESS(boolean) ((boolean) ? "true" : "false")

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

#define retro_sanitize_string(pp)                        \
  G_STMT_START                                           \
  {                                                      \
    G_STATIC_ASSERT (sizeof *(pp) == sizeof (gpointer)); \
    glib_typeof ((pp)) _pp = (pp);                       \
    glib_typeof (*(pp)) _ptr = *_pp;                     \
    *_pp = NULL;                                         \
    if (g_strcmp0 (_ptr, "") == 0)                       \
      *(_pp) = NULL;                                     \
  }                                                      \
  G_STMT_END

static gboolean
rumble_callback_set_rumble_state (guint             port,
                                  RetroRumbleEffect effect,
                                  guint16           strength)
{
  RetroCore *self = retro_core_get_instance ();

  g_return_val_if_fail (self, FALSE);

  if (!retro_core_get_controller_supports_rumble (self, port))
    return FALSE;

  g_signal_emit_by_name (self, "set-rumble-state", port, effect, strength);

  return TRUE;
}

static void
log_cb (guint level, const gchar *format, ...)
{
  RetroCore *self = retro_core_get_instance ();
  const gchar *log_domain;
  GLogLevelFlags log_level;
  g_autofree gchar *message = NULL;
  va_list args;

  g_return_if_fail (self);

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
    retro_debug ("Unexpected log level: %d", level);

    return;
  }

  // Get the arguments, set up the formatted message, pass it to the logging
  // method and free it.
  va_start (args, format);
  message = g_strdup_vprintf (format, args);

  log_domain = retro_core_get_name (self);
  g_signal_emit_by_name (self, "log", log_domain, log_level, message);
}

static const gchar *
get_input_string (RetroInputDescriptor *descriptor) {
  const gchar *controller_type_name = NULL;
  g_autoptr (GEnumClass) index_enum_class = NULL;
  g_autoptr (GEnumClass) id_enum_class = NULL;
  GEnumValue *id_enum_value;

  g_return_val_if_fail (descriptor, NULL);

  switch (descriptor->controller_type & RETRO_CONTROLLER_TYPE_TYPE_MASK) {
  case RETRO_CONTROLLER_TYPE_JOYPAD:
    controller_type_name = "joypad";
    id_enum_class = g_type_class_ref (RETRO_TYPE_JOYPAD_ID);
    break;
  case RETRO_CONTROLLER_TYPE_MOUSE:
    controller_type_name = "mouse";
    id_enum_class = g_type_class_ref (RETRO_TYPE_MOUSE_ID);
    break;
  case RETRO_CONTROLLER_TYPE_LIGHTGUN:
    controller_type_name = "lightgun";
    id_enum_class = g_type_class_ref (RETRO_TYPE_LIGHTGUN_ID);
    break;
  case RETRO_CONTROLLER_TYPE_ANALOG:
    controller_type_name = "analog";
    index_enum_class = g_type_class_ref (RETRO_TYPE_ANALOG_INDEX);
    id_enum_class = g_type_class_ref (RETRO_TYPE_ANALOG_ID);
    break;
  case RETRO_CONTROLLER_TYPE_POINTER:
    controller_type_name = "pointer";
    id_enum_class = g_type_class_ref (RETRO_TYPE_POINTER_ID);
    break;
  case RETRO_CONTROLLER_TYPE_KEYBOARD:
    return g_strdup ("keyboard key");
  case RETRO_CONTROLLER_TYPE_NONE:
    return g_strdup ("none");
  default:
    return g_strdup ("unknown input");
  }

  id_enum_value = g_enum_get_value (id_enum_class, descriptor->id);

  if (index_enum_class) {
    GEnumValue *index_enum_value = g_enum_get_value (index_enum_class, descriptor->index);

    return (index_enum_value && id_enum_value) ?
      g_strdup_printf ("%s %s %s", controller_type_name, index_enum_value->value_nick, id_enum_value->value_nick) :
      g_strdup_printf ("unknown %s input", controller_type_name);
  }

  return (id_enum_value) ?
    g_strdup_printf ("%s %s", controller_type_name, id_enum_value->value_nick) :
    g_strdup_printf ("unknown %s input", controller_type_name);
}

/* Environment commands */

static gboolean
get_can_dupe (RetroCore *self,
              bool      *can_dupe)
{
  g_assert (self);
  g_return_val_if_fail (can_dupe, FALSE);

  *can_dupe = TRUE;

  retro_debug ("Get can dupe: true");

  return TRUE;
}

static gboolean
get_core_assets_directory (RetroCore    *self,
                           const gchar **core_assets_directory)
{
  g_assert (self);
  g_return_val_if_fail (core_assets_directory, FALSE);

  *(core_assets_directory) = retro_core_get_core_assets_directory (self);
  retro_sanitize_string (core_assets_directory);

  retro_debug ("Get core assets directory: %s", *core_assets_directory);

  return TRUE;
}

static gboolean
get_input_device_capabilities (RetroCore *self,
                               guint64   *capabilities)
{
  g_assert (self);
  g_return_val_if_fail (capabilities, FALSE);

  *capabilities = retro_core_get_controller_capabilities (self);

  retro_debug ("Get input device capabilities");

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
    { "C", RETRO_LANGUAGE_ENGLISH },
  };

  const gchar * const *locales = g_get_language_names ();
  gsize language_i = 0;

  g_assert (self);
  g_return_val_if_fail (language, FALSE);

  for (gsize locale_i = 0; locales[locale_i] != NULL; locale_i++) {
    for (language_i = 0;
         !g_str_equal (values[language_i].locale, "C") &&
         !g_str_equal (locales[locale_i], values[language_i].locale);
         language_i++);
    if (!g_str_equal (values[language_i].locale, "C"))
      break;
  }

  *language = values[language_i].language;

  retro_debug ("Get language: %s", values[language_i].locale);

  return TRUE;
}

static gboolean
get_libretro_path (RetroCore    *self,
                   const gchar **libretro_directory)
{
  g_assert (self);
  g_return_val_if_fail (libretro_directory, FALSE);

  *(libretro_directory) = retro_core_get_libretro_path (self);
  retro_sanitize_string (libretro_directory);

  retro_debug ("Get libretro directory: %s", *libretro_directory);

  return TRUE;
}

static gboolean
get_log_callback (RetroCore        *self,
                  RetroLogCallback *cb)
{
  g_assert (self);
  g_return_val_if_fail (cb, FALSE);

  cb->log = log_cb;

  retro_debug ("Get log callback");

  return TRUE;
}

static gboolean
get_overscan (RetroCore *self,
              bool      *overscan)
{
  g_assert (self);
  g_return_val_if_fail (overscan, FALSE);

  *overscan = !!self->overscan;

  retro_debug ("Get overscan: %s", TRUENESS (*overscan));

  return TRUE;
}

static gboolean
get_rumble_callback (RetroCore           *self,
                     RetroRumbleCallback *cb)
{
  g_assert (self);
  g_return_val_if_fail (cb, FALSE);

  cb->set_rumble_state = rumble_callback_set_rumble_state;

  retro_debug ("Get rumble callback");

  return TRUE;
}

static gboolean
get_save_directory (RetroCore    *self,
                    const gchar **save_directory)
{
  g_assert (self);
  g_return_val_if_fail (save_directory, FALSE);

  *(save_directory) = retro_core_get_save_directory (self);
  retro_sanitize_string (save_directory);

  retro_debug ("Get save directory: %s", *save_directory);

  return TRUE;
}

static gboolean
get_system_directory (RetroCore    *self,
                      const gchar **system_directory)
{
  g_assert (self);
  g_return_val_if_fail (system_directory, FALSE);

  *(system_directory) = retro_core_get_system_directory (self);
  retro_sanitize_string (system_directory);

  retro_debug ("Get system directory: %s", *system_directory);

  return TRUE;
}

static gboolean
get_username (RetroCore    *self,
              const gchar **username)
{
  g_assert (self);
  g_return_val_if_fail (username, FALSE);

  *(username) = retro_core_get_user_name (self);
  retro_sanitize_string (username);

  retro_debug ("Get username: %s", *username);

  return TRUE;
}

static gboolean
get_variable (RetroCore     *self,
              RetroVariable *variable)
{
  gchar *value;

  g_assert (self);
  g_return_val_if_fail (variable, FALSE);

  value = g_hash_table_lookup (self->variables, variable->key);

  if (G_UNLIKELY (!value)) {
    g_critical ("Couldn't get variable %s", variable->key);

    return FALSE;
  }

  variable->value = value;

  retro_debug ("Get variable %s: %s", variable->key, variable->value);

  return TRUE;
}

// The data must be bool, not gboolean, the sizes can be different.
static gboolean
get_variable_update (RetroCore *self,
                     bool      *update)
{
  g_assert (self);
  g_return_val_if_fail (update, FALSE);

  *update = !!retro_core_get_variable_update (self);

  /* We purposefully don't log when no variable update is detected because it's
   * not very useful to debug and it would print on every iteration, causing
   * slowdowns.
   */
  if (*update)
    retro_debug ("Get variable update: updated");

  return TRUE;
}

static RetroProcAddress
hw_rendering_callback_get_proc_address (const gchar *sym)
{
  RetroCore *self = retro_core_get_instance ();

  g_assert (self);
  g_return_val_if_fail (sym, FALSE);

  return retro_renderer_get_proc_address (self->renderer, sym);
}

static guintptr
hw_rendering_callback_get_current_framebuffer ()
{
  RetroCore *self = retro_core_get_instance ();

  g_assert (self);

  return retro_renderer_get_current_framebuffer (self->renderer);
}

static gboolean
set_hw_render (RetroCore             *self,
               RetroHWRenderCallback *callback)
{
  g_assert (self);
  g_return_val_if_fail (callback, FALSE);

  g_clear_object (&self->renderer);

  switch (callback->context_type) {
  case RETRO_HW_CONTEXT_OPENGL:
  case RETRO_HW_CONTEXT_OPENGL_CORE:
  case RETRO_HW_CONTEXT_OPENGLES2:
  case RETRO_HW_CONTEXT_OPENGLES3:
  case RETRO_HW_CONTEXT_OPENGLES_VERSION:
    retro_debug ("Set hardware render callback: OpenGL");

    self->renderer = retro_gl_renderer_new (self, callback);
    break;

  case RETRO_HW_CONTEXT_VULKAN:
    g_critical ("Couldn't set hardware render callback: Vulkan support is unimplemented");

    return FALSE;
  case RETRO_HW_CONTEXT_DIRECT3D:
    g_critical ("Couldn't set hardware render callback: Direct3D is unsupported");

    return FALSE;
  default:
    g_critical ("Couldn't set hardware render callback for unknown context type %d", callback->context_type);

    return FALSE;
  }

  callback->get_current_framebuffer = hw_rendering_callback_get_current_framebuffer;
  callback->get_proc_address = hw_rendering_callback_get_proc_address;

  return TRUE;
}

static gboolean
set_disk_control_interface (RetroCore                *self,
                            RetroDiskControlCallback *callback)
{
  g_assert (self);
  g_return_val_if_fail (callback, FALSE);

  retro_debug ("Set disk control callback");

  self->disk_control_callback = callback;

  return TRUE;
}

static gboolean
set_geometry (RetroCore         *self,
              RetroGameGeometry *geometry)
{
  g_assert (self);
  g_return_val_if_fail (geometry, FALSE);

  retro_debug ("Set geometry: base %u × %u, max %u × %u, aspect ratio %f",
               geometry->base_width, geometry->base_height,
               geometry->max_width, geometry->max_height,
               geometry->aspect_ratio);

  retro_core_set_geometry (self, geometry);

  return TRUE;
}

static gboolean
set_input_descriptors (RetroCore            *self,
                       RetroInputDescriptor *descriptors)
{
  int length;

  g_assert (self);
  g_return_val_if_fail (descriptors, FALSE);

  for (length = 0 ; descriptors[length].description ; length++)
    retro_debug ("Set input descriptor: port %u, type %u, index %u, id %u (%s%s): %s",
                 descriptors[length].port,
                 descriptors[length].controller_type,
                 descriptors[length].index,
                 descriptors[length].id,
                 get_input_string (&descriptors[length]),
                 (descriptors[length].controller_type & ~RETRO_CONTROLLER_TYPE_TYPE_MASK) ? ", specialized" : "",
                 descriptors[length].description);

  retro_core_set_controller_descriptors (self, descriptors, length);

  return TRUE;
}

static gboolean
set_keyboard_callback (RetroCore             *self,
                       RetroKeyboardCallback *callback)
{
  g_assert (self);
  g_return_val_if_fail (callback, FALSE);

  retro_debug ("Set keyboard callback");

  self->keyboard_callback = *callback;

  return TRUE;
}

static gboolean
set_message (RetroCore          *self,
             const RetroMessage *message)
{
  g_assert (self);
  g_return_val_if_fail (message, FALSE);

  retro_debug ("Emit message for %u frames: %s", message->frames, message->msg);

  g_signal_emit_by_name (self, "message", message->msg, message->frames);

  return TRUE;
}

static gboolean
set_pixel_format (RetroCore              *self,
                  const RetroPixelFormat *pixel_format)
{
  g_assert (self);
  g_return_val_if_fail (pixel_format, FALSE);

  switch (*pixel_format) {
  case RETRO_PIXEL_FORMAT_XRGB1555:
    retro_debug ("Set pixel format: XRGB1555");

    break;
  case RETRO_PIXEL_FORMAT_XRGB8888:
    retro_debug ("Set pixel format: XRGB8888");

    break;
  case RETRO_PIXEL_FORMAT_RGB565:
    retro_debug ("Set pixel format: RGB565");

    break;
  default:
    g_critical ("Couldn't set unknown pixel format %d", *pixel_format);

    return FALSE;
  }

  self->pixel_format = *pixel_format;

  return TRUE;
}

static gboolean
set_rotation (RetroCore           *self,
              const RetroRotation *rotation)
{
  g_assert (self);
  g_return_val_if_fail (rotation, FALSE);

  if (G_UNLIKELY (*rotation >= CLOCKWISE)) {
    g_critical ("Couldn't set unknown rotation %d", *rotation);

    return FALSE;
  }

  retro_debug ("Set rotation: %d°", *rotation * 90);

  self->rotation = *rotation;

  return TRUE;
}

static gboolean
set_support_no_game (RetroCore  *self,
                     const bool *support_no_game)
{
  g_assert (self);
  g_return_val_if_fail (support_no_game, FALSE);

  retro_debug ("Set support no game: %s", TRUENESS (*support_no_game));

  retro_core_set_support_no_game (self, *support_no_game);

  return TRUE;
}

static gboolean
set_system_av_info (RetroCore         *self,
                    RetroSystemAvInfo *system_av_info)
{
  g_assert (self);
  g_return_val_if_fail (system_av_info, FALSE);

  retro_debug ("Set system AV info: base %u × %u, max %u × %u, aspect ratio %f, %lf FPS, %lf Hz",
               system_av_info->geometry.base_width, system_av_info->geometry.base_height,
               system_av_info->geometry.max_width, system_av_info->geometry.max_height,
               system_av_info->geometry.aspect_ratio,
               system_av_info->timing.fps, system_av_info->timing.sample_rate);

  retro_core_set_system_av_info (self, system_av_info);

  return TRUE;
}

static gboolean
set_variables (RetroCore     *self,
               RetroVariable *variable_array)
{
  g_assert (self);
  g_return_val_if_fail (variable_array, FALSE);

  for (gsize i = 0 ; variable_array[i].key && variable_array[i].value ; i++) {
    retro_debug ("Set variable %s: %s", variable_array[i].key, variable_array[i].value);

    retro_core_insert_variable (self, &variable_array[i]);
  }

  g_signal_emit_by_name (self, "variables-set", variable_array);

  return TRUE;
}

static gboolean
shutdown (RetroCore *self)
{
  g_assert (self);

  retro_debug ("Emit shutdown");

  g_signal_emit_by_name (self, "shutdown");

  return TRUE;
}

#define RETRO_UNIMPLEMENT_ENVIRONMENT(cmd) \
  case cmd: \
    G_STMT_START { \
      if (cmd & RETRO_ENVIRONMENT_EXPERIMENTAL) \
        g_critical ("Unimplemented experimental command %s", #cmd); \
      else \
        g_critical ("Unimplemented command %s", #cmd); \
      return FALSE; \
    } G_STMT_END

static gboolean
environment_core_command (RetroCore *self,
                          unsigned   cmd,
                          gpointer   data)
{
  if (!self)
    return FALSE;

  switch (cmd) {
  case RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY:
    return get_core_assets_directory (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_CAN_DUPE:
    return get_can_dupe (self, (bool *) data);

  case RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES:
    return get_input_device_capabilities (self, (guint64 *) data);

  case RETRO_ENVIRONMENT_GET_LANGUAGE:
    return get_language (self, (unsigned *) data);

  case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
    return get_libretro_path (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
    return get_log_callback (self, (RetroLogCallback *) data);

  case RETRO_ENVIRONMENT_GET_OVERSCAN:
    return get_overscan (self, (bool *) data);

  case RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE:
    return get_rumble_callback (self, (RetroRumbleCallback *) data);

  case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
    return get_save_directory (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
    return get_system_directory (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_USERNAME:
    return get_username (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_VARIABLE:
    return get_variable (self, (RetroVariable *) data);

  case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
    return get_variable_update (self, (bool *) data);

  case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
    return set_disk_control_interface (self, (RetroDiskControlCallback *) data);

  case RETRO_ENVIRONMENT_SET_GEOMETRY:
    return set_geometry (self, (RetroGameGeometry *) data);

  case RETRO_ENVIRONMENT_SET_HW_RENDER:
    return set_hw_render (self, (RetroHWRenderCallback *) data);

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
    return set_support_no_game (self, (const bool *) data);

  case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
    return set_system_av_info (self, (RetroSystemAvInfo *) data);

  case RETRO_ENVIRONMENT_SET_VARIABLES:
    return set_variables (self, (RetroVariable *) data);

  case RETRO_ENVIRONMENT_SHUTDOWN:
    return shutdown (self);

  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_GET_CAMERA_INTERFACE);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_GET_PERF_INTERFACE);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_SET_CONTROLLER_INFO);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_SET_HW_RENDER_CONTEXT_NEGOTIATION_INTERFACE);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_SET_MEMORY_MAPS);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_SET_SERIALIZATION_QUIRKS);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO);
  RETRO_UNIMPLEMENT_ENVIRONMENT (RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS);

  default:
    if (cmd & RETRO_ENVIRONMENT_EXPERIMENTAL)
      g_critical ("Unknown experimental command %d", cmd ^ RETRO_ENVIRONMENT_EXPERIMENTAL);
    else
      g_critical ("Unknown command %d", cmd);

    return FALSE;
  }
}

/* Core callbacks */

static gboolean
environment_interface_cb (unsigned cmd,
                          gpointer data)
{
  RetroCore *self = retro_core_get_instance ();

  return environment_core_command (self, cmd, data);
}

static void
video_refresh_cb (guint8 *data,
                  guint   width,
                  guint   height,
                  gsize   pitch)
{
  RetroCore *self = retro_core_get_instance ();

  if (data == NULL)
    return;

  if (retro_core_is_running_ahead (self))
    return;

  retro_framebuffer_lock (self->framebuffer);

  if (self->renderer) {
    gint pixel_size;

    if (G_UNLIKELY (data && data != RETRO_HW_FRAME_BUFFER_VALID)) {
      g_critical ("Video data must be NULL or RETRO_HW_FRAME_BUFFER_VALID if "
                  "rendering to hardware.");

      return;
    }

    if (!retro_pixel_format_to_gl (self->pixel_format, NULL, NULL, &pixel_size))
      return;

    pitch = width * pixel_size;

    retro_framebuffer_set_data (self->framebuffer, self->pixel_format, pitch,
                                width, height, self->aspect_ratio, NULL);

    data = retro_framebuffer_get_pixels (self->framebuffer);

    retro_renderer_snapshot (self->renderer, self->pixel_format, width, height, pitch, data);
  }
  else
    retro_framebuffer_set_data (self->framebuffer, self->pixel_format, pitch,
                                width, height, self->aspect_ratio, data);

  retro_framebuffer_unlock (self->framebuffer);

  if (!self->block_video_signal)
    g_signal_emit_by_name (self, "video-output");
}

static void
audio_sample_cb (gint16 left,
                 gint16 right)
{
  RetroCore *self = retro_core_get_instance ();
  gint16 samples[] = { left, right };

  if (retro_core_is_running_ahead (self))
    return;

  if (self->sample_rate <= 0.0)
    return;

  g_signal_emit_by_name (self, "audio_output", samples, 2, self->sample_rate);
}

static gsize
audio_sample_batch_cb (gint16 *data,
                       gint    frames)
{
  RetroCore *self = retro_core_get_instance ();

  if (retro_core_is_running_ahead (self))
    return frames;

  if (self->sample_rate <= 0.0)
    return 0;

  g_signal_emit_by_name (self, "audio_output", data, frames * 2, self->sample_rate);

  return frames;
}

static void
input_poll_cb (void)
{
  RetroCore *self = retro_core_get_instance ();

  retro_core_poll_controllers (self);
}

static gint16
input_state_cb (guint port,
                guint device,
                guint index,
                guint id)
{
  RetroCore *self = retro_core_get_instance ();
  RetroInput input;

  retro_input_init (&input, device, id, index);

  return retro_core_get_controller_input_state (self, port, &input);
}

void
retro_core_set_environment_interface (RetroCore *self)
{
  RetroModule *module;
  RetroCallbackSetter set_environment;

  module = self->module;
  set_environment = retro_module_get_set_environment (module);
  set_environment (environment_interface_cb);
}

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

  set_video_refresh (video_refresh_cb);
  set_audio_sample (audio_sample_cb);
  set_audio_sample_batch (audio_sample_batch_cb);
  set_input_poll (input_poll_cb);
  set_input_state (input_state_cb);
}
