// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-core-view-input-device.h"

struct _RetroCoreViewInputDevice
{
  GObject parent_instance;
  GWeakRef view;
  RetroDeviceType device_type;
};

static void retro_input_device_interface_init (RetroInputDeviceIface   *iface);

G_DEFINE_TYPE_WITH_CODE (RetroCoreViewInputDevice, retro_core_view_input_device, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (RETRO_TYPE_INPUT_DEVICE,
                                                retro_input_device_interface_init))

/* Private */

static void
retro_core_view_input_device_poll (RetroInputDevice *base)
{
}

static gint16
retro_core_view_input_device_get_input_state (RetroInputDevice *base,
                                              RetroDeviceType   device,
                                              guint             index,
                                              guint             id)
{
  RetroCoreViewInputDevice *self = RETRO_CORE_VIEW_INPUT_DEVICE (base);
  gpointer view;
  gint16 result;

  g_return_val_if_fail (self != NULL, 0);

  if (device != self->device_type)
    return 0;

  view = g_weak_ref_get (&self->view);

  g_return_val_if_fail (view != NULL, 0);

  result = retro_core_view_get_input_state (RETRO_CORE_VIEW (view),
                                          self->device_type,
                                          index, id);

  g_object_unref (G_OBJECT (view));

  return result;
}

static RetroDeviceType
retro_core_view_input_device_get_device_type (RetroInputDevice *base)
{
  RetroCoreViewInputDevice *self = RETRO_CORE_VIEW_INPUT_DEVICE (base);

  g_return_val_if_fail (self != NULL, 0);

  return self->device_type;
}

static guint64
retro_core_view_input_device_get_device_capabilities (RetroInputDevice *base)
{
  RetroCoreViewInputDevice *self = RETRO_CORE_VIEW_INPUT_DEVICE (base);
  gpointer view;
  guint64 result;

  g_return_val_if_fail (self != NULL, 0);

  view = g_weak_ref_get (&self->view);

  g_return_val_if_fail (view != NULL, 0);

  result = retro_core_view_get_device_capabilities (RETRO_CORE_VIEW (view));

  g_object_unref (G_OBJECT (view));

  return result;
}

static void
retro_core_view_input_device_finalize (GObject *object)
{
  RetroCoreViewInputDevice *self = RETRO_CORE_VIEW_INPUT_DEVICE (object);

  g_weak_ref_clear (&self->view);

  G_OBJECT_CLASS (retro_core_view_input_device_parent_class)->finalize (object);
}

static void
retro_core_view_input_device_class_init (RetroCoreViewInputDeviceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_core_view_input_device_finalize;
}

static void
retro_core_view_input_device_init (RetroCoreViewInputDevice *self)
{
}

static void
retro_input_device_interface_init (RetroInputDeviceIface *iface)
{
  iface->poll = retro_core_view_input_device_poll;
  iface->get_input_state =  retro_core_view_input_device_get_input_state;
  iface->get_device_type = retro_core_view_input_device_get_device_type;
  iface->get_device_capabilities = retro_core_view_input_device_get_device_capabilities;
}

/* Public */

RetroCoreViewInputDevice *
retro_core_view_input_device_new (RetroCoreView *view, RetroDeviceType device_type)
{
  RetroCoreViewInputDevice *self = NULL;

  g_return_val_if_fail (view != NULL, NULL);

  self = g_object_new (RETRO_TYPE_CORE_VIEW_INPUT_DEVICE, NULL);
  g_weak_ref_init (&self->view, view);
  self->device_type = device_type;

  return self;
}
