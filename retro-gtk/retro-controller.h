// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_CONTROLLER_H
#define RETRO_CONTROLLER_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-input.h"
#include "retro-rumble-effect.h"

G_BEGIN_DECLS

#define RETRO_TYPE_CONTROLLER (retro_controller_get_type())

G_DECLARE_INTERFACE (RetroController, retro_controller, RETRO, CONTROLLER, GObject)

struct _RetroControllerInterface
{
  GTypeInterface parent_iface;

  void (*poll) (RetroController *self);
  gint16 (*get_input_state) (RetroController *self,
                             RetroInput      *input);
  RetroControllerType (*get_controller_type) (RetroController *self);
  guint64 (*get_capabilities) (RetroController *self);
};

void retro_controller_poll (RetroController *self);
gint16 retro_controller_get_input_state (RetroController *self,
                                         RetroInput      *input);
RetroControllerType retro_controller_get_controller_type (RetroController *self);
guint64 retro_controller_get_capabilities (RetroController *self);
gboolean retro_controller_has_capability (RetroController     *self,
                                          RetroControllerType  controller_type);
gboolean retro_controller_set_rumble_state (RetroController   *self,
                                            RetroRumbleEffect  effect,
                                            guint16            strength);

G_END_DECLS

#endif /* RETRO_CONTROLLER_H */
