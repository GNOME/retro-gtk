// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-core-view.h"

G_BEGIN_DECLS

#define RETRO_TYPE_CORE_VIEW_CONTROLLER (retro_core_view_controller_get_type())

G_DECLARE_FINAL_TYPE (RetroCoreViewController, retro_core_view_controller, RETRO, CORE_VIEW_CONTROLLER, GObject)

RetroCoreViewController *retro_core_view_controller_new (RetroCoreView       *view,
                                                         RetroControllerType  controller_type);

G_END_DECLS
