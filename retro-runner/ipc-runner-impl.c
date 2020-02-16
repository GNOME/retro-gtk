// This file is part of retro-gtk. License: GPL-3.0+.

#include "ipc-runner-impl-private.h"

#include <errno.h>
#include <sys/mman.h>
#include <gio/gunixfdlist.h>
#include "retro-core-private.h"
#include "retro-keyboard-key-private.h"
#ifdef PULSEAUDIO_ENABLED
#include "retro-pa-player-private.h"
#endif

struct _IpcRunnerImpl
{
  IpcRunnerSkeleton parent_instance;

  RetroCore *core;
#ifdef PULSEAUDIO_ENABLED
  RetroPaPlayer *audio_player;
#endif

  GVariant *variables;
  gboolean block_video_signal;
};

static void ipc_runner_iface_init (IpcRunnerIface *iface);

G_DEFINE_TYPE_WITH_CODE (IpcRunnerImpl, ipc_runner_impl, IPC_TYPE_RUNNER_SKELETON,
                         G_IMPLEMENT_INTERFACE (IPC_TYPE_RUNNER, ipc_runner_iface_init))

enum {
  PROP_0,
  PROP_CORE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static gboolean
ipc_runner_impl_handle_boot (IpcRunner             *runner,
                             GDBusMethodInvocation *invocation,
                             GUnixFDList           *fd_list,
                             GVariant              *defaults,
                             const gchar * const   *medias,
                             GVariant              *default_controller)
{
  IpcRunnerImpl *self;
  g_autoptr(GError) error = NULL;
  g_autoptr(GUnixFDList) out_fd_list = NULL;
  GVariantIter *iter;
  gchar *key, *value;
  gint handle, fd;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  g_variant_get (defaults, "a(ss)", &iter);

  while (g_variant_iter_loop (iter, "(ss)", &key, &value))
    retro_core_override_variable_default (self->core, key, value);

  g_variant_iter_free (iter);

  retro_core_set_medias (self->core, medias);

  g_variant_get (default_controller, "h", &handle);
  if (G_LIKELY (handle < g_unix_fd_list_get_length (fd_list))) {
    fd = g_unix_fd_list_get (fd_list, handle, &error);
    if (error) {
      g_dbus_method_invocation_return_gerror (g_steal_pointer (&invocation), error);

      return TRUE;
    }
  } else {
    g_dbus_method_invocation_return_error (g_steal_pointer (&invocation),
                                           G_DBUS_ERROR,
                                           G_DBUS_ERROR_INVALID_ARGS,
                                           "Invalid FD handle value");

    return TRUE;
  }

  retro_core_set_default_controller (self->core, fd);
  retro_core_boot (self->core, &error);
  if (error) {
    g_dbus_method_invocation_return_gerror (g_steal_pointer (&invocation), error);
    g_variant_unref (self->variables);

    return TRUE;
  }

  /* DBus doesn't support nulls, so create an empty array instead */
  if (!self->variables) {
    GVariantBuilder* builder;

    builder = g_variant_builder_new (G_VARIANT_TYPE ("a(ss)"));

    self->variables = g_variant_ref_sink (g_variant_builder_end (builder));
  }

  out_fd_list = g_unix_fd_list_new ();
  fd = retro_core_get_framebuffer_fd (self->core);
  handle = g_unix_fd_list_append (out_fd_list, fd, &error);
  if (error) {
    g_dbus_method_invocation_return_gerror (g_steal_pointer (&invocation), error);
    g_variant_unref (self->variables);

    return TRUE;
  }

  ipc_runner_complete_boot (runner, invocation, out_fd_list,
                            self->variables, g_variant_new ("h", handle));

  g_variant_unref (self->variables);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_set_current_media (IpcRunner             *runner,
                                          GDBusMethodInvocation *invocation,
                                          uint                   index)
{
  IpcRunnerImpl *self;
  g_autoptr(GError) error = NULL;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  retro_core_set_current_media (self->core, index, &error);
  if (error) {
    g_dbus_method_invocation_return_gerror (g_steal_pointer (&invocation), error);

    return TRUE;
  }

  ipc_runner_complete_set_current_media (runner, invocation);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_run (IpcRunner             *runner,
                            GDBusMethodInvocation *invocation)
{
  IpcRunnerImpl *self;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  retro_core_run (self->core);

  ipc_runner_complete_run (runner, invocation);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_stop (IpcRunner             *runner,
                             GDBusMethodInvocation *invocation)
{
  IpcRunnerImpl *self;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  retro_core_stop (self->core);

  ipc_runner_complete_stop (runner, invocation);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_reset (IpcRunner             *runner,
                              GDBusMethodInvocation *invocation)
{
  IpcRunnerImpl *self;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  retro_core_reset (self->core);

  ipc_runner_complete_reset (runner, invocation);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_iteration (IpcRunner             *runner,
                                  GDBusMethodInvocation *invocation)
{
  IpcRunnerImpl *self;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  /* For this call UI process will do the video handling itself
   * to ensure it's synchronous, no signal emission needed.
   * See retro_core_iteration() in retro-core/retro-core.c */
  self->block_video_signal = TRUE;
  retro_core_iteration (self->core);
  self->block_video_signal = FALSE;

  ipc_runner_complete_iteration (runner, invocation);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_get_can_access_state (IpcRunner             *runner,
                                             GDBusMethodInvocation *invocation)
{
  IpcRunnerImpl *self;
  gboolean can_access_state;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  can_access_state = retro_core_get_can_access_state (self->core);

  ipc_runner_complete_get_can_access_state (runner, invocation, can_access_state);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_save_state (IpcRunner             *runner,
                                   GDBusMethodInvocation *invocation,
                                   const gchar           *filename)
{
  IpcRunnerImpl *self;
  g_autoptr(GError) error = NULL;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  retro_core_save_state (self->core, filename, &error);
  if (error) {
    g_dbus_method_invocation_return_gerror (g_steal_pointer (&invocation), error);

    return TRUE;
  }

  ipc_runner_complete_save_state (runner, invocation);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_load_state (IpcRunner             *runner,
                                   GDBusMethodInvocation *invocation,
                                   const gchar           *filename)
{
  IpcRunnerImpl *self;
  g_autoptr(GError) error = NULL;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  retro_core_load_state (self->core, filename, &error);

  if (error) {
    g_dbus_method_invocation_return_gerror (g_steal_pointer (&invocation), error);

    return TRUE;
  }

  ipc_runner_complete_load_state (runner, invocation);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_get_memory_size (IpcRunner             *runner,
                                        GDBusMethodInvocation *invocation,
                                        RetroMemoryType        memory_type)
{
  IpcRunnerImpl *self;
  gsize memory_size;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  memory_size = retro_core_get_memory_size (self->core, memory_type);

  ipc_runner_complete_get_memory_size (runner, invocation, memory_size);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_save_memory (IpcRunner             *runner,
                                    GDBusMethodInvocation *invocation,
                                    RetroMemoryType        memory_type,
                                    const gchar           *filename)
{
  IpcRunnerImpl *self;
  g_autoptr(GError) error = NULL;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  retro_core_save_memory (self->core, memory_type, filename, &error);

  if (error) {
    g_dbus_method_invocation_return_gerror (g_steal_pointer (&invocation), error);

    return TRUE;
  }

  ipc_runner_complete_save_memory (runner, invocation);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_load_memory (IpcRunner             *runner,
                                    GDBusMethodInvocation *invocation,
                                    RetroMemoryType        memory_type,
                                    const gchar           *filename)
{
  IpcRunnerImpl *self;
  g_autoptr(GError) error = NULL;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  retro_core_load_memory (self->core, memory_type, filename, &error);

  if (error) {
    g_dbus_method_invocation_return_gerror (g_steal_pointer (&invocation), error);

    return TRUE;
  }

  ipc_runner_complete_load_memory (runner, invocation);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_update_variable (IpcRunner             *runner,
                                        GDBusMethodInvocation *invocation,
                                        const gchar           *key,
                                        const gchar           *value)
{
  IpcRunnerImpl *self;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  retro_core_update_variable (self->core, key, value);

  ipc_runner_complete_update_variable (runner, invocation);

  return TRUE;
}

static gboolean
ipc_runner_handle_set_controller (IpcRunner             *runner,
                                  GDBusMethodInvocation *invocation,
                                  GUnixFDList           *fd_list,
                                  guint                  port,
                                  RetroControllerType    type,
                                  GVariant              *data_handle)
{
  IpcRunnerImpl *self;
  gint fd;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  if (type != RETRO_CONTROLLER_TYPE_NONE) {
    g_autoptr(GError) error = NULL;
    gint handle;

    g_variant_get (data_handle, "h", &handle);
    if (G_LIKELY (handle < g_unix_fd_list_get_length (fd_list))) {
      fd = g_unix_fd_list_get (fd_list, handle, &error);
      if (error) {
        g_dbus_method_invocation_return_gerror (g_steal_pointer (&invocation), error);

        return TRUE;
      }
    } else {
      g_dbus_method_invocation_return_error (g_steal_pointer (&invocation),
                                             G_DBUS_ERROR,
                                             G_DBUS_ERROR_INVALID_ARGS,
                                             "Invalid FD handle value");

      return TRUE;
    }
  }
  else
    fd = -1;

  retro_core_set_controller (self->core, port, type, fd);

  ipc_runner_complete_set_controller (runner, invocation, NULL);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_key_event (IpcRunner                *runner,
                                  GDBusMethodInvocation    *invocation,
                                  gboolean                  pressed,
                                  RetroKeyboardKey          keycode,
                                  guint32                   character,
                                  RetroKeyboardModifierKey  modifiers)
{
  IpcRunnerImpl *self;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  retro_core_send_input_key_event (self->core, pressed, keycode, character, modifiers);

  ipc_runner_complete_key_event (runner, invocation);

  return TRUE;
}

static gboolean
ipc_runner_impl_handle_get_properties (IpcRunner             *runner,
                                        GDBusMethodInvocation *invocation)
{
  IpcRunnerImpl *self;

  g_return_val_if_fail (IPC_IS_RUNNER_IMPL (runner), FALSE);
  g_return_val_if_fail (G_IS_DBUS_METHOD_INVOCATION (invocation), FALSE);

  self = IPC_RUNNER_IMPL (runner);

  ipc_runner_complete_get_properties (runner, invocation,
                                      retro_core_get_game_loaded (self->core),
                                      retro_core_get_frames_per_second (self->core),
                                      retro_core_get_support_no_game (self->core));

  return TRUE;
}

static void
ipc_runner_impl_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  IpcRunnerImpl *self = IPC_RUNNER_IMPL (object);

  switch (prop_id) {
  case PROP_CORE:
    g_value_set_object (value, self->core);

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
ipc_runner_impl_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  IpcRunnerImpl *self = IPC_RUNNER_IMPL (object);

  switch (prop_id) {
  case PROP_CORE:
    self->core = g_value_get_object (value);

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
message_cb (RetroCore     *core,
            const gchar   *message,
            guint          frames,
            IpcRunnerImpl *self)
{
  ipc_runner_emit_message (IPC_RUNNER (self), message, frames);
}

static void
video_output_cb (RetroCore     *core,
                 IpcRunnerImpl *self)
{
  if (!self->block_video_signal)
    ipc_runner_emit_video_output (IPC_RUNNER (self));
}

static void
log_cb (RetroCore      *core,
        const gchar    *domain,
        GLogLevelFlags  level,
        const gchar    *message,
        IpcRunnerImpl  *self)
{
  ipc_runner_emit_log (IPC_RUNNER (self), domain, level, message);
}

static void
variables_set_cb (RetroCore     *core,
                  RetroVariable *variables,
                  IpcRunnerImpl *self)
{
  gint i;
  GVariantBuilder* builder;

  builder = g_variant_builder_new (G_VARIANT_TYPE ("a(ss)"));

  for (i = 0; variables[i].key && variables[i].value; i++)
    g_variant_builder_add (builder, "(ss)", variables[i].key, variables[i].value);

  if (retro_core_get_is_initiated (self->core))
    ipc_runner_emit_variables_set (IPC_RUNNER (self),
                                   g_variant_builder_end (builder));
  else
    self->variables = g_variant_ref_sink (g_variant_builder_end (builder));
}

static void
set_rumble_state_cb (RetroCore         *core,
                     guint              port,
                     RetroRumbleEffect  effect,
                     guint16            strength,
                     IpcRunnerImpl     *self)
{
  ipc_runner_emit_set_rumble_state (IPC_RUNNER (self), port, effect, strength);
}

static void
ipc_runner_impl_constructed (GObject *object)
{
  IpcRunnerImpl *self = (IpcRunnerImpl *)object;

#ifdef PULSEAUDIO_ENABLED
  self->audio_player = retro_pa_player_new ();
  retro_pa_player_set_core (self->audio_player, self->core);
#endif

  g_object_bind_property (self->core, "api-version",
                          self,       "api-version",
                          G_BINDING_SYNC_CREATE);
  g_object_bind_property (self->core, "system-directory",
                          self,       "system-directory",
                          G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
  g_object_bind_property (self->core, "content-directory",
                          self,       "content-directory",
                          G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
  g_object_bind_property (self->core, "save-directory",
                          self,       "save-directory",
                          G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
  g_object_bind_property (self->core, "game-loaded",
                          self,       "game-loaded",
                          G_BINDING_SYNC_CREATE);
  g_object_bind_property (self->core, "frames-per-second",
                          self,       "frames-per-second",
                          G_BINDING_SYNC_CREATE);
  g_object_bind_property (self->core, "support-no-game",
                          self,       "support-no-game",
                          G_BINDING_SYNC_CREATE);
  g_object_bind_property (self->core, "speed-rate",
                          self,       "speed-rate",
                          G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);
  g_object_bind_property (self->core, "runahead",
                          self,       "runahead",
                          G_BINDING_SYNC_CREATE | G_BINDING_BIDIRECTIONAL);

  g_signal_connect (self->core, "message",
                    G_CALLBACK (message_cb), self);
  g_signal_connect (self->core, "video-output",
                    G_CALLBACK (video_output_cb), self);
  g_signal_connect (self->core, "log",
                    G_CALLBACK (log_cb), self);
  g_signal_connect (self->core, "variables-set",
                    G_CALLBACK (variables_set_cb), self);
  g_signal_connect (self->core, "set-rumble-state",
                    G_CALLBACK (set_rumble_state_cb), self);

  G_OBJECT_CLASS (ipc_runner_impl_parent_class)->constructed (object);
}

static void
ipc_runner_impl_finalize (GObject *object)
{
  IpcRunnerImpl *self = (IpcRunnerImpl *)object;

  g_signal_handlers_disconnect_by_data (self->core, self);

  g_object_unref (self->core);
#ifdef PULSEAUDIO_ENABLED
  g_object_unref (self->audio_player);
#endif

  G_OBJECT_CLASS (ipc_runner_impl_parent_class)->finalize (object);
}

static void
ipc_runner_iface_init (IpcRunnerIface *iface)
{
  iface->handle_boot = ipc_runner_impl_handle_boot;
  iface->handle_set_current_media = ipc_runner_impl_handle_set_current_media;

  iface->handle_run = ipc_runner_impl_handle_run;
  iface->handle_stop = ipc_runner_impl_handle_stop;
  iface->handle_reset = ipc_runner_impl_handle_reset;
  iface->handle_iteration = ipc_runner_impl_handle_iteration;

  iface->handle_get_can_access_state = ipc_runner_impl_handle_get_can_access_state;
  iface->handle_save_state = ipc_runner_impl_handle_save_state;
  iface->handle_load_state = ipc_runner_impl_handle_load_state;
  iface->handle_get_memory_size = ipc_runner_impl_handle_get_memory_size;
  iface->handle_save_memory = ipc_runner_impl_handle_save_memory;
  iface->handle_load_memory = ipc_runner_impl_handle_load_memory;

  iface->handle_update_variable = ipc_runner_impl_handle_update_variable;

  iface->handle_set_controller = ipc_runner_handle_set_controller;
  iface->handle_key_event = ipc_runner_impl_handle_key_event;

  iface->handle_get_properties = ipc_runner_impl_handle_get_properties;
}

static void
ipc_runner_impl_class_init (IpcRunnerImplClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = ipc_runner_impl_constructed;
  object_class->finalize = ipc_runner_impl_finalize;
  object_class->get_property = ipc_runner_impl_get_property;
  object_class->set_property = ipc_runner_impl_set_property;

  properties [PROP_CORE] =
    g_param_spec_object ("core",
                         "Core",
                         "Core",
                         RETRO_TYPE_CORE,
                         (G_PARAM_READWRITE |
                          G_PARAM_CONSTRUCT_ONLY |
                          G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
ipc_runner_impl_init (IpcRunnerImpl *self)
{
}

IpcRunnerImpl *
ipc_runner_impl_new (RetroCore *core)
{
  g_return_val_if_fail (RETRO_IS_CORE (core), NULL);

  return g_object_new (IPC_TYPE_RUNNER_IMPL, "core", core, NULL);
}
