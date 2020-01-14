// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-input.h"

G_BEGIN_DECLS

#define RETRO_TYPE_CONTROLLER_STATE (retro_controller_state_get_type())

G_DECLARE_FINAL_TYPE (RetroControllerState, retro_controller_state, RETRO, CONTROLLER_STATE, GObject)

RetroControllerState *retro_controller_state_new (gint fd);

gint retro_controller_state_get_fd (RetroControllerState *self);

void retro_controller_state_lock (RetroControllerState *self);
void retro_controller_state_unlock (RetroControllerState *self);

#ifdef RETRO_RUNNER_COMPILATION

gboolean retro_controller_state_has_type (RetroControllerState *self,
                                          RetroControllerType   type);
gint16 retro_controller_state_get_input (RetroControllerState *self,
                                         RetroInput           *input);

gboolean retro_controller_state_get_supports_rumble (RetroControllerState *self);

void retro_controller_state_snapshot (RetroControllerState *self);

#else

void retro_controller_state_set_for_type (RetroControllerState *self,
                                          RetroControllerType   type,
                                          gint16               *state,
                                          gsize                 n_items);
void retro_controller_state_clear_type (RetroControllerState *self,
                                        RetroControllerType   type);

void retro_controller_state_set_supports_rumble (RetroControllerState *self,
                                                 gboolean              supports_rumble);

#endif

G_END_DECLS
