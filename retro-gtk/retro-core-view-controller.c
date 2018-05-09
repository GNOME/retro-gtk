// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-core-view-controller.h"

#include "retro-controller.h"

struct _RetroCoreViewController
{
  GObject parent_instance;
  GWeakRef view;
  RetroControllerType controller_type;
};

static void retro_controller_interface_init (RetroControllerInterface *iface);

G_DEFINE_TYPE_WITH_CODE (RetroCoreViewController, retro_core_view_controller, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (RETRO_TYPE_CONTROLLER,
                                                retro_controller_interface_init))

/* Private */

static void
retro_core_view_controller_poll (RetroController *base)
{
}

static gint16
retro_core_view_controller_get_input_state (RetroController *base,
                                            RetroInput      *input)
{
  RetroCoreViewController *self = RETRO_CORE_VIEW_CONTROLLER (base);
  gpointer view;
  gint16 result;

  g_return_val_if_fail (self != NULL, 0);

  if (retro_input_get_controller_type (input) != self->controller_type)
    return 0;

  view = g_weak_ref_get (&self->view);

  if (view == NULL)
    return 0;

  result = retro_core_view_get_input_state (RETRO_CORE_VIEW (view), input);

  g_object_unref (G_OBJECT (view));

  return result;
}

static RetroControllerType
retro_core_view_controller_get_controller_type (RetroController *base)
{
  RetroCoreViewController *self = RETRO_CORE_VIEW_CONTROLLER (base);

  g_return_val_if_fail (self != NULL, 0);

  return self->controller_type;
}

static guint64
retro_core_view_controller_get_capabilities (RetroController *base)
{
  RetroCoreViewController *self = RETRO_CORE_VIEW_CONTROLLER (base);
  gpointer view;
  guint64 capabilities;

  g_return_val_if_fail (self != NULL, 0);

  view = g_weak_ref_get (&self->view);

  if (view == NULL)
    return 0;

  capabilities = retro_core_view_get_controller_capabilities (RETRO_CORE_VIEW (view));

  g_object_unref (G_OBJECT (view));

  return capabilities & (1 << self->controller_type);
}

static gboolean
retro_core_view_controller_set_rumble_state (RetroController   *self,
                                             RetroRumbleEffect  effect,
                                             guint16            strength)
{
  return FALSE;
}

static void
retro_core_view_controller_finalize (GObject *object)
{
  RetroCoreViewController *self = RETRO_CORE_VIEW_CONTROLLER (object);

  g_weak_ref_clear (&self->view);

  G_OBJECT_CLASS (retro_core_view_controller_parent_class)->finalize (object);
}

static void
retro_core_view_controller_class_init (RetroCoreViewControllerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_core_view_controller_finalize;
}

static void
retro_core_view_controller_init (RetroCoreViewController *self)
{
}

static void
retro_controller_interface_init (RetroControllerInterface *iface)
{
  iface->poll = retro_core_view_controller_poll;
  iface->get_input_state =  retro_core_view_controller_get_input_state;
  iface->get_controller_type = retro_core_view_controller_get_controller_type;
  iface->get_capabilities = retro_core_view_controller_get_capabilities;
  iface->set_rumble_state = retro_core_view_controller_set_rumble_state;
}

/* Public */

RetroCoreViewController *
retro_core_view_controller_new (RetroCoreView       *view,
                                RetroControllerType  controller_type)
{
  RetroCoreViewController *self = NULL;

  g_return_val_if_fail (view != NULL, NULL);

  self = g_object_new (RETRO_TYPE_CORE_VIEW_CONTROLLER, NULL);
  g_weak_ref_init (&self->view, view);
  self->controller_type = controller_type;

  return self;
}
