// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-screen.h"

struct _RetroScreen
{
  GObject parent_instance;

  GdkRectangle view;
};

G_DEFINE_TYPE (RetroScreen, retro_screen, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_VIEW,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

/* Private */

static void
retro_screen_class_init (RetroScreenClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  /**
   * RetroScreen:view:
   *
   * The view representing the screen in the video output.
   */
  properties[PROP_VIEW] =
    g_param_spec_boxed ("view",
                        "View",
                        "The view",
                        GDK_TYPE_RECTANGLE,
                        G_PARAM_READWRITE |
                        G_PARAM_STATIC_NAME |
                        G_PARAM_STATIC_NICK |
                        G_PARAM_STATIC_BLURB);

}

static void
retro_screen_init (RetroScreen *self)
{
}

/* Public */

/**
 * retro_screen_get_view:
 * @self: a #RetroScreen
 *
 * Gets the view of @self.
 *
 * Returns: (transfer none): the view of @self
 */
const GdkRectangle *
retro_screen_get_view (RetroScreen *self)
{
  g_return_val_if_fail (RETRO_IS_SCREEN (self), NULL);

  return &self->view;
}

/**
 * retro_screen_set_view:
 * @self: a #RetroScreen
 * @view: the view
 *
 * Sets the view for @self.
 */
void
retro_screen_set_view (RetroScreen        *self,
                       const GdkRectangle *view)
{
  g_return_if_fail (RETRO_IS_SCREEN (self));
  g_return_if_fail (view != NULL);

  if (gdk_rectangle_equal (&self->view, view))
    return;

  self->view = *view;

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_VIEW]);
}

RetroScreen *
retro_screen_new ()
{
  return g_object_new (RETRO_TYPE_SCREEN, NULL);
}
