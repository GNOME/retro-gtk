// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#include <glib-object.h>

#include "ipc-runner-private.h"

G_BEGIN_DECLS

#define RETRO_TYPE_RUNNER_PROCESS (retro_runner_process_get_type())

G_DECLARE_FINAL_TYPE (RetroRunnerProcess, retro_runner_process, RETRO, RUNNER_PROCESS, GObject)

RetroRunnerProcess *retro_runner_process_new (const gchar *filename) G_GNUC_WARN_UNUSED_RESULT;

void retro_runner_process_start (RetroRunnerProcess  *self,
                                 GError             **error);
IpcRunner *retro_runner_process_get_proxy (RetroRunnerProcess *self);
void retro_runner_process_stop (RetroRunnerProcess  *self,
                                GError             **error);

G_END_DECLS
