// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-core.h"

#include <errno.h>
#include <sys/mman.h>
#include <gio/gunixfdlist.h>
#include <string.h>
#include "retro-controller-codes.h"
#include "retro-controller-iterator-private.h"
#include "retro-controller-state-private.h"
#include "retro-controller-type.h"
#include "retro-framebuffer-private.h"
#include "retro-input-private.h"
#include "retro-keyboard-private.h"
#include "retro-memfd-private.h"
#include "retro-option-iterator-private.h"
#include "retro-option-private.h"
#include "retro-pixel-format-private.h"
#include "retro-pixdata-private.h"
#include "retro-runner-process-private.h"

#define RETRO_CORE_ERROR (retro_core_error_quark ())

enum {
  RETRO_CORE_ERROR_COULDNT_ACCESS_FILE,
  RETRO_CORE_ERROR_COULDNT_SERIALIZE,
  RETRO_CORE_ERROR_COULDNT_DESERIALIZE,
  RETRO_CORE_ERROR_SERIALIZATION_NOT_SUPPORTED,
  RETRO_CORE_ERROR_NO_CALLBACK,
  RETRO_CORE_ERROR_NO_MEMORY_REGION,
  RETRO_CORE_ERROR_UNEXPECTED_MEMORY_REGION,
  RETRO_CORE_ERROR_SIZE_MISMATCH,
};

G_DEFINE_QUARK (retro-core-error, retro_core_error)

typedef struct {
  RetroController *controller;
  guint port;
  gulong state_changed_id;
  RetroControllerState *state;
} RetroCoreControllerInfo;

typedef struct {
  RetroController *controller;
  RetroControllerType type;
  gulong state_changed_id;
  RetroControllerState *state;
} RetroCoreDefaultControllerInfo;

struct _RetroCore
{
  GObject parent_instance;

  RetroRunnerProcess *process;

  gchar *filename;
  gchar *system_directory;
  gchar *content_directory;
  gchar *save_directory;

  gchar **media_uris;
  gdouble frames_per_second;
  gboolean game_loaded;
  gboolean support_no_game;

  GHashTable *options;
  GHashTable *option_overrides;

  RetroControllerState *default_controller_state;
  RetroCoreDefaultControllerInfo *default_controllers[RETRO_CONTROLLER_TYPE_COUNT];
  GHashTable *controllers;

  gdouble runahead;
  gdouble speed_rate;

  GtkWidget *keyboard_widget;
  gulong key_press_event_id;
  gulong key_release_event_id;

  RetroFramebuffer *framebuffer;
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
  PROP_RUNAHEAD,
  PROP_SPEED_RATE,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

enum {
  SIG_VIDEO_OUTPUT_SIGNAL,
  SIG_LOG_SIGNAL,
  SIG_SHUTDOWN_SIGNAL,
  SIG_MESSAGE_SIGNAL,
  SIG_CRASHED_SIGNAL,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

static void retro_core_set_filename (RetroCore   *self,
                                     const gchar *filename);

static void exit_cb (RetroRunnerProcess *process,
                     gboolean            success,
                     gchar              *error,
                     RetroCore          *self);

/* Private */

static void
free_controller_info (RetroCoreControllerInfo *info)
{
  g_signal_handler_disconnect(info->controller, info->state_changed_id);
  g_object_unref (info->controller);
  g_object_unref (info->state);
  g_free (info);
}

static void
free_default_controller_info (RetroCoreDefaultControllerInfo *info)
{
  g_signal_handler_disconnect(info->controller, info->state_changed_id);
  g_object_unref (info->controller);
  g_free (info);
}

static void
retro_core_constructed (GObject *object)
{
  RetroCore *self = RETRO_CORE (object);

  if (G_UNLIKELY (!self->filename))
    g_error ("A RetroCore’s “filename” property must be set when constructing it.");

  self->process = retro_runner_process_new (self->filename);
  g_signal_connect_object (self->process, "exit", G_CALLBACK (exit_cb), self, 0);

  G_OBJECT_CLASS (retro_core_parent_class)->constructed (object);
}

static void
retro_core_finalize (GObject *object)
{
  RetroCore *self = RETRO_CORE (object);
  gint i;

  retro_core_set_keyboard (self, NULL);
  g_object_unref (self->framebuffer);

  if (self->media_uris != NULL)
    g_strfreev (self->media_uris);

  for (i = 0; i < RETRO_CONTROLLER_TYPE_COUNT; i++)
    if (self->default_controllers[i])
      free_default_controller_info (self->default_controllers[i]);
  g_object_unref (self->default_controller_state);

  g_hash_table_unref (self->controllers);
  g_hash_table_unref (self->options);
  g_hash_table_unref (self->option_overrides);

  g_object_unref (self->process);
  g_free (self->filename);
  g_free (self->system_directory);
  g_free (self->content_directory);
  g_free (self->save_directory);
  g_clear_object (&self->keyboard_widget);

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
  case PROP_CONTENT_DIRECTORY:
    g_value_set_string (value, retro_core_get_content_directory (self));

    break;
  case PROP_SAVE_DIRECTORY:
    g_value_set_string (value, retro_core_get_save_directory (self));

    break;
  case PROP_IS_INITIATED:
    g_value_set_boolean (value, retro_core_get_is_initiated (self));

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
                         0.0, G_MAXDOUBLE, 1.0,
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB);

  g_object_class_install_properties (G_OBJECT_CLASS (klass), N_PROPS, properties);

  /**
   * RetroCore::video-output:
   * @self: the #RetroCore
   * @pixdata: (type RetroPixdata): the #RetroPixdata
   *
   * The ::video-output signal is emitted each time a new video frame is emitted
   * by the core.
   *
   * @pixdata will be invalid after the signal emission, copy it in some way if
   * you want to keep it.
   */
  signals[SIG_VIDEO_OUTPUT_SIGNAL] =
    g_signal_new ("video-output", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  1,
                  // G_TYPE_POINTER instead of RETRO_TYPE_PIXDATA to implicit
                  // copy when sending the RetroPixdata.
                  G_TYPE_POINTER);

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
   */
  signals[SIG_SHUTDOWN_SIGNAL] =
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
  signals[SIG_MESSAGE_SIGNAL] =
    g_signal_new ("message", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  2,
                  G_TYPE_STRING,
                  G_TYPE_UINT);

  /**
   * RetroCore::crashed:
   * @self: the #RetroCore
   * @message: the message to show to the user
   *
   * The ::crash signal is emitted when the core crashes.
   */
  signals[SIG_CRASHED_SIGNAL] =
    g_signal_new ("crashed", RETRO_TYPE_CORE, G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  1,
                  G_TYPE_STRING);
}

static void
retro_core_init (RetroCore *self)
{
  gint fd;

  self->options = g_hash_table_new_full (g_str_hash, g_str_equal,
                                         g_free, g_object_unref);
  self->option_overrides = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                  g_free, g_free);

  self->controllers = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL,
                                             (GDestroyNotify) free_controller_info);

  fd = retro_memfd_create ("[retro-runner default controller]");
  self->default_controller_state = retro_controller_state_new (fd);

  self->speed_rate = 1;
}

static void
retro_core_set_filename (RetroCore   *self,
                         const gchar *filename)
{
  if (g_strcmp0 (filename, retro_core_get_filename (self)) == 0)
    return;

  g_free (self->filename);
  self->filename = g_strdup (filename);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_FILENAME]);
}

static void
crash (RetroCore *self,
       GError    *error)
{
  g_signal_emit (self, signals[SIG_CRASHED_SIGNAL], 0, error->message);
}

static void
crash_or_propagate_error (RetroCore  *self,
                          GError     *error,
                          GError    **out_error)
{
  if (g_dbus_error_strip_remote_error (error)) {
    g_propagate_error (out_error, error);
    return;
  }

  crash (self, error);
}

static void
exit_cb (RetroRunnerProcess *process,
         gboolean            success,
         gchar              *error,
         RetroCore          *self)
{
  if (success)
    g_signal_emit (self, signals[SIG_SHUTDOWN_SIGNAL], 0);
  else
    g_signal_emit (self, signals[SIG_CRASHED_SIGNAL], 0, error);
}

static gboolean
key_event_cb (GtkWidget   *widget,
              GdkEventKey *event,
              RetroCore   *self)
{
  gboolean pressed;
  RetroKeyboardKey retro_key;
  RetroKeyboardModifierKey retro_modifier_key;
  guint32 character;
  g_autoptr(GError) error = NULL;
  IpcRunner *proxy;

  if (!retro_core_get_is_initiated (self))
    return FALSE;

  pressed = event->type == GDK_KEY_PRESS;
  retro_key = retro_keyboard_key_converter (event->hardware_keycode);
  retro_modifier_key = retro_keyboard_modifier_key_converter (event->keyval, event->state);
  character = gdk_keyval_to_unicode (event->keyval);

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_key_event_sync (proxy, pressed, retro_key,
                                       character, retro_modifier_key, NULL,
                                       &error))
    crash (self, error);

  return FALSE;
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
  IpcRunner *proxy;

  g_return_val_if_fail (RETRO_IS_CORE (self), 0);

  proxy = retro_runner_process_get_proxy (self->process);

  if (!proxy)
    return FALSE;

  return ipc_runner_get_api_version (proxy);
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

  return (retro_runner_process_get_proxy (self->process) != NULL);
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

static void
set_rumble_state_cb (IpcRunner         *runner,
                     guint              port,
                     RetroRumbleEffect  effect,
                     guint16            strength,
                     RetroCore         *self)
{
  RetroCoreControllerInfo *info;

  if (!g_hash_table_contains (self->controllers, GUINT_TO_POINTER (port)))
    return;

  info = g_hash_table_lookup (self->controllers, GUINT_TO_POINTER (port));

  if (info == NULL)
    return;

  retro_controller_set_rumble_state (info->controller, effect, strength);
}

static void
video_output_cb (IpcRunner *runner,
                 RetroCore *self)
{
  RetroPixdata pixdata;
  RetroPixelFormat pixel_format;
  gsize rowstride;
  guint width, height;
  gdouble aspect_ratio;
  gconstpointer pixels;

  retro_framebuffer_lock (self->framebuffer);

  if (!retro_framebuffer_get_is_dirty (self->framebuffer)) {
    retro_framebuffer_unlock (self->framebuffer);

    return;
  }

  rowstride = retro_framebuffer_get_rowstride (self->framebuffer);
  pixel_format = retro_framebuffer_get_format (self->framebuffer);
  width = retro_framebuffer_get_width (self->framebuffer);
  height = retro_framebuffer_get_height (self->framebuffer);
  aspect_ratio = retro_framebuffer_get_aspect_ratio (self->framebuffer);
  pixels = retro_framebuffer_get_pixels (self->framebuffer);

  retro_pixdata_init (&pixdata, pixels, pixel_format, rowstride,
                      width, height, aspect_ratio);

  g_signal_emit (self, signals[SIG_VIDEO_OUTPUT_SIGNAL], 0, &pixdata);

  retro_framebuffer_unlock (self->framebuffer);
}

static void
option_value_changed_cb (RetroOption *option,
                         RetroCore   *self)
{
  const gchar *key, *value;
  g_autoptr(GError) error = NULL;
  IpcRunner *proxy;

  key = retro_option_get_key (option);
  value = retro_option_get_value (option);

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_update_variable_sync (proxy, key, value, NULL, &error))
    crash (self, error);
}

static void
insert_variable (RetroCore   *self,
                 const gchar *key,
                 const gchar *value)
{
  RetroOption *option;
  GError *tmp_error = NULL;

  option = retro_option_new (key, value, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_debug ("%s", tmp_error->message);
    g_clear_error (&tmp_error);

    return;
  }

  if (g_hash_table_contains (self->option_overrides, key)) {
    gchar *override;

    override = g_hash_table_lookup (self->option_overrides, key);
    retro_option_set_value (option, override, &tmp_error);

    if (G_UNLIKELY (tmp_error != NULL)) {
      g_debug ("%s", tmp_error->message);
      g_clear_error (&tmp_error);
    }
  }

  g_hash_table_insert (self->options, g_strdup (key), option);

  g_signal_connect_object (option,
                           "value-changed",
                           G_CALLBACK (option_value_changed_cb),
                           self,
                           0);
}

static void
variables_set_cb (IpcRunner *runner,
                  GVariant  *data,
                  RetroCore *self)
{
  GVariantIter *iter;
  gchar *key, *value;

  g_variant_get (data, "a(ss)", &iter);

  while (g_variant_iter_loop (iter, "(ss)", &key, &value))
    insert_variable (self, key, value);

  g_variant_iter_free (iter);
}

static void
message_cb (IpcRunner   *runner,
            const gchar *message,
            guint        frames,
            RetroCore   *self)
{
  g_signal_emit (self, signals[SIG_MESSAGE_SIGNAL], 0, message, frames);
}

static void
log_cb (IpcRunner   *runner,
        const gchar *domain,
        guint        level,
        const gchar *message,
        RetroCore   *self)
{
  g_signal_emit (self, signals[SIG_LOG_SIGNAL], 0, domain, level, message);
}

static GVariant *
serialize_option_overrides (RetroCore *self)
{
  GVariantBuilder* builder;
  GHashTableIter iter;
  gpointer key, value;

  builder = g_variant_builder_new (G_VARIANT_TYPE ("a(ss)"));
  g_hash_table_iter_init (&iter, self->option_overrides);

  while (g_hash_table_iter_next (&iter, &key, &value))
    g_variant_builder_add (builder, "(ss)", key, value);

  return g_variant_builder_end (builder);
}

// FIXME Merge this into retro_core_set_controller().
static void
set_controller_port_device (RetroCore               *self,
                            guint                    port,
                            RetroControllerType      controller_type,
                            RetroCoreControllerInfo *info)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(GUnixFDList) fd_list = NULL;
  gint handle;
  IpcRunner *proxy;

  fd_list = g_unix_fd_list_new ();
  if (info) {
    gint fd = retro_controller_state_get_fd (info->state);
    handle = g_unix_fd_list_append (fd_list, fd, &error);
    if (handle == -1) {
      crash (self, error);
      return;
    }
  }
  else
    handle = -1;

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_set_controller_sync (proxy, port, controller_type,
                                            g_variant_new ("h", handle),
                                            fd_list, NULL, NULL, &error))
    crash (self, error);
}

static void
notify_api_version_cb (IpcRunner  *proxy,
                       GParamSpec *spec,
                       RetroCore  *self)
{
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_API_VERSION]);
}

static void
notify_game_loaded_cb (IpcRunner  *proxy,
                       GParamSpec *spec,
                       RetroCore  *self)
{
  self->game_loaded = ipc_runner_get_game_loaded (proxy);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_GAME_LOADED]);
}

static void
notify_frames_per_second_cb (IpcRunner  *proxy,
                             GParamSpec *spec,
                             RetroCore  *self)
{
  self->frames_per_second = ipc_runner_get_frames_per_second (proxy);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_FRAMES_PER_SECOND]);
}

static void
notify_support_no_game_cb (IpcRunner  *proxy,
                           GParamSpec *spec,
                           RetroCore  *self)
{
  self->support_no_game = ipc_runner_get_support_no_game (proxy);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SUPPORT_NO_GAME]);
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
  RetroControllerType controller_type;
  GHashTableIter iter;
  RetroCoreControllerInfo *info;
  g_autoptr (GPtrArray) medias_array = NULL;
  gint length, i;
  GError *tmp_error = NULL;
  IpcRunner *proxy;
  GVariant *variables;
  g_autoptr(GVariant) framebuffer_variant = NULL;
  g_autoptr(GUnixFDList) fd_list = NULL;
  g_autoptr(GUnixFDList) out_fd_list = NULL;
  gint fd, handle;

  g_return_if_fail (RETRO_IS_CORE (self));

  retro_runner_process_start (self->process, &tmp_error);
  if (tmp_error) {
    crash (self, tmp_error);
    return;
  }

  proxy = retro_runner_process_get_proxy (self->process);
  g_signal_connect_object (proxy, "variables-set", G_CALLBACK (variables_set_cb), self, 0);
  g_signal_connect_object (proxy, "message", G_CALLBACK (message_cb), self, 0);
  g_signal_connect_object (proxy, "log", G_CALLBACK (log_cb), self, 0);
  g_signal_connect_object (proxy, "video-output", G_CALLBACK (video_output_cb), self, 0);
  g_signal_connect_object (proxy, "set-rumble-state", G_CALLBACK (set_rumble_state_cb), self, 0);

  g_object_bind_property (self,  "system-directory",
                          proxy, "system-directory",
                          G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
  g_object_bind_property (self,  "content-directory",
                          proxy, "content-directory",
                          G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
  g_object_bind_property (self,  "save-directory",
                          proxy, "save-directory",
                          G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
  g_object_bind_property (self,  "speed-rate",
                          proxy, "speed-rate",
                          G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
  g_object_bind_property (self,  "runahead",
                          proxy, "runahead",
                          G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);

  medias_array = g_ptr_array_new ();
  if (self->media_uris) {
    length = g_strv_length (self->media_uris);
    for (i = 0; i < length; i++)
      g_ptr_array_add (medias_array, self->media_uris[i]);
  }
  g_ptr_array_add (medias_array, NULL);

  fd_list = g_unix_fd_list_new ();
  fd = retro_controller_state_get_fd (self->default_controller_state);
  handle = g_unix_fd_list_append (fd_list, fd, &tmp_error);
  if (handle == -1) {
    crash (self, tmp_error);
    return;
  }

  if (!ipc_runner_call_boot_sync (proxy,
                                  serialize_option_overrides (self),
                                  (const gchar * const *) medias_array->pdata,
                                  g_variant_new ("h", handle), fd_list,
                                  &variables,
                                  &framebuffer_variant, &out_fd_list,
                                  NULL, &tmp_error)) {
    crash_or_propagate_error (self, tmp_error, error);
    return;
  }

  g_variant_get (framebuffer_variant, "h", &handle);
  if (G_LIKELY (handle < g_unix_fd_list_get_length (out_fd_list))) {
    fd = g_unix_fd_list_get (out_fd_list, handle, &tmp_error);
    if (tmp_error) {
      crash (self, tmp_error);
      return;
    }
  } else {
    g_critical ("Invalid framebuffer handle");
    return;
  }

  self->framebuffer = retro_framebuffer_new (fd);

  g_hash_table_iter_init (&iter, self->controllers);
  while (g_hash_table_iter_next (&iter, NULL, (gpointer *) &info)) {
    controller_type = retro_controller_get_controller_type (info->controller);
    set_controller_port_device (self, info->port, controller_type, info);
  }

  if (!ipc_runner_call_get_properties_sync (proxy,
                                            &self->game_loaded,
                                            &self->frames_per_second,
                                            &self->support_no_game,
                                            NULL, &tmp_error)) {
    crash_or_propagate_error (self, tmp_error, error);
    return;
  }

  g_signal_connect_object (proxy, "notify::api-version", G_CALLBACK (notify_api_version_cb), self, 0);
  g_signal_connect_object (proxy, "notify::game-loaded", G_CALLBACK (notify_game_loaded_cb), self, 0);
  g_signal_connect_object (proxy, "notify::frames-per-second", G_CALLBACK (notify_frames_per_second_cb), self, 0);
  g_signal_connect_object (proxy, "notify::support-no-game", G_CALLBACK (notify_support_no_game_cb), self, 0);

  variables_set_cb (proxy, variables, self);

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_IS_INITIATED]);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_API_VERSION]);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_GAME_LOADED]);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_FRAMES_PER_SECOND]);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SUPPORT_NO_GAME]);
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
  GError *tmp_error = NULL;
  IpcRunner *proxy;
  guint length;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (retro_core_get_is_initiated (self));

  length = g_strv_length (self->media_uris);

  g_return_if_fail (media_index < length);

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_set_current_media_sync (proxy, media_index, NULL, &tmp_error))
    crash_or_propagate_error (self, tmp_error, error);
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
  g_autoptr(GError) error = NULL;
  IpcRunner *proxy;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (retro_core_get_is_initiated (self));

  if (self->speed_rate <= 0)
    return;

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_run_sync (proxy, NULL, &error))
    crash (self, error);
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
  g_autoptr(GError) error = NULL;
  IpcRunner *proxy;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (retro_core_get_is_initiated (self));

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_stop_sync (proxy, NULL, &error))
    crash (self, error);
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
  g_autoptr(GError) error = NULL;
  IpcRunner *proxy;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (retro_core_get_is_initiated (self));

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_reset_sync (proxy, NULL, &error))
    crash (self, error);
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
  g_autoptr(GError) error = NULL;
  IpcRunner *proxy;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (retro_core_get_is_initiated (self));

  proxy = retro_runner_process_get_proxy (self->process);

  if (!ipc_runner_call_iteration_sync (proxy, NULL, &error)) {
    crash (self, error);
    return;
  }

  /* Since this is sync API, we must ensure video is updated synchronously.
   * RetroFramebuffer already contains new data by this point, but the signal
   * emission won't arrive until later. To circumvent it, handle the video right
   * here and runner process will know not to emit the signal this time.
   * See usage of the block_video_signal field in retro-runner/ipc-runner-impl.c */
  video_output_cb (proxy, self);
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
  g_autoptr(GError) error = NULL;
  gboolean result;
  IpcRunner *proxy;

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);
  g_return_val_if_fail (retro_core_get_is_initiated (self), FALSE);

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_get_can_access_state_sync (proxy, &result, NULL, &error))
    crash (self, error);

  return result;
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
  GError *tmp_error = NULL;
  IpcRunner *proxy;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (filename != NULL);
  g_return_if_fail (retro_core_get_is_initiated (self));

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_save_state_sync (proxy, filename, NULL, &tmp_error))
    crash_or_propagate_error (self, tmp_error, error);
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
  GError *tmp_error = NULL;
  IpcRunner *proxy;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (filename != NULL);
  g_return_if_fail (retro_core_get_is_initiated (self));

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_load_state_sync (proxy, filename, NULL, &tmp_error))
    crash_or_propagate_error (self, tmp_error, error);
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
  g_autoptr(GError) error = NULL;
  gsize size;
  IpcRunner *proxy;

  g_return_val_if_fail (RETRO_IS_CORE (self), 0UL);
  g_return_val_if_fail (retro_core_get_is_initiated (self), 0UL);

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_get_memory_size_sync (proxy, memory_type, &size,
                                             NULL, &error))
    crash (self, error);

  return size;
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
  GError *tmp_error = NULL;
  IpcRunner *proxy;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (filename != NULL);
  g_return_if_fail (retro_core_get_is_initiated (self));

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_save_memory_sync (proxy, memory_type, filename, NULL, &tmp_error))
    crash_or_propagate_error (self, tmp_error, error);
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
  GError *tmp_error = NULL;
  IpcRunner *proxy;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (filename != NULL);
  g_return_if_fail (retro_core_get_is_initiated (self));

  proxy = retro_runner_process_get_proxy (self->process);
  if (!ipc_runner_call_load_memory_sync (proxy, memory_type, filename, NULL, &tmp_error))
    crash_or_propagate_error (self, tmp_error, error);
}

static void
sync_controller_for_type (RetroControllerState *state,
                          RetroController      *controller,
                          RetroControllerType   type)
{
  RetroInput input;
  g_autofree gint16 *data = NULL;
  gint id, index, max_id, max_index, next;

  if (!retro_controller_has_capability (controller, type))
    return;

  max_id = retro_controller_type_get_id_count (type);
  max_index = retro_controller_type_get_index_count (type);

  data = g_new (gint16, max_index * max_id);
  next = 0;

  for (index = 0; index < max_index; index++) {
    for (id = 0; id < max_id; id++) {
      retro_input_init (&input, type, id, index);
      data[next++] = retro_controller_get_input_state (controller, &input);
    }
  }

  retro_controller_state_set_for_type (state, type, data, max_index * max_id);
}

static void
default_controller_state_changed_cb (RetroController                *controller,
                                     RetroCoreDefaultControllerInfo *info)
{
  retro_controller_state_lock (info->state);
  sync_controller_for_type (info->state, controller, info->type);
  retro_controller_state_unlock (info->state);
}

static void
controller_state_changed_cb (RetroController         *controller,
                             RetroCoreControllerInfo *info)
{
  gint type;
  gboolean rumble = retro_controller_get_supports_rumble (info->controller);

  retro_controller_state_lock (info->state);

  retro_controller_state_set_supports_rumble (info->state, rumble);

  for (type = 1; type < RETRO_CONTROLLER_TYPE_COUNT; type++)
    if (retro_controller_has_capability (info->controller, type))
      sync_controller_for_type (info->state, info->controller, type);
    else
      retro_controller_state_clear_type (info->state, type);

  retro_controller_state_unlock (info->state);
}

/**
 * retro_core_set_default_controller:
 * @self: a #RetroCore
 * @controller_type: a #RetroControllerType
 * @controller: (nullable): a #RetroController
 *
 * Uses @controller as the default controller for the given type. When a port
 * has no controller plugged plugged into it, the core will use the default
 * controllers instead.
 */
void
retro_core_set_default_controller (RetroCore           *self,
                                   RetroControllerType  controller_type,
                                   RetroController     *controller)
{
  RetroCoreDefaultControllerInfo *info;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (controller_type < RETRO_CONTROLLER_TYPE_COUNT);
  g_return_if_fail (controller == NULL || RETRO_IS_CONTROLLER (controller));

  if (self->default_controllers[controller_type])
    free_default_controller_info (self->default_controllers[controller_type]);

  if (controller != NULL) {
    info = g_new0 (RetroCoreDefaultControllerInfo, 1);

    info->controller = g_object_ref (controller);
    info->type = controller_type;
    info->state = self->default_controller_state;
    info->state_changed_id =
      g_signal_connect (controller, "state-changed",
                        G_CALLBACK (default_controller_state_changed_cb), info);
    default_controller_state_changed_cb (info->controller, info);
  }
  else {
    info = NULL;
    retro_controller_state_lock (self->default_controller_state);
    retro_controller_state_clear_type (self->default_controller_state,
                                       controller_type);
    retro_controller_state_unlock (self->default_controller_state);
  }

  self->default_controllers[controller_type] = info;
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
  RetroControllerType controller_type;
  RetroCoreControllerInfo *info;

  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (controller == NULL || RETRO_IS_CONTROLLER (controller));

  if (controller != NULL) {
    info = g_new0 (RetroCoreControllerInfo, 1);
    gint fd;
    g_autofree gchar *name = NULL;

    name = g_strdup_printf ("[retro-runner controller %u]", port);
    fd = retro_memfd_create (name);

    info->controller = g_object_ref (controller);
    info->port = port;
    info->state = retro_controller_state_new (fd);
    info->state_changed_id =
      g_signal_connect (controller, "state-changed",
                        G_CALLBACK (controller_state_changed_cb), info);
    controller_state_changed_cb (info->controller, info);

    g_hash_table_insert (self->controllers, GUINT_TO_POINTER (port), info);
    controller_type = retro_controller_get_controller_type (controller);
  }
  else {
    info = NULL;
    g_hash_table_remove (self->controllers, GUINT_TO_POINTER (port));
    controller_type = RETRO_CONTROLLER_TYPE_NONE;
  }

  if (!retro_core_get_is_initiated (self))
    return;

  set_controller_port_device (self, port, controller_type, info);
}

static void
keyboard_widget_notify (RetroCore *self,
                        GObject   *keyboard_widget)
{
  self->keyboard_widget = NULL;
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
    g_object_weak_unref (G_OBJECT (self->keyboard_widget), (GWeakNotify) keyboard_widget_notify, self);
    self->keyboard_widget = NULL;
  }

  if (widget != NULL) {
    self->key_press_event_id =
      g_signal_connect_object (widget,
                               "key-press-event",
                               G_CALLBACK (key_event_cb),
                               self,
                               0);
    self->key_release_event_id =
      g_signal_connect_object (widget,
                               "key-release-event",
                               G_CALLBACK (key_event_cb),
                               self,
                               0);
    self->keyboard_widget = widget;
    g_object_weak_ref (G_OBJECT (widget), (GWeakNotify) keyboard_widget_notify, self);
  }
}

static RetroController *
get_controller (gpointer value)
{
  RetroCoreControllerInfo *info = value;

  return info->controller;
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

  return retro_controller_iterator_new (self->controllers, get_controller);
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

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_RUNAHEAD]);
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
}

/**
 * retro_core_has_option:
 * @self: a #RetroCore
 * @key: the key of the option
 *
 * Gets whether the core has an option for the given key.
 *
 * Returns: whether the core has an option for the given key
 */
gboolean
retro_core_has_option (RetroCore   *self,
                       const gchar *key)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);
  g_return_val_if_fail (key != NULL, FALSE);

  return g_hash_table_contains (self->options, key);
}

/**
 * retro_core_get_option:
 * @self: a #RetroCore
 * @key: the key of the option
 *
 * Gets the option for the given key.
 *
 * Returns: (transfer none): the option
 */
RetroOption *
retro_core_get_option (RetroCore    *self,
                       const gchar  *key)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  return RETRO_OPTION (g_hash_table_lookup (self->options, key));
}

/**
 * retro_core_iterate_options:
 * @self: a #RetroCore
 *
 * Creates a new #RetroOptionIterator which can be used to iterate through the
 * options of @self.
 *
 * Returns: (transfer full): a new #RetroOptionIterator
 */
RetroOptionIterator *
retro_core_iterate_options (RetroCore *self)
{
  g_return_val_if_fail (RETRO_IS_CORE (self), NULL);

  return retro_option_iterator_new (self->options);
}

/**
 * retro_core_override_option_default:
 * @self: a #RetroCore
 * @key: the key of the option
 * @value: the default value
 *
 * Overrides default value for the option @key. This can be used to set value
 * for a startup-only option.
 *
 * You can use this before booting the core.
 */
void
retro_core_override_option_default (RetroCore   *self,
                                    const gchar *key,
                                    const gchar *value)
{
  g_return_if_fail (RETRO_IS_CORE (self));
  g_return_if_fail (key != NULL);
  g_return_if_fail (value != NULL);
  g_return_if_fail (!retro_core_get_is_initiated (self));

  g_hash_table_replace (self->option_overrides, g_strdup (key), g_strdup (value));
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

  return g_object_new (RETRO_TYPE_CORE, "filename", filename, NULL);
}
