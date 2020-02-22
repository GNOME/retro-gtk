// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-runner-process-private.h"

#include "../retro-gtk-config.h"

#include <glib-unix.h>
#include <gio/gunixconnection.h>
#include <sys/socket.h>

struct _RetroRunnerProcess
{
  GObject parent_instance;

  GDBusConnection *connection;
  GCancellable *cancellable;
  IpcRunner *proxy;
  gchar *filename;
};

enum {
  PROP_0,
  PROP_FILENAME,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

enum {
  SIGNAL_EXIT,
  N_SIGNALS,
};

static guint signals [N_SIGNALS];

G_DEFINE_TYPE (RetroRunnerProcess, retro_runner_process, G_TYPE_OBJECT)


static void
retro_runner_process_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  RetroRunnerProcess *self = RETRO_RUNNER_PROCESS (object);

  switch (prop_id) {
  case PROP_FILENAME:
    g_value_set_string (value, self->filename);

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_runner_process_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  RetroRunnerProcess *self = RETRO_RUNNER_PROCESS (object);

  switch (prop_id) {
  case PROP_FILENAME:
    self->filename = g_strdup (g_value_get_string (value));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_runner_process_dispose (GObject *object)
{
  RetroRunnerProcess *self = (RetroRunnerProcess *)object;

  if (self->connection)
    retro_runner_process_stop (self, NULL);

  G_OBJECT_CLASS (retro_runner_process_parent_class)->dispose (object);
}

static void
retro_runner_process_finalize (GObject *object)
{
  RetroRunnerProcess *self = (RetroRunnerProcess *)object;

  g_free (self->filename);

  G_OBJECT_CLASS (retro_runner_process_parent_class)->finalize (object);
}

static void
retro_runner_process_class_init (RetroRunnerProcessClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = retro_runner_process_dispose;
  object_class->finalize = retro_runner_process_finalize;
  object_class->get_property = retro_runner_process_get_property;
  object_class->set_property = retro_runner_process_set_property;

  /**
   * RetroRunnerProcess:filename:
   *
   * The filename of the core to run remotely.
   */
  properties [PROP_FILENAME] =
    g_param_spec_string ("filename",
                         "Filename",
                         "Filename",
                         NULL,
                         (G_PARAM_READWRITE |
                          G_PARAM_CONSTRUCT_ONLY |
                          G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  /**
   * RetroRunnerProcess::exit:
   * @self: the #RetroRunnerProcess
   * @success: whether the runner process stopped successfully
   * @message: the message to show to the user, or %NULL if @success is %TRUE
   *
   * The ::exit signal is emitted when the runner process exits.
   */
  signals [SIGNAL_EXIT] =
    g_signal_new ("exit",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL,
                  G_TYPE_NONE,
                  2, G_TYPE_BOOLEAN, G_TYPE_STRING);
}

static void
retro_runner_process_init (RetroRunnerProcess *self)
{
}

static void
wait_check_cb (GSubprocess        *process,
               GAsyncResult       *result,
               RetroRunnerProcess *self)
{
  gboolean success;
  g_autoptr(GError) error = NULL;

  success = g_subprocess_wait_check_finish (process, result, &error);

  /* Don't do anything since self might have been already finalized */
  if (error && g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
    return;

  g_clear_object (&self->proxy);
  g_clear_object (&self->connection);
  g_clear_object (&self->cancellable);

  if (!success && error) {
    g_warning ("Subprocess stopped unexpectedly: %s", error->message);
    g_signal_emit (self, signals[SIGNAL_EXIT], 0, FALSE, error->message);
  } else
    g_signal_emit (self, signals[SIGNAL_EXIT], 0, TRUE, NULL);
}

/**
 * retro_runner_process_get_proxy:
 * @self: a #RetroRunnerProcess
 *
 * Retrieves the gdbus-codegen proxy with interacting with the runner process.
 *
 * Returns: A proxy for interacting with the remote process, or %NULL
 */
IpcRunner *
retro_runner_process_get_proxy (RetroRunnerProcess *self)
{
  g_return_val_if_fail (RETRO_IS_RUNNER_PROCESS (self), NULL);

  return self->proxy;
}

static GSocketConnection *
create_connection (GSubprocessLauncher  *launcher,
                   gint                  subprocess_fd,
                   GError              **error)
{
  g_autoptr (GSocket) socket = NULL;
  gint status, sv[2];
  GSocketConnection *connection;

  status = socketpair (PF_UNIX, SOCK_STREAM, 0, sv);
  if (status != 0)
    return NULL;

  if (!g_unix_set_fd_nonblocking (sv[0], TRUE, error) ||
      !g_unix_set_fd_nonblocking (sv[1], TRUE, error))
    return NULL;

  g_subprocess_launcher_take_fd (launcher, sv[1], subprocess_fd);

  socket = g_socket_new_from_fd (sv[0], error);
  if (!socket)
    return NULL;

  connection = g_socket_connection_factory_create_connection (socket);
  if (!connection)
    return NULL;

  g_assert (G_IS_UNIX_CONNECTION (connection));

  return connection;
}

static gboolean
is_debug (void)
{
  gchar **envp;
  const gchar *env_value;
  gboolean result = FALSE;

  envp = g_get_environ ();
  env_value = g_environ_getenv (envp, "RETRO_DEBUG");

  result = (g_strcmp0 ("1", env_value) == 0);

  g_strfreev (envp);

  return result;
}

/**
 * retro_runner_process_start:
 * @self: a #RetroRunnerProcess
 * @error: return location for a #GError, or %NULL
 *
 * Starts the remote process.
 */
/* Adapted from GNOME Builder's gbp-git-client.c */
void
retro_runner_process_start (RetroRunnerProcess  *self,
                            GError             **error)
{
  g_autoptr(GSocketConnection) connection = NULL;
  g_autoptr(GSubprocessLauncher) launcher = NULL;
  g_autoptr(GSubprocess) process = NULL;
  GError *tmp_error = NULL;

  g_return_if_fail (RETRO_IS_RUNNER_PROCESS (self));
  g_return_if_fail (!G_IS_DBUS_CONNECTION (self->connection));

  launcher = g_subprocess_launcher_new (G_SUBPROCESS_FLAGS_NONE);

  if (!(connection = create_connection (launcher, 3, error)))
    return;

  if (is_debug ()) {
    if (!(process = g_subprocess_launcher_spawn (launcher, &tmp_error,
                                                 "gdb", "-batch", "-ex", "run",
                                                 "-ex", "bt", "--args",
                                                 RETRO_RUNNER_PATH,
                                                 g_get_application_name (),
                                                 self->filename, NULL))) {
      g_propagate_error (error, tmp_error);
      return;
    }
  } else {
    if (!(process = g_subprocess_launcher_spawn (launcher, &tmp_error,
                                                 RETRO_RUNNER_PATH,
                                                 g_get_application_name (),
                                                 self->filename, NULL))) {
      g_propagate_error (error, tmp_error);
      return;
    }
  }

  if (!(self->connection = g_dbus_connection_new_sync (G_IO_STREAM (connection),
                                                       NULL,
                                                       G_DBUS_CONNECTION_FLAGS_DELAY_MESSAGE_PROCESSING |
                                                       G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
                                                       NULL, NULL, &tmp_error))) {
    g_propagate_error (error, tmp_error);
    return;
  }

  g_dbus_connection_start_message_processing (self->connection);

  self->cancellable = g_cancellable_new ();
  g_subprocess_wait_check_async (process, self->cancellable,
                                 (GAsyncReadyCallback) wait_check_cb, self);

  self->proxy = ipc_runner_proxy_new_sync (self->connection, 0, NULL,
                                           "/org/gnome/Retro/Runner", NULL,
                                           &tmp_error);
  if (!self->proxy)
    g_propagate_error (error, tmp_error);
}

/**
 * retro_runner_process_stop:
 * @self: a #RetroRunnerProcess
 * @error: return location for a #GError, or %NULL
 *
 * Stops the remote process.
 */
void
retro_runner_process_stop (RetroRunnerProcess  *self,
                           GError             **error)
{
  GError *tmp_error = NULL;

  g_return_if_fail (RETRO_IS_RUNNER_PROCESS (self));
  g_return_if_fail (G_IS_DBUS_CONNECTION (self->connection));

  g_cancellable_cancel (self->cancellable);

  if (!g_dbus_connection_close_sync (self->connection, NULL, &tmp_error))
    g_propagate_error (error, tmp_error);

  g_clear_object (&self->proxy);
  g_clear_object (&self->connection);
  g_clear_object (&self->cancellable);
}

/**
 * retro_runner_process_new:
 * @filename: the filename of a Libretro core
 *
 * Creates a new #RetroRunnerProcess.
 *
 * Returns: (transfer full): a new #RetroRunnerProcess
 */
RetroRunnerProcess *
retro_runner_process_new (const gchar *filename)
{
  g_return_val_if_fail (filename != NULL, NULL);

  return g_object_new (RETRO_TYPE_RUNNER_PROCESS, "filename", filename, NULL);
}
