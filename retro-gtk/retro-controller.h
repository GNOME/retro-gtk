// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-input.h"
#include "retro-rumble-effect.h"

G_BEGIN_DECLS

#define RETRO_TYPE_CONTROLLER (retro_controller_get_type())

G_DECLARE_INTERFACE (RetroController, retro_controller, RETRO, CONTROLLER, GObject)

/**
 * RetroControllerInterface:
 * @parent_iface: The parent interface.
 * @get_input_state: Gets the state from on of the controller's inputs.
 * @get_controller_type: Gets the type of the controller.
 * @get_capabilities: Gets the capabilities of the controller.
 * @get_supports_rumble: Gets whether the controller supports rumble.
 * @set_rumble_state: Sets the rumble state of the controller.
 *
 * An interface for a controller, e.g. a gamepad.
 **/
struct _RetroControllerInterface
{
  GTypeInterface parent_iface;

  gint16 (*get_input_state) (RetroController *self,
                             RetroInput      *input);
  RetroControllerType (*get_controller_type) (RetroController *self);
  guint64 (*get_capabilities) (RetroController *self);
  gboolean (*get_supports_rumble) (RetroController *self);
  void (*set_rumble_state) (RetroController   *self,
                            RetroRumbleEffect  effect,
                            guint16            strength);
};

gint16 retro_controller_get_input_state (RetroController *self,
                                         RetroInput      *input);
RetroControllerType retro_controller_get_controller_type (RetroController *self);
guint64 retro_controller_get_capabilities (RetroController *self);
gboolean retro_controller_has_capability (RetroController     *self,
                                          RetroControllerType  controller_type);
gboolean retro_controller_get_supports_rumble (RetroController *self);
void retro_controller_set_rumble_state (RetroController   *self,
                                        RetroRumbleEffect  effect,
                                        guint16            strength);
void retro_controller_emit_state_changed (RetroController *self);

G_END_DECLS
