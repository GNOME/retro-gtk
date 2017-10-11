// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-core-private.h"

#include <string.h>
#include "retro-controller-iterator-private.h"
#include "retro-keyboard-key.h"

#define RETRO_CORE_ERROR (retro_core_error_quark ())

enum {
  RETRO_CORE_ERROR_COULDNT_SERIALIZE,
  RETRO_CORE_ERROR_COULDNT_DESERIALIZE,
  RETRO_CORE_ERROR_SERIALIZATION_NOT_SUPPORTED,
  RETRO_CORE_ERROR_NO_CALLBACK,
};

G_DEFINE_TYPE (RetroCore, retro_core, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_API_VERSION,
  PROP_FILENAME,
  PROP_SYSTEM_DIRECTORY,
  PROP_CONTENT_DIRECTORY,
  PROP_SAVE_DIRECTORY,
  PROP_IS_INITIATED,
  PROP_GAME_LOADED,
  PROP_SUPPORT_NO_GAME,
  PROP_FRAMES_PER_SECOND,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

enum {
  SIG_VIDEO_OUTPUT_SIGNAL,
  SIG_AUDIO_OUTPUT_SIGNAL,
  SIG_LOG_SIGNAL,
  SIG_SHUTDOWN_SIGNAL,
  SIG_MESSAGE_SIGNAL,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

#define RETRO_CORE_OBJECTS_LENGTH 32

static GRecMutex retro_core_r_mutex = { 0 };
static GRecMutex retro_core_w_mutex = { 0 };
static RetroCore *retro_core_objects[32];
static gint retro_core_i = 0;

static void retro_core_set_filename (RetroCore   *self,
                                     const gchar *filename);
static void retro_core_set_is_initiated (RetroCore *self,
                                         gboolean   is_initiated);
static void retro_core_set_game_loaded (RetroCore *self,
                                        gboolean   game_loaded);

/* Private */

static GQuark
retro_core_error_quark (void)
{
  return g_quark_from_static_string ("retro-core-error-quark");
}

void
retro_core_push_cb_data (RetroCore *self)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  g_rec_mutex_lock (&retro_core_w_mutex);
  g_rec_mutex_lock (&retro_core_r_mutex);

  if (G_UNLIKELY (retro_core_i == RETRO_CORE_OBJECTS_LENGTH)) {
    g_critical ("RetroCore callback data stack overflow.");

    g_rec_mutex_unlock (&retro_core_r_mutex);
    g_assert_not_reached ();
  }

  retro_core_objects[retro_core_i] = self;
  retro_core_i++;

  g_rec_mutex_unlock (&retro_core_r_mutex);
}

void
retro_core_pop_cb_data (void)
{
  g_rec_mutex_lock (&retro_core_r_mutex);

  if (G_UNLIKELY (retro_core_i == 0)) {
    g_critical ("RetroCore callback data stack underflow.");

    g_rec_mutex_unlock (&retro_core_r_mutex);
    g_rec_mutex_unlock (&retro_core_w_mutex);
    g_assert_not_reached ();
  }
  retro_core_i--;

  retro_core_objects[retro_core_i] = NULL;

  g_rec_mutex_unlock (&retro_core_r_mutex);
  g_rec_mutex_unlock (&retro_core_w_mutex);
}

RetroCore *
retro_core_get_cb_data (void)
{
  RetroCore *result;

  g_rec_mutex_lock (&retro_core_r_mutex);

  if (retro_core_i == 0) {
    g_critical ("RetroCore callback data segmentation fault.");

    g_rec_mutex_unlock (&retro_core_r_mutex);
    g_assert_not_reached ();
  }

  result = retro_core_objects[retro_core_i - 1];
  g_rec_mutex_unlock (&retro_core_r_mutex);

  return result;
}

static void
retro_core_finalize (GObject *object)
{
  RetroCore *self = RETRO_CORE (object);
  RetroUnloadGame unload_game;
  RetroDeinit deinit;

  g_return_if_fail (RETRO_IS_CORE (self));

  retro_core_push_cb_data (self);
  if (retro_core_get_game_loaded (self)) {
    unload_game = retro_module_get_unload_game (self->module);
    unload_game ();
  }
  deinit = retro_module_get_deinit (self->module);
  deinit ();
  retro_core_pop_cb_data ();

  if (self->media_uris != NULL)
    g_strfreev (self->media_uris);

  g_object_unref (self->module);
  g_hash_table_unref (self->controllers);
  g_object_unref (self->options);

  g_free (self->filename);
  g_free (self->system_directory);
  g_free (self->libretro_path);
  g_free (self->content_directory);
  g_free (self->save_directory);
  g_object_unref (self->keyboard_widget);

  G_OBJECT_CLASS (retro_core_parent_class)->finalize (object);
}

static void
retro_core_get_property (GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  RetroCore *self = RETRO_CORE (object);

  switch (prop_id) {
  case PROP_API_VERSION:
    g_value_set_uint (value, retro_core_get_api_version (self));

    break;
  case PROP_SYSTEM_DIRECTORY:
    g_value_set_string (value, retro_core_get_system_directory (self));

    break;
  case PROP_CONTENT_DIRECTORY:
    g_value_set_string (value, retro_core_get_content_directory (self));

    break;
  case PROP_SAVE_DIRECTORY:
    g_value_set_string (value, retro_core_get_save_directory (self));

    break;
  case PROP_FRAMES_PER_SECOND:
    g_value_set_double (value, retro_core_get_frames_per_second (self));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_core_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  RetroCore *self = RETRO_CORE (object);

  switch (prop_id) {
  case PROP_FILENAME:
    retro_core_set_filename (self, g_value_get_string (value));

    break;
  case PROP_SYSTEM_DIRECTORY:
    retro_core_set_system_directory (self, g_value_get_string (value));

    break;
  case PROP_CONTENT_DIRECTORY:
    retro_core_set_content_directory (self, g_value_get_string (value));

    break;
  case PROP_SAVE_DIRECTORY:
    retro_core_set_save_directory (self, g_value_get_string (value));

    break;
  case PROP_IS_INITIATED:
    retro_core_set_is_initiated (self, g_value_get_boolean (value));

    break;
  case PROP_GAME_LOADED:
    retro_core_set_game_loaded (self, g_value_get_boolean (value));

    break;
  case PROP_SUPPORT_NO_GAME:
    retro_core_set_support_no_game (self, g_value_get_boolean (value));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_core_class_init (RetroCoreClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_core_finalize;
  object_class->get_property = retro_core_get_property;
  object_class->set_property = retro_core_set_property;

  /**
   * RetroCore:api-version:
   *
   * The Libretro API version implement by the core.
   */
  properties[PROP_API_VERSION] =
    g_param_spec_uint ("api-version",
                       "API version",
                       "The API version",
                       0,
                       G_MAXUINT,
                       0U,
                       G_PARAM_READABLE |
                       G_PARAM_STATIC_NAME |
                       G_PARAM_STATIC_NICK |
                       G_PARAM_STATIC_BLURB);

  /**
   * RetroCore:filename:
   *
   * The filename of the core.
   */
  properties[PROP_FILENAME] =
    g_param_spec_string ("filename",
                         "Filename",
                         "The module filename",
                         NULL,
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB);

  /**
   * RetroCore:system-directory:
   *
   * The system directory of the core.
   *
   * The core will look here for additional data, such as firmware ROMs or
   * configuration files.
   */
  properties[PROP_SYSTEM_DIRECTORY] =
    g_param_spec_string ("system-directory",
                         "System directory",
                         "The system directory",
                         NULL,
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB);

  // FIXME This should be removed as it is deprecated by Libretro.
  properties[PROP_CONTENT_DIRECTORY] =
    g_param_spec_string ("content-directory",
                         "Content directory",
                         "The content directory",
                         NULL,
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB);

  /**
   * RetroCore:save-directory:
   *
   * The save directory of the core.
   *
   * The core will save some data here.
   */
  properties[PROP_SAVE_DIRECTORY] =
    g_param_spec_string ("save-directory",
                         "Save directory",
                         "The save directory",
                         NULL,
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB);

  /**
   * RetroCore:is-initiated:
   *
   * Whether the core has been initiated.
   */
  properties[PROP_IS_INITIATED] =
    g_param_spec_boolean ("is-initiated",
                          "Is initiated",
                          "Whether the core has been initiated",
                          FALSE,
                          G_PARAM_READABLE |
                          G_PARAM_STATIC_NAME |
                          G_PARAM_STATIC_NICK |
                          G_PARAM_STATIC_BLURB);

  /**
   * RetroCore:game-loaded:
   *
   * Whether a game has been loaded.
   */
  properties[PROP_GAME_LOADED] =
    g_param_spec_boolean ("game-loaded",
                          "Game loaded",
                          "Whether a game has been loaded",
                          FALSE,
                          G_PARAM_READABLE |
                          G_PARAM_STATIC_NAME |
                          G_PARAM_STATIC_NICK |
                          G_PARAM_STATIC_BLURB);

  /**
   * RetroCore:support-no-game:
   *
   * Whether the core supports running with no game.
   */
  properties[PROP_SUPPORT_NO_GAME] =
    g_param_spec_boolean ("support-no-game",
                          "Support no game",
                          "Whether the core supports running with no game",
                          FALSE,
                          G_PARAM_READABLE |
                          G_PARAM_STATIC_NAME |
                          G_PARAM_STATIC_NICK |
                          G_PARAM_STATIC_BLURB);

  /**
   * RetroCore:frames-per-second:
   *
   * The FPS rate for the core's video output.
   */
  properties[PROP_FRAMES_PER_SECOND] =
    g_param_spec_double ("frames-per-second",
                         "Frames per second",
                         "The FPS rate for the core's video output",
                         -G_MAXDOUBLE,
                         G_MAXDOUBLE,
                         0.0,
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB);

  g_object_class_install_properties (G_OBJECT_CLASS (klass), N_PROPS, properties);

  /**
   * RetroCore::video-output:
   * @self: the #RetroCore
   * @data: (array length=length) (element-type guint8): the video frame data
   * @length: the lentgh of @data
   * @width: the width of the video frame
   * @height: the height of the video frame
   * @pitch: the distance in bytes between rows
   * @pixel_format: the pixel format
   * @aspect_ratio: the aspect ratio to render the frame
   *
   * The ::video-output signal is emitted each time a new video frame is emitted
   * by the core.
   */
  signals[SIG_VIDEO_OUTPUT_SIGNAL] =
    g_signal_new ("video-output", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  7,
                  G_TYPE_POINTER,
                  G_TYPE_ULONG,
                  G_TYPE_UINT,
                  G_TYPE_UINT,
                  G_TYPE_ULONG,
                  RETRO_TYPE_PIXEL_FORMAT,
                  G_TYPE_FLOAT);

  /**
   * RetroCore::audio-output:
   * @self: the #RetroCore
   * @data: (array length=length) (element-type guint8): the audio frame data
   * @length: the lentgh of @data
   * @sample_rate: the sample rate to play the frame
   *
   * The ::audio-output signal is emitted each time a new audio frame is emitted
   * by the core.
   */
  signals[SIG_AUDIO_OUTPUT_SIGNAL] =
    g_signal_new ("audio-output", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  3,
                  G_TYPE_POINTER,
                  G_TYPE_ULONG,
                  G_TYPE_DOUBLE);

  /**
   * RetroCore::log:
   * @self: the #RetroCore
   * @log_domain: the log domain
   * @log_level: (type GLogLevelFlags): the log level
   * @message: the message
   *
   * The ::log signal is emitted each time the core emits a message to log.
   */
  signals[SIG_LOG_SIGNAL] =
    g_signal_new ("log", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  3,
                  G_TYPE_STRING,
                  G_TYPE_UINT,
                  G_TYPE_STRING);

  /**
   * RetroCore::shutdown:
   * @self: the #RetroCore
   *
   * The ::shutdown signal is emitted when the core shut down.
   *
   * The core must be released or re-started in order to function anew.
   *
   * Returns: whether the request got fulfilled
   */
  signals[SIG_SHUTDOWN_SIGNAL] =
    g_signal_new ("shutdown", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_BOOLEAN,
                  0);

  /**
   * RetroCore::message:
   * @self: the #RetroCore
   * @message: the message
   * @frames: the number of frames the message should be displayed
   *
   * The ::message signal is emitted each time the core emits a message to
   * display during a given amount of frames.
   *
   * Returns: whether the request got fulfilled
   */
  signals[SIG_MESSAGE_SIGNAL] =
    g_signal_new ("message", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_BOOLEAN,
                  2,
                  G_TYPE_STRING,
                  G_TYPE_UINT);
}

static void
retro_core_init (RetroCore *self)
{
}

static void
retro_core_set_filename (RetroCore   *self,
                         const gchar *filename)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (g_strcmp0 (filename, retro_core_get_filename (self)) == 0)
    return;

  g_free (self->filename);
  self->filename = g_strdup (filename);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_FILENAME]);
}

static void
retro_core_set_is_initiated (RetroCore *self,
                             gboolean   is_initiated)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (retro_core_get_is_initiated (self) == is_initiated)
    return;

  self->is_initiated = is_initiated;
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_IS_INITIATED]);
}

static void
retro_core_set_game_loaded (RetroCore *self,
                            gboolean   game_loaded)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (retro_core_get_game_loaded (self) == game_loaded)
    return;

  self->game_loaded = game_loaded;
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_GAME_LOADED]);
}

/**
 * retro_core_get_system_info:
 * @self: A #RetroCore
 * @system_info: (out) (transfer none): A location for a #RetroSystemInfo
 *
 * Gives basic informations on how to handle the Libretro core.
 */
static void
retro_core_get_system_info (RetroCore       *self,
                            RetroSystemInfo *system_info)
{
  RetroGetSystemInfo get_system_info;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (system_info != NULL);

  retro_core_push_cb_data (self);
  get_system_info = retro_module_get_get_system_info (self->module);
  get_system_info (system_info);
  retro_core_pop_cb_data ();
}

static gboolean
retro_core_get_needs_full_path (RetroCore *self)
{
  RetroSystemInfo system_info = { 0 };

  retro_core_get_system_info (self, &system_info);

  return system_info.need_fullpath;
}

/**
 * retro_core_get_name:
 * @self: a #RetroCore
 *
 * Gets the name of @self.
 *
 * Returns: (transfer none): the name of @self
 */
gchar *
retro_core_get_name (RetroCore *self)
{
  RetroSystemInfo system_info = { 0 };

  retro_core_get_system_info (self, &system_info);

  return system_info.library_name;
}

static void
retro_core_controller_connected (RetroCore       *self,
                                 guint            port,
                                 RetroController *controller)
{
  RetroControllerType controller_type;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (controller != NULL);

  if (!retro_core_get_is_initiated (self))
    return;

  controller_type = retro_controller_get_controller_type (controller);
  retro_core_set_controller_port_device (self, port, controller_type);
}

static void
retro_core_controller_disconnected (RetroCore *self,
                                    guint      port)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (!retro_core_get_is_initiated (self))
    return;

  retro_core_set_controller_port_device (self, port, RETRO_CONTROLLER_TYPE_NONE);
}

static void
retro_core_send_input_key_event (RetroCore                *self,
                                 gboolean                  down,
                                 RetroKeyboardKey          keycode,
                                 guint32                   character,
                                 RetroKeyboardModifierKey  key_modifiers)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (self->keyboard_callback.callback == NULL)
    return;

  self->keyboard_callback.callback (down, keycode, character, key_modifiers);
}

static gboolean
retro_core_key_event (RetroCore   *self,
                      GdkEventKey *event)
{
  gboolean pressed;
  RetroKeyboardKey retro_key;
  RetroKeyboardModifierKey retro_modifier_key;
  guint32 character;

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (!retro_core_get_is_initiated (self))
    return FALSE;

  pressed = event->type == GDK_KEY_PRESS;
  retro_key = retro_keyboard_key_converter (event->keyval);
  retro_modifier_key = retro_keyboard_modifier_key_converter (event->keyval, event->state);
  character = gdk_keyval_to_unicode (event->keyval);

  retro_core_push_cb_data (self);
  retro_core_send_input_key_event (self,
                                   pressed,
                                   retro_key,
                                   character,
                                   retro_modifier_key);
  retro_core_pop_cb_data ();

  return FALSE;
}

static gboolean
retro_core_set_disk_ejected (RetroCore  *self,
                             gboolean    ejected,
                             GError    **error)
{
  RetroDiskControlCallbackSetEjectState set_eject_state;
  gboolean result;

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  set_eject_state = self->disk_control_callback->set_eject_state;

  if (set_eject_state == NULL) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_NO_CALLBACK,
                         "DiskControl has no callback for this operation.");

    return FALSE;
  }

  retro_core_push_cb_data (self);
  result = set_eject_state (ejected);
  retro_core_pop_cb_data ();

  return result;
}

static gboolean
retro_core_set_disk_image_index (RetroCore  *self,
                                 guint       index,
                                 GError    **error)
{
  RetroDiskControlCallbackSetImageIndex set_image_index;
  gboolean result;

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  set_image_index = self->disk_control_callback->set_image_index;

  if (set_image_index == NULL) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_NO_CALLBACK,
                         "DiskControl has no callback for this operation.");

    return FALSE;
  }

  retro_core_push_cb_data (self);
  result = set_image_index (index);
  retro_core_pop_cb_data ();

  return result;
}

static guint
retro_core_get_disk_images_number (RetroCore  *self,
                                   GError    **error)
{
  RetroDiskControlCallbackGetNumImages get_num_images;
  guint result;

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  get_num_images = self->disk_control_callback->get_num_images;

  if (get_num_images == NULL) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_NO_CALLBACK,
                         "DiskControl has no callback for this operation.");

    return FALSE;
  }

  retro_core_push_cb_data (self);
  result = get_num_images ();
  retro_core_pop_cb_data ();

  return result;
}

static gboolean
retro_core_replace_disk_image_index (RetroCore     *self,
                                     guint          index,
                                     RetroGameInfo *info,
                                     GError        **error)
{
  RetroDiskControlCallbackReplaceImageIndex replace_image_index;
  gboolean result;

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  replace_image_index = self->disk_control_callback->replace_image_index;

  if (replace_image_index == NULL) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_NO_CALLBACK,
                         "DiskControl has no callback for this operation.");

    return FALSE;
  }

  retro_core_push_cb_data (self);
  result = replace_image_index (index, info);
  retro_core_pop_cb_data ();

  return result;
}

static gboolean
retro_core_add_disk_image_index (RetroCore  *self,
                                 GError    **error)
{
  RetroDiskControlCallbackAddImageIndex add_image_index;
  gboolean result;

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  add_image_index = self->disk_control_callback->add_image_index;

  if (add_image_index == NULL) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_NO_CALLBACK,
                         "DiskControl has no callback for this operation.");

    return FALSE;
  }

  retro_core_push_cb_data (self);
  result = add_image_index ();
  retro_core_pop_cb_data ();

  return result;
}

static void
retro_core_load_discs (RetroCore  *self,
                       GError    **error)
{
  guint length;
  gboolean fullpath;
  GFile *file;
  gchar *path;
  guint index;
  RetroGameInfo *game_info = NULL;
  GError *tmp_error = NULL;

  g_return_if_fail (RETRO_IS_CORE (self));

  retro_core_set_disk_ejected (self, TRUE, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return;
  }

  length = g_strv_length (self->media_uris);
  while (retro_core_get_disk_images_number (self, &tmp_error) < length &&
         (tmp_error != NULL)) {
    retro_core_add_disk_image_index (self, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);

      return;
    }
  }

  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return;
  }

  fullpath = retro_core_get_needs_full_path (self);
  for (index = 0; index < length; index++) {
    file = g_file_new_for_uri (self->media_uris[index]);
    path = g_file_get_path (file);

    if (fullpath) {
      game_info = retro_game_info_new (path);
    }
    else {
      game_info = retro_game_info_new_with_data (path, &tmp_error);
      if (G_UNLIKELY (tmp_error != NULL)) {
        g_propagate_error (error, tmp_error);

        if (game_info != NULL)
          retro_game_info_free (game_info);
        g_free (path);
        g_object_unref (file);

        return;
      }
    }

    retro_core_replace_disk_image_index (self, index, game_info, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);

      retro_game_info_free (game_info);
      g_free (path);
      g_object_unref (file);

      return;
    }

    retro_game_info_free (game_info);
    g_free (path);
    g_object_unref (file);
  }

  retro_core_set_disk_ejected (self, FALSE, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return;
  }
}

static gboolean
retro_core_load_game (RetroCore     *self,
                      RetroGameInfo *game)
{
  RetroUnloadGame unload_game;
  RetroLoadGame load_game;
  RetroGetSystemAvInfo get_system_av_info;
  gboolean game_loaded;
  RetroSystemAvInfo info = {{ 0 }};

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);
  g_return_val_if_fail (game != NULL, FALSE);

  if (retro_core_get_game_loaded (self)) {
    retro_core_push_cb_data (self);
    unload_game = retro_module_get_unload_game (self->module);
    unload_game ();
    retro_core_pop_cb_data ();
  }

  retro_core_push_cb_data (self);
  load_game = retro_module_get_load_game (self->module);
  game_loaded = load_game (game);
  retro_core_set_game_loaded (self, game_loaded);
  get_system_av_info = retro_module_get_get_system_av_info (self->module);
  get_system_av_info (&info);
  retro_core_set_system_av_info (self, &info);
  retro_core_pop_cb_data ();

  return game_loaded;
}

static gboolean
retro_core_prepare (RetroCore *self) {
  RetroLoadGame load_game;
  RetroGetSystemAvInfo get_system_av_info;
  gboolean game_loaded;
  RetroSystemAvInfo info = {{ 0 }};

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  retro_core_push_cb_data (self);
  load_game = retro_module_get_load_game (self->module);
  game_loaded = load_game (NULL);
  retro_core_set_game_loaded (self, game_loaded);
  get_system_av_info = retro_module_get_get_system_av_info (self->module);
  get_system_av_info (&info);
  retro_core_set_system_av_info (self, &info);
  retro_core_pop_cb_data ();

  return game_loaded;
}

static void
retro_core_load_medias (RetroCore *self,
                        GError** error)
{
  guint length;
  gchar *uri;
  GFile *file;
  gchar *path;
  gboolean fullpath;
  RetroGameInfo *game_info = NULL;
  GError *tmp_error = NULL;

  g_return_if_fail (RETRO_IS_CORE (self));
  length = g_strv_length (self->media_uris);

  if (length == 0) {
    retro_core_prepare (self);

    return;
  }

  uri = g_strdup (self->media_uris[0]);
  file = g_file_new_for_uri (uri);
  path = g_file_get_path (file);
  fullpath = retro_core_get_needs_full_path (self);
  if (fullpath) {
    game_info = retro_game_info_new (path);
  }
  else {
    game_info = retro_game_info_new_with_data (path, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);
      retro_game_info_free (game_info);
      g_free (path);
      g_object_unref (file);
      g_free (uri);

      return;
    }
  }
  if (!retro_core_load_game (self, game_info)) {
    retro_game_info_free (game_info);
    g_free (path);
    g_object_unref (file);
    g_free (uri);

    return;
  }
  if (self->disk_control_callback != NULL) {
    retro_core_load_discs (self, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);
      retro_game_info_free (game_info);
      g_free (path);
      g_object_unref (file);
      g_free (uri);

      return;
    }
  }
  retro_game_info_free (game_info);
  g_free (path);
  g_object_unref (file);
  g_free (uri);
}

void retro_core_set_environment_interface (RetroCore *self);
void retro_core_set_callbacks (RetroCore *self);

static gboolean
on_key_event (GtkWidget   *widget,
              GdkEventKey *event,
              gpointer     self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);
  g_return_val_if_fail (GTK_IS_WIDGET (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  return retro_core_key_event (RETRO_CORE (self), event);
}

/* Public */

/**
 * retro_core_get_api_version:
 * @self: a #RetroCore
 *
 * Gets the Libretro API version implement by the core.
 *
 * Returns: the API version
 */
guint
retro_core_get_api_version (RetroCore *self)
{
  guint result;
  RetroApiVersion api_version;

  g_return_val_if_fail (RETRO_IS_CORE (self), 0U);

  retro_core_push_cb_data (self);
  api_version = retro_module_get_api_version (self->module);
  result = api_version ();
  retro_core_pop_cb_data ();

  return result;
}

/**
 * retro_core_get_filename:
 * @self: a #RetroCore
 *
 * Gets the filename of the core.
 *
 * Returns: (transfer none): the filename of the core
 */
const gchar *
retro_core_get_filename (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);

  return self->filename;
}

/**
 * retro_core_get_system_directory:
 * @self: a #RetroCore
 *
 * Gets the system directory of the core.
 *
 * The core will look here for additional data, such as firmware ROMs or
 * configuration files.
 *
 * Returns: the system directory of the core
 */
const gchar *
retro_core_get_system_directory (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);

  return self->system_directory;
}

/**
 * retro_core_set_system_directory:
 * @self: a #RetroCore
 * @system_directory: the system directory
 *
 * Sets the system directory of the core.
 *
 * The core will look here for additional data, such as firmware ROMs or
 * configuration files.
 */
void
retro_core_set_system_directory (RetroCore   *self,
                                 const gchar *system_directory)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (g_strcmp0 (system_directory, retro_core_get_system_directory (self)) == 0)
    return;

  g_free (self->system_directory);
  self->system_directory = g_strdup (system_directory);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SYSTEM_DIRECTORY]);
}

const gchar *
retro_core_get_libretro_path (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);

  return self->libretro_path;
}

// FIXME This should be removed as it is deprecated by Libretro.
const gchar *
retro_core_get_content_directory (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);

  return self->content_directory;
}

// FIXME This should be removed as it is deprecated by Libretro.
void
retro_core_set_content_directory (RetroCore   *self,
                                  const gchar *content_directory)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (g_strcmp0 (content_directory, retro_core_get_content_directory (self)) == 0)
    return;

  g_free (self->content_directory);
  self->content_directory = g_strdup (content_directory);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_CONTENT_DIRECTORY]);
}

/**
 * retro_core_get_save_directory:
 * @self: a #RetroCore
 *
 * Gets the save directory of the core.
 *
 * The core will save some data here.
 *
 * Returns: the save directory of the core
 */
const gchar *
retro_core_get_save_directory (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);

  return self->save_directory;
}

/**
 * retro_core_set_save_directory:
 * @self: a #RetroCore
 * @save_directory: the save directory
 *
 * Sets the save directory of the core.
 *
 * The core will save some data here.
 */
void
retro_core_set_save_directory (RetroCore   *self,
                               const gchar *save_directory)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (g_strcmp0 (save_directory, retro_core_get_save_directory (self)) == 0)
    return;

  g_free (self->save_directory);
  self->save_directory = g_strdup (save_directory);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SAVE_DIRECTORY]);
}

/**
 * retro_core_get_is_initiated:
 * @self: a #RetroCore
 *
 * Gets whether the core has been initiated.
 *
 * Returns: whether the core has been initiated
 */
gboolean
retro_core_get_is_initiated (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  return self->is_initiated;
}

/**
 * retro_core_get_game_loaded:
 * @self: a #RetroCore
 *
 * Gets whether a game has been loaded.
 *
 * Returns: whether a game has been loaded
 */
gboolean
retro_core_get_game_loaded (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  return self->game_loaded;
}

/**
 * retro_core_get_support_no_game:
 * @self: a #RetroCore
 *
 * Gets whether the core supports running with no game.
 *
 * Returns: whether the core supports running with no game
 */
gboolean
retro_core_get_support_no_game (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  return self->support_no_game;
}

/**
 * retro_core_set_support_no_game:
 * @self: a #RetroCore
 * @support_no_game: the save directory
 *
 * Sets whether the core supports running with no game.
 */
void
retro_core_set_support_no_game (RetroCore *self,
                                gboolean   support_no_game)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (retro_core_get_support_no_game (self) == support_no_game)
    return;

  self->support_no_game = support_no_game;
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SUPPORT_NO_GAME]);
}

/**
 * retro_core_get_frames_per_second:
 * @self: a #RetroCore
 *
 * Gets the FPS rate for the core's video output.
 *
 * Returns: the FPS rate for the core's video output
 */
gdouble
retro_core_get_frames_per_second (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), 0.0);

  return self->frames_per_second;
}

/**
 * retro_core_boot:
 * @self: a #RetroCore
 * @error: return location for a #GError, or %NULL
 *
 * This initializes @self, loads its available options and loads the medias. You
 * need to boot @self before using some of its methods.
 */
void
retro_core_boot (RetroCore  *self,
                 GError    **error)
{
  RetroInit init;
  RetroControllerIterator *controller_iterator;
  guint *port;
  RetroController *controller;
  RetroControllerType controller_type;
  GError *tmp_error = NULL;

  g_return_if_fail (RETRO_IS_CORE (self));

  retro_core_set_environment_interface (self);

  retro_core_push_cb_data (self);
  init = retro_module_get_init (self->module);
  init ();
  retro_core_pop_cb_data ();

  controller_iterator = retro_core_iterate_controllers (self);
  while (retro_controller_iterator_next (controller_iterator,
                                         &port,
                                         &controller)) {
    controller_type = retro_controller_get_controller_type (controller);
    retro_core_set_controller_port_device (self, *port, controller_type);
  }
  g_object_unref (controller_iterator);

  retro_core_set_is_initiated (self, TRUE);

  retro_core_load_medias (self, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return;
  }
}

/**
 * retro_core_set_medias:
 * @self: a #RetroCore
 * @uris: (array zero-terminated=1) (element-type utf8)
 * (transfer none): the URIs
 *
 * Sets the medias to load into the core.
 *
 * You can use this before booting the core.
 */
void
retro_core_set_medias (RetroCore    *self,
                       const gchar **uris)
{

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (!retro_core_get_is_initiated (self));

  if (self->media_uris != NULL)
    g_strfreev (self->media_uris);

  self->media_uris = g_strdupv ((gchar **) uris);
}

/**
 * retro_core_set_current_media:
 * @self: a #RetroCore
 * @media_index: the media index
 * @error: return location for a #GError, or %NULL
 *
 * Sets the current media index.
 *
 * You can use this after booting the core.
 */
void
retro_core_set_current_media (RetroCore  *self,
                              guint       media_index,
                              GError    **error)
{
  guint length;
  GError *tmp_error = NULL;

  g_return_if_fail (RETRO_IS_CORE (self));
  length = g_strv_length (self->media_uris);

  g_return_if_fail (media_index < length);

  if (self->disk_control_callback == NULL)
    return;

  retro_core_set_disk_ejected (self, TRUE, &tmp_error);
  if (tmp_error != NULL) {
    g_propagate_error (error, tmp_error);

    return;
  }

  retro_core_set_disk_image_index (self, media_index, &tmp_error);
  if (tmp_error != NULL) {
    g_propagate_error (error, tmp_error);

    return;
  }

  retro_core_set_disk_ejected (self, FALSE, &tmp_error);
  if (tmp_error != NULL) {
    g_propagate_error (error, tmp_error);

    return;
  }
}

// FIXME Merge this into retro_core_set_controller().
void
retro_core_set_controller_port_device (RetroCore           *self,
                                       guint                port,
                                       RetroControllerType  controller_type)
{
  RetroSetControllerPortDevice set_controller_port_device;

  g_return_if_fail (RETRO_IS_CORE (self));

  retro_core_push_cb_data (self);
  set_controller_port_device = retro_module_get_set_controller_port_device (self->module);
  set_controller_port_device (port, controller_type);
  retro_core_pop_cb_data ();
}

/**
 * retro_core_reset:
 * @self: a #RetroCore
 *
 * Resets @self.
 */
void
retro_core_reset (RetroCore *self)
{
  RetroReset reset;

  g_return_if_fail (RETRO_IS_CORE (self));

  retro_core_push_cb_data (self);
  reset = retro_module_get_reset (self->module);
  reset ();
  retro_core_pop_cb_data ();
}

/**
 * retro_core_run:
 * @self: a #RetroCore
 *
 * Iterate @self for a frame.
 */
void
retro_core_run (RetroCore *self)
{
  RetroRun run;

  g_return_if_fail (RETRO_IS_CORE (self));

  retro_core_push_cb_data (self);
  run = retro_module_get_run (self->module);
  run ();
  retro_core_pop_cb_data ();
}

/**
 * retro_core_get_can_access_state:
 * @self: a #RetroCore
 *
 * Gets whether the state of @self can be accessed.
 *
 * Returns: whether the state of @self can be accessed
 */
gboolean
retro_core_get_can_access_state (RetroCore *self)
{
  RetroSerializeSize serialize_size = NULL;
  gsize size;

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  retro_core_push_cb_data (self);
  serialize_size = retro_module_get_serialize_size (self->module);
  size = serialize_size ();
  retro_core_pop_cb_data ();

  return size > 0;
}

/**
 * retro_core_get_state:
 * @self: a #RetroCore
 * @length: return location for the length of the returned data
 * @error: return location for a #GError, or %NULL
 *
 * Gets the state of @self.
 *
 * Returns: (array length=length): the content of the memory region
 */
guint8 *
retro_core_get_state (RetroCore  *self,
                      gsize      *length,
                      GError    **error)
{
  RetroSerializeSize serialize_size = NULL;
  RetroSerialize serialize = NULL;
  guint8 *data;
  gsize size;
  gboolean success;

  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);
  g_return_val_if_fail (length != NULL, NULL);

  serialize_size = retro_module_get_serialize_size (self->module);

  retro_core_push_cb_data (self);
  size = serialize_size ();
  retro_core_pop_cb_data ();

  if (size <= 0) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_SERIALIZATION_NOT_SUPPORTED,
                 "Couldn't serialize the internal state: serialization not supported.");

    return NULL;
  }

  serialize = retro_module_get_serialize (self->module);
  data = g_new0 (guint8, size);

  retro_core_push_cb_data (self);
  success = serialize (data, size);
  retro_core_pop_cb_data ();

  if (!success) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_COULDNT_SERIALIZE,
                 "Couldn't serialize the internal state: serialization failed.");
    g_free (data);

    return NULL;
  }

  *length = size;

  return data;
}

/**
 * retro_core_set_state:
 * @self: a #RetroCore
 * @data: (array length=length): the data to set
 * @length: the length of @data
 * @error: return location for a #GError, or %NULL
 *
 * Sets the state of the @self.
 */
void
retro_core_set_state (RetroCore     *self,
                      const guint8  *data,
                      gsize          length,
                      GError       **error)
{
  RetroSerializeSize serialize_size = NULL;
  RetroUnserialize unserialize = NULL;
  gsize size;
  gboolean success;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (data != NULL);

  serialize_size = retro_module_get_serialize_size (self->module);

  retro_core_push_cb_data (self);
  size = serialize_size ();
  retro_core_pop_cb_data ();

  if (size <= 0) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_SERIALIZATION_NOT_SUPPORTED,
                 "Couldn't deserialize the internal state: serialization not supported.");

    return;
  }

  if (length > size) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_COULDNT_DESERIALIZE,
                 "Couldn't deserialize the internal state: expected at most %"
                 G_GSIZE_FORMAT" bytes, got %"G_GSIZE_FORMAT".",
                 size,
                 length);

    return;
  }

  unserialize = retro_module_get_unserialize (self->module);

  retro_core_push_cb_data (self);
  success = unserialize ((guint8 *) data, length);
  retro_core_pop_cb_data ();

  if (!success) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_COULDNT_DESERIALIZE,
                 "Couldn't deserialize the internal state: deserialization failed.");
  }
}

/**
 * retro_core_get_memory_size:
 * @self: a #RetroCore
 * @memory_type: the type of memory
 *
 * Gets the size of a memory region of @self.
 *
 * Returns: the size of a memory region
 */
gsize
retro_core_get_memory_size (RetroCore       *self,
                            RetroMemoryType  memory_type)
{
  gsize size;
  RetroGetMemorySize get_memory_size;

  g_return_val_if_fail (RETRO_IS_CORE (self), 0UL);

  retro_core_push_cb_data (self);
  get_memory_size = retro_module_get_get_memory_size (self->module);
  size = get_memory_size (memory_type);
  retro_core_pop_cb_data ();

  return size;
}

/**
 * retro_core_get_memory:
 * @self: a #RetroCore
 * @memory_type: the type of memory
 *
 * Gets a memory region of @self.
 *
 * Returns: (transfer full): a #GBytes, or %NULL
 */
GBytes *
retro_core_get_memory (RetroCore       *self,
                       RetroMemoryType  memory_type)
{
  RetroGetMemoryData get_mem_data;
  RetroGetMemorySize get_mem_size;
  guint8 *data;
  gsize size;

  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);

  get_mem_data = retro_module_get_get_memory_data (self->module);
  get_mem_size = retro_module_get_get_memory_size (self->module);

  retro_core_push_cb_data (self);
  data = get_mem_data (memory_type);
  size = get_mem_size (memory_type);
  retro_core_pop_cb_data ();

  return g_bytes_new (data, size);
}

/**
 * retro_core_set_memory:
 * @self: a #RetroCore
 * @memory_type: the type of memory
 * @bytes: a #GBytes
 *
 * Sets a memory region of @self.
 */
void
retro_core_set_memory (RetroCore       *self,
                       RetroMemoryType  memory_type,
                       GBytes          *bytes)
{
  RetroGetMemoryData get_mem_region;
  RetroGetMemorySize get_mem_region_size;
  guint8 *memory_region;
  gsize memory_region_size;
  gconstpointer data;
  gsize size;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (bytes != NULL);

  get_mem_region = retro_module_get_get_memory_data (self->module);
  get_mem_region_size = retro_module_get_get_memory_size (self->module);

  retro_core_push_cb_data (self);
  memory_region = get_mem_region (memory_type);
  memory_region_size = get_mem_region_size (memory_type);
  retro_core_pop_cb_data ();

  data = g_bytes_get_data (bytes, &size);

  if (memory_region == NULL) {
    g_debug ("%s doesn't have memory region %d.",
             retro_core_get_name (self),
             memory_type);

    return;
  }

  if (memory_region_size == 0) {
    g_debug ("%s has an unexpected 0-sized non-null memory region %d. Aborting "
             "setting the memory region.",
             retro_core_get_name (self),
             memory_type);

    return;
  }

  if (memory_region_size < size) {
    g_debug ("%s expects %"G_GSIZE_FORMAT" bytes for memory region %d: %"
             G_GSIZE_FORMAT" bytes were passed. Aborting setting the memory "
             "region.",
             retro_core_get_name (self),
             memory_region_size,
             memory_type,
             size);

    return;
  }

  if (memory_region_size != size)
    g_debug ("%s expects %"G_GSIZE_FORMAT" bytes for memory region %d: %"
             G_GSIZE_FORMAT" bytes were passed. The excess will be filled with"
             "zeros.",
             retro_core_get_name (self),
             memory_region_size,
             memory_type,
             size);

  memcpy (memory_region, data, size);
  memset (memory_region + size, 0, memory_region_size - size);
}

/**
 * retro_core_poll_controllers:
 * @self: a #RetroCore
 *
 * Polls the pending input events for the controllers plugged into @self.
 */
void
retro_core_poll_controllers (RetroCore *self)
{
  RetroControllerIterator *iterator;
  guint *port;
  RetroController *controller;

  g_return_if_fail (RETRO_IS_CORE (self));

  iterator = retro_core_iterate_controllers (self);
  while (retro_controller_iterator_next (iterator, &port, &controller))
    if (controller != NULL)
      retro_controller_poll (controller);
  g_object_unref (iterator);
}

/**
 * retro_core_get_controller_input_state:
 * @self: a #RetroCore
 * @port: the port number
 * @controller_type: a #RetroControllerType to query @self
 * @index: an input index to interpret depending on @controller_type
 * @id: an input id to interpret depending on @controller_type
 *
 * Gets the state of an input of the controller plugged into the given port of
 * @self.
 *
 * Returns: the input's state
 */
gint16
retro_core_get_controller_input_state (RetroCore           *self,
                                       guint                port,
                                       RetroControllerType  controller_type,
                                       guint                index,
                                       guint                id)
{
  RetroController *controller;

  g_return_val_if_fail (RETRO_IS_CORE (self), 0);

  if (!g_hash_table_contains (self->controllers, &port))
    return 0;

  controller = g_hash_table_lookup (self->controllers, &port);

  if (controller == NULL)
    return 0;

  if ((retro_controller_get_capabilities (controller) & (1 << controller_type)) == 0)
    return 0;

  return retro_controller_get_input_state (controller,
                                           controller_type,
                                           index,
                                           id);
}

// FIXME documentation
void
retro_core_set_controller_descriptors (RetroCore            *self,
                                       RetroInputDescriptor *input_descriptors,
                                       gsize                 length)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  // TODO
}

// FIXME documentation
guint64
retro_core_get_controller_capabilities (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), 0);

  // TODO

  return 0;
}

/**
 * retro_core_set_controller:
 * @self: a #RetroCore
 * @port: the port number
 * @controller: (nullable): a #RetroController
 *
 * Plugs @controller into the specified port number of @self.
 */
void
retro_core_set_controller (RetroCore       *self,
                           guint            port,
                           RetroController *controller)
{
  guint *port_copy;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (RETRO_IS_CONTROLLER (controller));

  port_copy = g_new (guint, 1);
  *port_copy = port;
  g_hash_table_insert (self->controllers,
                       port_copy,
                       g_object_ref (controller));
  retro_core_controller_connected (self, port, controller);
}

/**
 * retro_core_set_keyboard:
 * @self: a #RetroCore
 * @widget: (nullable): a #GtkWidget, or %NULL
 *
 * Sets the widget whose key events will be forwarded to @self.
 */
void
retro_core_set_keyboard (RetroCore *self,
                         GtkWidget *widget)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (self->keyboard_widget != NULL) {
    g_signal_handler_disconnect (G_OBJECT (self->keyboard_widget), self->key_press_event_id);
    g_signal_handler_disconnect (G_OBJECT (self->keyboard_widget), self->key_release_event_id);
    g_clear_object (&self->keyboard_widget);
  }

  if (widget != NULL) {
    self->key_press_event_id =
      g_signal_connect_object (widget,
                               "key-press-event",
                               G_CALLBACK (on_key_event),
                               self,
                               0);
    self->key_release_event_id =
      g_signal_connect_object (widget,
                               "key-release-event",
                               G_CALLBACK (on_key_event),
                               self,
                               0);
    self->keyboard_widget = g_object_ref (widget);
  }
}

/**
 * retro_core_remove_controller:
 * @self: a #RetroCore
 * @port: the port number
 *
 * Removes the controller plugged into @self at port @port, if any.
 */
void
retro_core_remove_controller (RetroCore *self,
                              guint      port)
{

  g_return_if_fail (RETRO_IS_CORE (self));

  // FIXME Do that only if a controller is plugged into that port.
  g_hash_table_remove (self->controllers, &port);
  retro_core_controller_disconnected (self, port);
}

/**
 * retro_core_iterate_controllers:
 * @self: a #RetroCore
 *
 * Creates a new #RetroControllerIterator which can be used to iterate through
 * the controllers plugged into @self.
 *
 * Returns: (transfer full): a new #RetroControllerIterator
 */
RetroControllerIterator *
retro_core_iterate_controllers (RetroCore *self)
{

  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);

  return retro_controller_iterator_new (self->controllers);
}

/**
 * retro_core_new:
 * @filename: the filename of a Libretro core
 *
 * Creates a new #RetroCore.
 *
 * Returns: (transfer full): a new #RetroCore
 */
RetroCore *
retro_core_new (const gchar *filename)
{
  RetroCore *self;
  GFile *file;
  GFile *relative_path_file;

  g_return_val_if_fail (filename != NULL, NULL);

  self = g_object_new (RETRO_TYPE_CORE, NULL);

  retro_core_set_filename (self, filename);

  file = g_file_new_for_path (filename);
  relative_path_file = g_file_resolve_relative_path (file, "");

  g_object_unref (file);

  self->libretro_path = g_file_get_path (relative_path_file);

  g_object_unref (relative_path_file);

  self->module = retro_module_new (self->libretro_path);

  retro_core_set_callbacks (self);
  self->controllers = g_hash_table_new_full (g_int_hash, g_int_equal,
                                             g_free, g_object_unref);
  self->options = retro_options_new ();

  return self;
}
