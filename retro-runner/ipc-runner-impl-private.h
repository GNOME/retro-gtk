// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#include "ipc-runner.h"
#include "retro-core.h"
#include "retro-variable-private.h"
#include "retro-pixel-format-private.h"
#include "retro-rumble-effect.h"

G_BEGIN_DECLS

#define IPC_TYPE_RUNNER_IMPL (ipc_runner_impl_get_type())

G_DECLARE_FINAL_TYPE (IpcRunnerImpl, ipc_runner_impl, IPC, RUNNER_IMPL, IpcRunnerSkeleton)

IpcRunnerImpl *ipc_runner_impl_new (RetroCore *core);

G_END_DECLS
