// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_INPUT_DEVICE_H
#define RETRO_INPUT_DEVICE_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-device-type.h"
#include "retro-rumble-effect.h"

G_BEGIN_DECLS

#define RETRO_TYPE_INPUT_DEVICE (retro_input_device_get_type())

G_DECLARE_INTERFACE (RetroInputDevice, retro_input_device, RETRO, INPUT_DEVICE, GObject)

struct _RetroInputDeviceInterface
{
  GTypeInterface parent_iface;

  void (*poll) (RetroInputDevice *self);
  gint16 (*get_input_state) (RetroInputDevice *self,
                             RetroDeviceType   device,
                             guint             index,
                             guint             id);
  RetroDeviceType (*get_device_type) (RetroInputDevice *self);
  guint64 (*get_device_capabilities) (RetroInputDevice *self);
};

void retro_input_device_poll (RetroInputDevice *self);
gint16 retro_input_device_get_input_state (RetroInputDevice *self,
                                           RetroDeviceType   device,
                                           guint             index,
                                           guint             id);
RetroDeviceType retro_input_device_get_device_type (RetroInputDevice *self);
guint64 retro_input_device_get_device_capabilities (RetroInputDevice *self);
gboolean retro_input_device_set_rumble_state (RetroInputDevice  *self,
                                              RetroRumbleEffect  effect,
                                              guint16            strength);

G_END_DECLS

#endif /* RETRO_INPUT_DEVICE_H */
