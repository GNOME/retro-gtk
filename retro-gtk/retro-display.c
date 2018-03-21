// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-display.h"

G_DEFINE_INTERFACE (RetroDisplay, retro_display, GTK_TYPE_WIDGET)

static void
retro_display_default_init (RetroDisplayInterface *iface)
{
}

/**
 * retro_display_get_pixbuf:
 * @self: a #RetroDisplay
 *
 * Gets the currently displayed video frame.
 *
 * Returns: (transfer none): a #GdkPixbuf
 */
GdkPixbuf *
retro_display_get_pixbuf (RetroDisplay *self)
{
  RetroDisplayInterface *iface;

  g_return_val_if_fail (RETRO_IS_DISPLAY (self), NULL);

  iface = RETRO_DISPLAY_GET_IFACE (self);

  g_return_val_if_fail (iface->get_pixbuf != NULL, NULL);

  return iface->get_pixbuf (self);
}

/**
 * retro_display_set_pixbuf:
 * @self: a #RetroDisplay
 * @pixbuf: a #GdkPixbuf
 *
 * Sets @pixbuf as the currently displayed video frame.
 */
void
retro_display_set_pixbuf (RetroDisplay *self,
                          GdkPixbuf    *pixbuf)
{
  RetroDisplayInterface *iface;

  g_return_if_fail (RETRO_IS_DISPLAY (self));

  iface = RETRO_DISPLAY_GET_IFACE (self);

  g_return_if_fail (iface->set_pixbuf != NULL);

  iface->set_pixbuf (self, pixbuf);
}

/**
 * retro_display_set_core:
 * @self: a #RetroDisplay
 * @core: (nullable): a #RetroCore, or %NULL
 *
 * Sets @core as the #RetroCore displayed by @self.
 */
void
retro_display_set_core (RetroDisplay *self,
                        RetroCore    *core)
{
  RetroDisplayInterface *iface;

  g_return_if_fail (RETRO_IS_DISPLAY (self));

  iface = RETRO_DISPLAY_GET_IFACE (self);

  g_return_if_fail (iface->set_core != NULL);

  iface->set_core (self, core);
}

/**
 * retro_display_set_filter:
 * @self: a #RetroDisplay
 * @filter: a #RetroVideoFilter
 *
 * Sets the video filter to use to render the core's video on @self.
 */
void
retro_display_set_filter (RetroDisplay     *self,
                          RetroVideoFilter  filter)
{
  RetroDisplayInterface *iface;

  g_return_if_fail (RETRO_IS_DISPLAY (self));

  iface = RETRO_DISPLAY_GET_IFACE (self);

  g_return_if_fail (iface->set_filter != NULL);

  iface->set_filter (self, filter);
}

/**
 * retro_display_get_coordinates_on_display:
 * @self: a #RetroDisplay
 * @widget_x: the abscissa on @self
 * @widget_y: the ordinate on @self
 * @display_x: return location for a the abscissa on the core's video display
 * @display_y: return location for a the ordinate on the core's video display
 *
 * Gets coordinates on the core's video output from coordinates on @self, and
 * whether the point is inside the core's video display.
 *
 * Returns: whether the coordinates are on the core's video display
 */
gboolean
retro_display_get_coordinates_on_display (RetroDisplay *self,
                                          gdouble       widget_x,
                                          gdouble       widget_y,
                                          gdouble      *display_x,
                                          gdouble      *display_y)
{
  RetroDisplayInterface *iface;

  g_return_val_if_fail (RETRO_IS_DISPLAY (self), FALSE);

  iface = RETRO_DISPLAY_GET_IFACE (self);

  g_return_val_if_fail (iface->get_coordinates_on_display != NULL, FALSE);

  return iface->get_coordinates_on_display (self,
                                            widget_x, widget_y,
                                            display_x, display_y);
}
