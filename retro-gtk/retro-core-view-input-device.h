// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_CORE_VIEW_INPUT_DEVICE_H
#define RETRO_CORE_VIEW_INPUT_DEVICE_H

#include <glib-object.h>
#include "retro-gtk-internal.h"

G_BEGIN_DECLS

#define RETRO_TYPE_CORE_VIEW_INPUT_DEVICE (retro_core_view_input_device_get_type())

G_DECLARE_FINAL_TYPE (RetroCoreViewInputDevice, retro_core_view_input_device, RETRO, CORE_VIEW_INPUT_DEVICE, GObject)

RetroCoreViewInputDevice *retro_core_view_input_device_new (RetroCoreView *view, RetroDeviceType device_type);

G_END_DECLS

#endif /* RETRO_CORE_VIEW_INPUT_DEVICE_H */

