// This file is part of retro-gtk. License: GPL-3.0+.

#include <glib-2.0/glib.h>
#include <glib-unix.h>
#include <gio/gunixconnection.h>

#ifdef __linux__
#include <sys/prctl.h>
#endif

#ifdef __FreeBSD__
#include <sys/procctl.h>
#endif

#include "ipc-runner-impl-private.h"
#include "retro-pa-player-private.h"

static gboolean
run_main_loop (GMainLoop        *loop,
               GDBusConnection  *connection,
               const gchar      *filename,
               GError          **error)
{
  g_autoptr(IpcRunnerImpl) runner = NULL;
  RetroCore *core;

  core = retro_core_new (filename);
  runner = ipc_runner_impl_new (core);
  g_signal_connect_swapped (core, "shutdown", G_CALLBACK (g_main_loop_quit), loop);

  if (!g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (runner),
                                         connection,
                                         "/org/gnome/Retro/Runner",
                                         error))
    return FALSE;

  g_dbus_connection_start_message_processing (connection);

  g_debug ("Running main loop");

  g_main_loop_run (loop);

  return TRUE;
}

/* Adapted from GNOME Builder's gnome-builder-git.c */
gint
main (gint    argc,
      gchar **argv)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(GMainLoop) loop = NULL;
  g_autoptr(GSocket) socket = NULL;
  g_autoptr(GIOStream) stream = NULL;
  g_autoptr(GDBusConnection) connection = NULL;
  g_autofree gchar *guid = NULL;

  /* Arguments: application name, core filename */
  g_assert (argc >= 3);

  g_set_prgname ("retro-runner");
  g_set_application_name (argv[1]);

#ifdef __linux__
  prctl (PR_SET_PDEATHSIG, SIGTERM);
#elif defined(__FreeBSD__)
  procctl (P_PID, 0, PROC_PDEATHSIG_CTL, &(int){ SIGTERM });
#else
#error "Please submit a patch to support parent-death signal on your OS"
#endif

  loop = g_main_loop_new (NULL, FALSE);

  g_debug ("Starting runner process");

  /* The file descriptor 3 is passed from the parent process */
  if (!g_unix_set_fd_nonblocking (3, TRUE, &error))
    goto error;

  socket = g_socket_new_from_fd (3, &error);
  stream = G_IO_STREAM (g_socket_connection_factory_create_connection (socket));

  g_assert (G_IS_UNIX_CONNECTION (stream));

  guid = g_dbus_generate_guid ();
  connection = g_dbus_connection_new_sync (stream, guid,
                                           G_DBUS_CONNECTION_FLAGS_DELAY_MESSAGE_PROCESSING |
                                           G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_SERVER,
                                           NULL, NULL, &error);
  if (!connection)
    goto error;

  g_debug ("Connected");

  g_dbus_connection_set_exit_on_close (connection, FALSE);
  g_signal_connect_swapped (connection, "closed", G_CALLBACK (g_main_loop_quit), loop);

  if (!run_main_loop (loop, connection, argv[2], &error))
    goto error;

  g_debug ("Stopping runner process");

  return EXIT_SUCCESS;

error:
  if (error)
    g_critical ("Couldn't initialize runner process: %s", error->message);

  return EXIT_FAILURE;
}
