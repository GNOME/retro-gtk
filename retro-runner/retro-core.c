// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-core-private.h"

#include <gio/gio.h>
#include <string.h>
#include "retro-core-error-private.h"
#include "retro-error-private.h"
#include "retro-environment-private.h"
#include "retro-input-private.h"
#include "retro-main-loop-source-private.h"
#include "retro-memfd-private.h"
#include "retro-rumble-effect.h"

G_DEFINE_QUARK (retro-core-error, retro_core_error)

G_DEFINE_TYPE (RetroCore, retro_core, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_API_VERSION,
  PROP_FILENAME,
  PROP_SYSTEM_DIRECTORY,
  PROP_CORE_ASSETS_DIRECTORY,
  PROP_SAVE_DIRECTORY,
  PROP_USER_NAME,
  PROP_IS_INITIATED,
  PROP_GAME_LOADED,
  PROP_SUPPORT_NO_GAME,
  PROP_FRAMES_PER_SECOND,
  PROP_RUNAHEAD,
  PROP_SPEED_RATE,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

enum {
  SIGNAL_VIDEO_OUTPUT,
  SIGNAL_AUDIO_OUTPUT,
  SIGNAL_ITERATED,
  SIGNAL_LOG,
  SIGNAL_SHUTDOWN,
  SIGNAL_MESSAGE,
  SIGNAL_VARIABLES_SET,
  SIGNAL_SET_RUMBLE_STATE,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

static RetroCore *retro_core_instance = NULL;

static void set_filename (RetroCore   *self,
                          const gchar *filename);

/* Private */

RetroCore *
retro_core_get_instance (void)
{
  g_assert (RETRO_IS_CORE (retro_core_instance));

  return retro_core_instance;
}

static void
retro_core_constructed (GObject *object)
{
  RetroCore *self = RETRO_CORE (object);
  g_autoptr (GFile) file = NULL;
  g_autoptr (GFile) relative_path_file = NULL;
  gint memfd;

  if (G_UNLIKELY (!self->filename))
    g_error ("A RetroCore’s “filename” property must be set when constructing it.");

  file = g_file_new_for_path (self->filename);
  relative_path_file = g_file_resolve_relative_path (file, "");

  self->libretro_path = g_file_get_path (relative_path_file);
  self->module = retro_module_new (self->libretro_path);

  retro_core_set_callbacks (self);

  memfd = retro_memfd_create ("[retro-runner framebuffer]");
  self->framebuffer = retro_framebuffer_new (memfd);

  G_OBJECT_CLASS (retro_core_parent_class)->constructed (object);
}

static void
retro_core_finalize (GObject *object)
{
  RetroCore *self = RETRO_CORE (object);
  RetroUnloadGame unload_game;
  RetroDeinit deinit;

  retro_core_stop (self);

  if (retro_core_get_game_loaded (self)) {
    unload_game = retro_module_get_unload_game (self->module);
    unload_game ();
  }
  deinit = retro_module_get_deinit (self->module);
  deinit ();

  g_clear_pointer (&self->media_uris, g_strfreev);

  g_object_unref (self->module);
  g_object_unref (self->framebuffer);
  g_clear_object (&self->default_controller);
  g_hash_table_unref (self->controllers);
  g_hash_table_unref (self->variables);
  g_hash_table_unref (self->variable_overrides);

  g_free (self->filename);
  g_free (self->system_directory);
  g_free (self->libretro_path);
  g_free (self->core_assets_directory);
  g_free (self->save_directory);
  g_clear_object (&self->renderer);

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
  case PROP_FILENAME:
    g_value_set_string (value, retro_core_get_filename (self));

    break;
  case PROP_SYSTEM_DIRECTORY:
    g_value_set_string (value, retro_core_get_system_directory (self));

    break;
  case PROP_CORE_ASSETS_DIRECTORY:
    g_value_set_string (value, retro_core_get_core_assets_directory (self));

    break;
  case PROP_SAVE_DIRECTORY:
    g_value_set_string (value, retro_core_get_save_directory (self));

    break;
  case PROP_USER_NAME:
    g_value_set_string (value, retro_core_get_user_name (self));

    break;
  case PROP_GAME_LOADED:
    g_value_set_boolean (value, retro_core_get_game_loaded (self));

    break;
  case PROP_SUPPORT_NO_GAME:
    g_value_set_boolean (value, retro_core_get_support_no_game (self));

    break;
  case PROP_FRAMES_PER_SECOND:
    g_value_set_double (value, retro_core_get_frames_per_second (self));

    break;
  case PROP_RUNAHEAD:
    g_value_set_uint (value, retro_core_get_runahead (self));

    break;
  case PROP_SPEED_RATE:
    g_value_set_double (value, retro_core_get_speed_rate (self));

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
    set_filename (self, g_value_get_string (value));

    break;
  case PROP_SYSTEM_DIRECTORY:
    retro_core_set_system_directory (self, g_value_get_string (value));

    break;
  case PROP_CORE_ASSETS_DIRECTORY:
    retro_core_set_core_assets_directory (self, g_value_get_string (value));

    break;
  case PROP_SAVE_DIRECTORY:
    retro_core_set_save_directory (self, g_value_get_string (value));

    break;
  case PROP_USER_NAME:
    retro_core_set_user_name (self, g_value_get_string (value));

    break;
  case PROP_RUNAHEAD:
    retro_core_set_runahead (self, g_value_get_uint (value));

    break;
  case PROP_SPEED_RATE:
    retro_core_set_speed_rate (self, g_value_get_double (value));

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

  object_class->constructed = retro_core_constructed;
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
                         G_PARAM_READWRITE |
                         G_PARAM_CONSTRUCT_ONLY |
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

  properties[PROP_CORE_ASSETS_DIRECTORY] =
    g_param_spec_string ("core-assets-directory",
                         "Core assets directory",
                         "The core assets directory",
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
   * RetroCore:user-name:
   *
   * The name of the user.
   */
  properties[PROP_USER_NAME] =
    g_param_spec_string ("user-name",
                         "User name",
                         "The user name",
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

  /**
   * RetroCore:runahead:
   *
   * The number of frames to run ahead of time. This allows to know in advance
   * what should happen in the given number of frames, allowing to react in
   * advance.
   *
   * Note that this is resource intensive as the core will be iterated over
   * multiple times for each perceived iteration. In order to work, this
   * requires the cores to properly support serialization, otherwise the
   * behavior is undefined.
   */
  properties[PROP_RUNAHEAD] =
    g_param_spec_uint ("runahead",
                       "Runahead",
                       "The number of frame to run ahead of time",
                       0,
                       G_MAXUINT,
                       0,
                       G_PARAM_READWRITE |
                       G_PARAM_STATIC_NAME |
                       G_PARAM_STATIC_NICK |
                       G_PARAM_STATIC_BLURB);

  /**
   * RetroCore:speed-rate:
   *
   * The speed ratio at wich the core will run.
   */
  properties[PROP_SPEED_RATE] =
    g_param_spec_double ("speed-rate",
                         "Speed rate",
                         "The speed ratio at wich the core will run",
                         -G_MAXDOUBLE, G_MAXDOUBLE, 1.0,
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB);

  g_object_class_install_properties (G_OBJECT_CLASS (klass), N_PROPS, properties);

  /**
   * RetroCore::video-output:
   *
   * The ::video-output signal is emitted each time a new video frame is emitted
   * by the core.
   */
  signals[SIGNAL_VIDEO_OUTPUT] =
    g_signal_new ("video-output", RETRO_TYPE_CORE, G_SIGNAL_RUN_FIRST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  0);

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
  signals[SIGNAL_AUDIO_OUTPUT] =
    g_signal_new ("audio-output", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  3,
                  G_TYPE_POINTER,
                  G_TYPE_ULONG,
                  G_TYPE_DOUBLE);

  /**
   * RetroCore::iterated:
   * @self: the #RetroCore
   *
   * The ::iterated signal is emitted at the end of a core's iteration.
   *
   * When ::runahead is > 0 and the core is iterated over multiple times
   * internally, ::iterated will be emitted only once, after the perceived
   * iteration is completed.
   */
  signals[SIGNAL_ITERATED] =
    g_signal_new ("iterated", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  0);

  /**
   * RetroCore::log:
   * @self: the #RetroCore
   * @log_domain: the log domain
   * @log_level: (type GLogLevelFlags): the log level
   * @message: the message
   *
   * The ::log signal is emitted each time the core emits a message to log.
   */
  signals[SIGNAL_LOG] =
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
   */
  signals[SIGNAL_SHUTDOWN] =
    g_signal_new ("shutdown", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  0);

  /**
   * RetroCore::message:
   * @self: the #RetroCore
   * @message: the message
   * @frames: the number of frames the message should be displayed
   *
   * The ::message signal is emitted each time the core emits a message to
   * display during a given amount of frames.
   */
  signals[SIGNAL_MESSAGE] =
    g_signal_new ("message", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  2,
                  G_TYPE_STRING,
                  G_TYPE_UINT);

  /**
   * RetroCore::variables-set:
   * @self: the #RetroCore
   * @variables: an array of #RetroVariable
   *
   * The ::variables-set signal is emitted when the core sets the
   * options during boot.
   *
   * @variables will be invalid after the signal emission, copy it in some way
   * if you want to keep it.
   */
  signals[SIGNAL_VARIABLES_SET] =
    g_signal_new ("variables-set", RETRO_TYPE_CORE, G_SIGNAL_RUN_FIRST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  1,
                  G_TYPE_POINTER);

  /**
   * RetroCore::set-rumble-state:
   * @self: the #RetroCore
   * @port: the port number
   * @effect: the rumble effect
   * @strength: the rumble effect strength
   *
   * The ::set-rumble-state signal is emitted when the core requests
   * controller on the port @port to set rumble state.
   */
  signals[SIGNAL_SET_RUMBLE_STATE] =
    g_signal_new ("set-rumble-state", RETRO_TYPE_CORE, G_SIGNAL_RUN_FIRST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  3,
                  G_TYPE_UINT,
                  RETRO_TYPE_RUMBLE_EFFECT,
                  G_TYPE_UINT);
}

static void
retro_core_init (RetroCore *self)
{
  self->variables = g_hash_table_new_full (g_str_hash, g_str_equal,
                                           g_free, g_free);
  self->variable_overrides = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                    g_free, g_free);

  self->controllers = g_hash_table_new_full (g_direct_hash, g_direct_equal,
                                             NULL, g_object_unref);

  self->main_loop = -1;
  self->speed_rate = 1;
}

static void
set_filename (RetroCore   *self,
              const gchar *filename)
{
  if (g_strcmp0 (filename, retro_core_get_filename (self)) == 0)
    return;

  g_free (self->filename);
  self->filename = g_strdup (filename);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_FILENAME]);
}

static void
set_is_initiated (RetroCore *self,
                  gboolean   is_initiated)
{
  if (retro_core_get_is_initiated (self) == is_initiated)
    return;

  self->is_initiated = is_initiated;
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_IS_INITIATED]);
}

static void
set_game_loaded (RetroCore *self,
                 gboolean   game_loaded)
{
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
get_system_info (RetroCore       *self,
                 RetroSystemInfo *system_info)
{
  RetroGetSystemInfo get_system_info;

  g_assert (system_info != NULL);

  get_system_info = retro_module_get_get_system_info (self->module);
  get_system_info (system_info);
}

static gboolean
get_needs_full_path (RetroCore *self)
{
  RetroSystemInfo system_info = { 0 };

  get_system_info (self, &system_info);

  return system_info.need_fullpath;
}

static void
restart (RetroCore *self)
{
  if (self->main_loop < 0)
    return;

  retro_core_stop (self);
  retro_core_run (self);
}

void
retro_core_set_system_av_info (RetroCore         *self,
                               RetroSystemAvInfo *system_av_info)
{
  if (self->frames_per_second != system_av_info->timing.fps) {
    self->frames_per_second = system_av_info->timing.fps;
    g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_FRAMES_PER_SECOND]);
    restart (self);
  }
  retro_core_set_geometry (self, &system_av_info->geometry);
  self->sample_rate = system_av_info->timing.sample_rate;
}

void
retro_core_set_geometry (RetroCore         *self,
                         RetroGameGeometry *geometry)
{
  if (geometry->aspect_ratio > 0.f)
    self->aspect_ratio = geometry->aspect_ratio;
  else
    self->aspect_ratio = (float) geometry->base_width /
                         (float) geometry->base_height;
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

  get_system_info (self, &system_info);

  return system_info.library_name;
}

void
retro_core_update_variable (RetroCore   *self,
                            const gchar *key,
                            const gchar *value)
{
  g_assert (key != NULL);
  g_assert (value != NULL);

  g_hash_table_replace (self->variables, g_strdup (key), g_strdup (value));

  self->variable_updated = TRUE;
}

static gboolean
set_disk_ejected (RetroCore  *self,
                  gboolean    ejected,
                  GError    **error)
{
  RetroDiskControlCallbackSetEjectState set_eject_state;

  set_eject_state = self->disk_control_callback->set_eject_state;

  if (set_eject_state == NULL) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_NO_CALLBACK,
                         "DiskControl has no callback for this operation.");

    return FALSE;
  }

  return set_eject_state (ejected);
}

static gboolean
set_disk_image_index (RetroCore  *self,
                      guint       index,
                      GError    **error)
{
  RetroDiskControlCallbackSetImageIndex set_image_index;

  set_image_index = self->disk_control_callback->set_image_index;

  if (set_image_index == NULL) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_NO_CALLBACK,
                         "DiskControl has no callback for this operation.");

    return FALSE;
  }

  return set_image_index (index);
}

static guint
get_disk_images_number (RetroCore  *self,
                        GError    **error)
{
  RetroDiskControlCallbackGetNumImages get_num_images;

  get_num_images = self->disk_control_callback->get_num_images;

  if (get_num_images == NULL) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_NO_CALLBACK,
                         "DiskControl has no callback for this operation.");

    return FALSE;
  }

  return get_num_images ();
}

static gboolean
replace_disk_image_index (RetroCore      *self,
                          guint           index,
                          RetroGameInfo  *info,
                          GError        **error)
{
  RetroDiskControlCallbackReplaceImageIndex replace_image_index;

  replace_image_index = self->disk_control_callback->replace_image_index;

  if (replace_image_index == NULL) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_NO_CALLBACK,
                         "DiskControl has no callback for this operation.");

    return FALSE;
  }

  return replace_image_index (index, info);
}

static gboolean
add_disk_image_index (RetroCore  *self,
                      GError    **error)
{
  RetroDiskControlCallbackAddImageIndex add_image_index;

  add_image_index = self->disk_control_callback->add_image_index;

  if (add_image_index == NULL) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_NO_CALLBACK,
                         "DiskControl has no callback for this operation.");

    return FALSE;
  }

  return add_image_index ();
}

static void
load_discs (RetroCore  *self,
            GError    **error)
{
  guint length;
  gboolean fullpath;

  retro_try_propagate ({
    set_disk_ejected (self, TRUE, &catch);
  }, catch, error);

  length = g_strv_length (self->media_uris);
  retro_try_propagate ({
    while (get_disk_images_number (self, &catch) < length && (catch == NULL)) {
      retro_try_propagate ({
        add_disk_image_index (self, &catch);
      }, catch, error);
    }
  }, catch, error);

  fullpath = get_needs_full_path (self);
  for (gsize index = 0; index < length; index++) {
    g_autoptr (RetroGameInfo) game_info = NULL;

    retro_try_propagate ({
      game_info = retro_game_info_new (self->media_uris[index],
                                       fullpath,
                                       &catch);
    }, catch, error);

    retro_try_propagate ({
      replace_disk_image_index (self, index, game_info, &catch);
    }, catch, error);
  }

  retro_try_propagate ({
    set_disk_ejected (self, FALSE, &catch);
  }, catch, error);
}

static gboolean
load_game (RetroCore     *self,
           RetroGameInfo *game)
{
  RetroUnloadGame unload_game;
  RetroLoadGame load_game;
  RetroGetSystemAvInfo get_system_av_info;
  gboolean game_loaded;
  RetroSystemAvInfo info = {{ 0 }};

  g_assert (game != NULL);

  if (retro_core_get_game_loaded (self)) {
    unload_game = retro_module_get_unload_game (self->module);
    unload_game ();
  }

  load_game = retro_module_get_load_game (self->module);
  game_loaded = load_game (game);
  set_game_loaded (self, game_loaded);
  get_system_av_info = retro_module_get_get_system_av_info (self->module);
  get_system_av_info (&info);
  retro_core_set_system_av_info (self, &info);
  if (self->renderer)
    retro_renderer_realize (self->renderer, info.geometry.max_width, info.geometry.max_height);

  return game_loaded;
}

static gboolean
prepare (RetroCore *self) {
  RetroLoadGame load_game;
  RetroGetSystemAvInfo get_system_av_info;
  gboolean game_loaded;
  RetroSystemAvInfo info = {{ 0 }};

  load_game = retro_module_get_load_game (self->module);
  game_loaded = load_game (NULL);
  set_game_loaded (self, game_loaded);
  get_system_av_info = retro_module_get_get_system_av_info (self->module);
  get_system_av_info (&info);
  retro_core_set_system_av_info (self, &info);
  if (self->renderer)
    retro_renderer_realize (self->renderer, info.geometry.max_width, info.geometry.max_height);

  return game_loaded;
}

static void
load_medias (RetroCore  *self,
             GError    **error)
{
  guint length;
  g_autoptr (RetroGameInfo) game_info = NULL;

  length = self->media_uris == NULL ? 0 : g_strv_length (self->media_uris);

  if (length == 0) {
    prepare (self);

    return;
  }

  retro_try_propagate ({
    game_info = retro_game_info_new (self->media_uris[0],
                                     get_needs_full_path (self),
                                     &catch);
  }, catch, error);

  if (!load_game (self, game_info))
    return;

  if (self->disk_control_callback != NULL)
    retro_try_propagate ({
      load_discs (self, &catch);
    }, catch, error);
}

/* FIXME: this is partially copied from retro_option_new() */
static gchar *
get_default_value (const gchar *description)
{
  const gchar *description_separator, *value_separator, *values;

  description_separator = g_strstr_len (description, -1, "; ");
  if (G_UNLIKELY (description_separator == NULL))
    return NULL;

  values = description_separator + 2;
  value_separator = g_strstr_len (values, -1, "|");

  if (!value_separator)
    return g_strdup (values);

  return g_strndup (values, value_separator - values);
}

void
retro_core_insert_variable (RetroCore           *self,
                            const RetroVariable *variable)
{
  gchar *value;

  if (g_hash_table_contains (self->variable_overrides, variable->key))
    value = g_strdup (g_hash_table_lookup (self->variable_overrides,
                                           variable->key));
  else
    value = get_default_value (variable->value);

  g_hash_table_insert (self->variables, g_strdup (variable->key), value);
}

gboolean
retro_core_get_variable_update (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  if (!self->variable_updated)
    return FALSE;

  self->variable_updated = FALSE;

  return TRUE;
}

gdouble
retro_core_get_sample_rate (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), 0);

  return self->sample_rate;
}

gint
retro_core_get_framebuffer_fd (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), 0);

  return retro_framebuffer_get_fd (self->framebuffer);
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
  RetroApiVersion api_version;

  g_return_val_if_fail (RETRO_IS_CORE (self), 0U);

  api_version = retro_module_get_api_version (self->module);

  return api_version ();
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

const gchar *
retro_core_get_core_assets_directory (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);

  return self->core_assets_directory;
}

void
retro_core_set_core_assets_directory (RetroCore   *self,
                                      const gchar *core_assets_directory)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (g_strcmp0 (core_assets_directory, retro_core_get_core_assets_directory (self)) == 0)
    return;

  g_free (self->core_assets_directory);
  self->core_assets_directory = g_strdup (core_assets_directory);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_CORE_ASSETS_DIRECTORY]);
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
 * retro_core_get_user_name:
 * @self: a #RetroCore
 *
 * Gets the name of the user.
 *
 * Returns: the name of the user
 */
const gchar *
retro_core_get_user_name (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);

  return self->user_name;
}

/**
 * retro_core_set_user_name:
 * @self: a #RetroCore
 * @user_name: the user name
 *
 * Sets the name of the user.
 */
void
retro_core_set_user_name (RetroCore   *self,
                          const gchar *user_name)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (g_strcmp0 (user_name, retro_core_get_user_name (self)) == 0)
    return;

  g_free (self->user_name);
  self->user_name = g_strdup (user_name);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_USER_NAME]);
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
 * @support_no_game: whether the core supports running with no game
 *
 * Sets whether the core supports running with no game.
 */
void
retro_core_set_support_no_game (RetroCore *self,
                                gboolean   support_no_game)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  support_no_game = !!support_no_game;

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

  g_return_if_fail (RETRO_IS_CORE (self));

  retro_core_set_environment_interface (self);

  init = retro_module_get_init (self->module);
  init ();

  set_is_initiated (self, TRUE);

  retro_try_propagate ({
    load_medias (self, &catch);
  }, catch, error);
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
retro_core_set_medias (RetroCore           *self,
                       const gchar * const *uris)
{
  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (!retro_core_get_is_initiated (self));

  g_clear_pointer (&self->media_uris, g_strfreev);
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

  g_return_if_fail (RETRO_IS_CORE (self));

  length = g_strv_length (self->media_uris);

  g_return_if_fail (media_index < length);

  if (self->disk_control_callback == NULL)
    return;

  retro_try_propagate ({
    set_disk_ejected (self, TRUE, &catch);
  }, catch, error);

  retro_try_propagate ({
    set_disk_image_index (self, media_index, &catch);
  }, catch, error);

  retro_try_propagate ({
    set_disk_ejected (self, FALSE, &catch);
  }, catch, error);
}

void
retro_core_set_default_controller (RetroCore *self,
                                   gint       fd)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  /* We cannot use g_set_object() because it would reference the new object,
   * leaking its initial reference.
   */
  g_clear_object (&self->default_controller);
  self->default_controller = retro_controller_state_new (fd);
}

void
retro_core_set_controller (RetroCore           *self,
                           guint                port,
                           RetroControllerType  controller_type,
                           gint                 fd)
{
  RetroSetControllerPortDevice set_controller_port_device;

  g_return_if_fail (RETRO_IS_CORE (self));

  if (controller_type == RETRO_CONTROLLER_TYPE_NONE)
    g_hash_table_remove (self->controllers, GUINT_TO_POINTER (port));
  else
    g_hash_table_insert (self->controllers, GUINT_TO_POINTER (port),
                         retro_controller_state_new (fd));

  set_controller_port_device = retro_module_get_set_controller_port_device (self->module);
  set_controller_port_device (port, controller_type);
}

gboolean
retro_core_get_controller_supports_rumble (RetroCore *self,
                                           guint      port)
{
  RetroControllerState *controller;

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  if (!g_hash_table_contains (self->controllers, GUINT_TO_POINTER (port)))
    return FALSE;

  controller = g_hash_table_lookup (self->controllers, GUINT_TO_POINTER (port));
  g_return_val_if_fail (controller != NULL, FALSE);

  return retro_controller_state_get_supports_rumble (controller);
}

void
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
run_main_loop (RetroCore *self)
{
  if (self->main_loop < 0)
    return FALSE;

  retro_core_iteration (self);

  return TRUE;
}

/**
 * retro_core_run:
 * @self: a #RetroCore
 *
 * Starts running the core. If the core was stopped, it will restart from this
 * moment.
 */
void
retro_core_run (RetroCore *self)
{
  gdouble fps;
  g_autoptr (GSource) source = NULL;

  g_return_if_fail (RETRO_IS_CORE (self));

  if (self->main_loop >= 0 || self->speed_rate <= 0)
    return;

  // TODO What if fps <= 0?
  fps = retro_core_get_frames_per_second (self);
  /* Do not make the timeout source hold a reference on the RetroCore, so
   * destroying the RetroCore while it is still running will stop it instead
   * of leaking a reference.
   */
  source = retro_main_loop_source_new (fps * self->speed_rate);
  g_source_set_callback (source, (GSourceFunc) run_main_loop, self, NULL);
  self->main_loop = g_source_attach (source, g_main_context_default ());
}

/**
 * retro_core_stop:
 * @self: a #RetroCore
 *
 * Stops running the core.
 */
void
retro_core_stop (RetroCore *self)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (self->main_loop < 0)
    return;

  g_source_remove (self->main_loop);
  self->main_loop = -1;
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

  reset = retro_module_get_reset (self->module);
  reset ();
}

static inline void
emit_iterated (RetroCore **self)
{
  if (*self)
    g_signal_emit (*self, signals[SIGNAL_ITERATED], 0);
}

/**
 * retro_core_iteration:
 * @self: a #RetroCore
 *
 * Iterate @self for a frame.
 */
void
retro_core_iteration (RetroCore *self)
{
  RetroRun run;
  RetroSerializeSize serialize_size = NULL;
  RetroSerialize serialize = NULL;
  RetroUnserialize unserialize = NULL;
  g_autofree guint8 *data = NULL;
  gsize size;
  gsize new_size;
  gboolean success;
  RetroCore *iterated __attribute__((cleanup(emit_iterated))) = NULL;

  g_return_if_fail (RETRO_IS_CORE (self));

  self->has_run = TRUE;

  iterated = self;
  run = retro_module_get_run (self->module);

  if (self->runahead == 0) {
    self->run_remaining = 0;
    run ();

    return;
  }

  serialize_size = retro_module_get_serialize_size (self->module);
  size = serialize_size ();

  if (size == 0) {
    self->run_remaining = 0;
    run ();

    g_critical ("Couldn't run ahead: serialization not supported.");

    return;
  }

  self->run_remaining = self->runahead;
  run ();

  self->run_remaining--;

  new_size = serialize_size ();

  if (size > new_size) {
    g_critical ("Couldn't run ahead: unexpected serialization size %"
                G_GSIZE_FORMAT", expected %"G_GSIZE_FORMAT" or less.",
                new_size, size);

    return;
  }

  size = new_size;
  data = g_new0 (guint8, size);

  serialize = retro_module_get_serialize (self->module);
  success = serialize (data, size);

  if (!success) {
    g_critical ("Couldn't run ahead: serialization unexpectedly failed.");

    return;
  }

  for (; self->run_remaining >= 0; self->run_remaining--)
    run ();

  new_size = serialize_size ();

  if (size > new_size) {
    g_critical ("Couldn't run ahead: unexpected deserialization size %"
                G_GSIZE_FORMAT", expected %"G_GSIZE_FORMAT" or less.",
                new_size, size);

    return;
  }

  unserialize = retro_module_get_unserialize (self->module);
  success = unserialize ((guint8 *) data, size);

  if (!success) {
    g_critical ("Couldn't run ahead: deserialization unexpectedly failed.");

    return;
  }
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

  serialize_size = retro_module_get_serialize_size (self->module);
  size = serialize_size ();

  return size > 0;
}

/**
 * retro_core_save_state:
 * @self: a #RetroCore
 * @filename: the file to save the state to
 * @error: return location for a #GError, or %NULL
 *
 * Saves the state of @self.
 */
void
retro_core_save_state (RetroCore    *self,
                       const gchar  *filename,
                       GError      **error)
{
  RetroSerializeSize serialize_size = NULL;
  RetroSerialize serialize = NULL;
  g_autofree guint8 *data = NULL;
  gsize size;
  gboolean success;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (filename != NULL);

  serialize_size = retro_module_get_serialize_size (self->module);
  size = serialize_size ();

  if (size <= 0) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_SERIALIZATION_NOT_SUPPORTED,
                         "Couldn't serialize the internal state: serialization not supported.");

    return;
  }

  serialize = retro_module_get_serialize (self->module);
  data = g_new0 (guint8, size);

  success = serialize (data, size);

  if (!success) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_COULDNT_SERIALIZE,
                         "Couldn't serialize the internal state: serialization failed.");

    return;
  }

  retro_try ({
    g_file_set_contents (filename, (gchar *) data, size, &catch);
  }, catch, {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_COULDNT_ACCESS_FILE,
                 "Couldn't serialize the internal state: %s",
                 catch->message);

    return;
  });
}

/**
 * retro_core_load_state:
 * @self: a #RetroCore
 * @filename: the file to load the state from
 * @error: return location for a #GError, or %NULL
 *
 * Loads the state of the @self.
 */
void
retro_core_load_state (RetroCore    *self,
                       const gchar  *filename,
                       GError      **error)
{
  RetroSerializeSize serialize_size = NULL;
  RetroUnserialize unserialize = NULL;
  gsize expected_size, data_size;
  g_autofree gchar *data = NULL;
  gboolean success;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (filename != NULL);

  retro_try ({
    g_file_get_contents (filename, &data, &data_size, &catch);
  }, catch, {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_COULDNT_ACCESS_FILE,
                 "Couldn't deserialize the internal state: %s",
                 catch->message);

    return;
  });

  /* Some cores, such as MAME and ParaLLEl N64, can only properly restore the
   * state after at least one frame has been run. */
  if (!self->has_run) {
    RetroRun run = retro_module_get_run (self->module);

    /* Ignore the video output here to avoid briefly showing the previous state
     * in case user is showing a screenshot of the state to restore here. */
    self->block_video_signal = TRUE;
    run ();
    self->block_video_signal = FALSE;

    self->has_run = TRUE;
  }

  serialize_size = retro_module_get_serialize_size (self->module);
  expected_size = serialize_size ();

  if (expected_size == 0) {
    g_set_error_literal (error,
                         RETRO_CORE_ERROR,
                         RETRO_CORE_ERROR_SERIALIZATION_NOT_SUPPORTED,
                         "Couldn't deserialize the internal state: serialization not supported.");

    return;
  }

  if (data_size != expected_size)
    g_critical ("%s expects %"G_GSIZE_FORMAT" bytes for its internal state, but %"
                G_GSIZE_FORMAT" bytes were passed.",
                retro_core_get_name (self),
                expected_size,
                data_size);

  unserialize = retro_module_get_unserialize (self->module);
  success = unserialize ((guint8 *) data, data_size);

  if (!success) {
    g_set_error_literal (error,
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
  RetroGetMemorySize get_memory_size;

  g_return_val_if_fail (RETRO_IS_CORE (self), 0UL);

  get_memory_size = retro_module_get_get_memory_size (self->module);

  return get_memory_size (memory_type);
}

/**
 * retro_core_save_memory:
 * @self: a #RetroCore
 * @memory_type: the type of memory
 * @filename: a file to save the data to
 * @error: return location for a #GError, or %NULL
 *
 * Saves a memory region of @self.
 */
void
retro_core_save_memory (RetroCore        *self,
                        RetroMemoryType   memory_type,
                        const gchar      *filename,
                        GError          **error)
{
  RetroGetMemoryData get_mem_data;
  RetroGetMemorySize get_mem_size;
  gchar *data;
  gsize size;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (filename != NULL);

  get_mem_data = retro_module_get_get_memory_data (self->module);
  get_mem_size = retro_module_get_get_memory_size (self->module);
  data = get_mem_data (memory_type);
  size = get_mem_size (memory_type);

  retro_try ({
    g_file_set_contents (filename, data, size, &catch);
  }, catch, {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_COULDNT_ACCESS_FILE,
                 "Couldn't save the memory state: %s",
                 catch->message);

    return;
  });
}

/**
 * retro_core_load_memory:
 * @self: a #RetroCore
 * @memory_type: the type of memory
 * @filename: a file to load the data from
 * @error: return location for a #GError, or %NULL
 *
 * Loads a memory region of @self.
 */
void
retro_core_load_memory (RetroCore        *self,
                        RetroMemoryType   memory_type,
                        const gchar      *filename,
                        GError          **error)
{
  RetroGetMemoryData get_mem_region;
  RetroGetMemorySize get_mem_region_size;
  guint8 *memory_region;
  gsize memory_region_size;
  g_autofree gchar *data = NULL;
  gsize data_size;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (filename != NULL);

  get_mem_region = retro_module_get_get_memory_data (self->module);
  get_mem_region_size = retro_module_get_get_memory_size (self->module);
  memory_region = get_mem_region (memory_type);
  memory_region_size = get_mem_region_size (memory_type);

  retro_try ({
    g_file_get_contents (filename, &data, &data_size, &catch);
  }, catch, {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_COULDNT_ACCESS_FILE,
                 "Couldn't load the memory state: %s",
                 catch->message);

    return;
  });

  if (memory_region == NULL) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_NO_MEMORY_REGION,
                 "Couldn't load the memory state: %s doesn't have memory region %d",
                 retro_core_get_name (self),
                 memory_type);

    return;
  }

  if (memory_region_size == 0) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_UNEXPECTED_MEMORY_REGION,
                 "Couldn't load the memory state: %s has an unexpected 0-sized non-null memory region %d",
                 retro_core_get_name (self),
                 memory_type);

    return;
  }

  if (memory_region_size < data_size) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_SIZE_MISMATCH,
                 "Couldn't load the memory state: %s expects %"G_GSIZE_FORMAT
                 " bytes for memory region %d: %"G_GSIZE_FORMAT
                 " bytes were passed",
                 retro_core_get_name (self),
                 memory_region_size,
                 memory_type,
                 data_size);

    return;
  }

  if (memory_region_size != data_size)
    g_debug ("%s expects %"G_GSIZE_FORMAT" bytes for memory region %d: %"
             G_GSIZE_FORMAT" bytes were passed. The excess will be filled with "
             "zeros.",
             retro_core_get_name (self),
             memory_region_size,
             memory_type,
             data_size);

  memcpy (memory_region, data, data_size);
  memset (memory_region + data_size, 0, memory_region_size - data_size);
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
  GHashTableIter iter;
  gpointer port;
  RetroControllerState *controller;

  g_return_if_fail (RETRO_IS_CORE (self));

  retro_controller_state_lock (self->default_controller);
  retro_controller_state_snapshot (self->default_controller);
  retro_controller_state_unlock (self->default_controller);

  g_hash_table_iter_init (&iter, self->controllers);

  while (g_hash_table_iter_next (&iter, &port, (gpointer *) &controller)) {
    retro_controller_state_lock (controller);
    retro_controller_state_snapshot (controller);
    retro_controller_state_unlock (controller);
  }
}

/**
 * retro_core_get_controller_input_state:
 * @self: a #RetroCore
 * @port: the port number
 * @input: a #RetroInput
 *
 * Gets the state of an input of the controller plugged into the given port of
 * @self.
 *
 * Returns: the input's state
 */
gint16
retro_core_get_controller_input_state (RetroCore  *self,
                                       guint       port,
                                       RetroInput *input)
{
  RetroControllerType type;
  RetroControllerState *controller;

  g_return_val_if_fail (RETRO_IS_CORE (self), 0);

  type = retro_input_get_controller_type (input) & RETRO_CONTROLLER_TYPE_TYPE_MASK;

  controller = g_hash_table_lookup (self->controllers, GUINT_TO_POINTER (port));
  if (controller && retro_controller_state_has_type (controller, type))
    return retro_controller_state_get_input (controller, input);

  if (self->default_controller && retro_controller_state_has_type (self->default_controller, type))
    return retro_controller_state_get_input (self->default_controller, input);

  return 0;
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

guint
retro_core_get_runahead (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), 0);

  return self->runahead;
}

void
retro_core_set_runahead (RetroCore *self,
                         guint      runahead)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  self->runahead = runahead;
}

gboolean
retro_core_is_running_ahead (RetroCore *self)
{
  return self->run_remaining > 0;
}

/**
 * retro_core_get_speed_rate:
 * @self: a #RetroCore
 *
 * Gets the speed rate at which to run the core.
 *
 * Returns: the speed rate
 */
gdouble
retro_core_get_speed_rate (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), 1.0);

  return self->speed_rate;
}

/**
 * retro_core_set_speed_rate:
 * @self: a #RetroCore
 * @speed_rate: a speed rate
 *
 * Sets the speed rate at which to run the core.
 */
void
retro_core_set_speed_rate (RetroCore *self,
                           gdouble    speed_rate)
{
  g_return_if_fail (RETRO_IS_CORE (self));

  if (self->speed_rate == speed_rate)
    return;

  self->speed_rate = speed_rate;
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SPEED_RATE]);

  restart (self);
}

/**
 * retro_core_override_variable_default:
 * @self: a #RetroCore
 * @key: the key of the variable
 * @value: the default value
 *
 * Overrides default value for the variable @key. This can be used to set value
 * for a startup-only option.
 *
 * You can use this before booting the core.
 *
 * See retro_core_override_option_default() in retro-gtk/retro-core.c
 */
void
retro_core_override_variable_default (RetroCore   *self,
                                      const gchar *key,
                                      const gchar *value)
{
  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (key != NULL);
  g_return_if_fail (value != NULL);

  g_hash_table_replace (self->variable_overrides, g_strdup (key), g_strdup (value));
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
  g_return_val_if_fail (filename != NULL, NULL);
  g_return_val_if_fail (retro_core_instance == NULL, NULL);

  retro_core_instance = g_object_new (RETRO_TYPE_CORE, "filename", filename, NULL);

  return retro_core_instance;
}
