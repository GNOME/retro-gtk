// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_MOUSE_ID_H
#define RETRO_MOUSE_ID_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_MOUSE_ID (retro_mouse_id_get_type ())

GType retro_mouse_id_get_type (void) G_GNUC_CONST;

/**
 * RetroMouseId:
 * @RETRO_MOUSE_ID_X: the X axis of a mouse
 * @RETRO_MOUSE_ID_Y: the Y axis of a mouse
 * @RETRO_MOUSE_ID_LEFT: the left button of a mouse
 * @RETRO_MOUSE_ID_RIGHT: the right button of a mouse
 * @RETRO_MOUSE_ID_WHEELUP: the up direction of a mouse wheel
 * @RETRO_MOUSE_ID_WHEELDOWN: the down direction of a mouse wheel
 * @RETRO_MOUSE_ID_MIDDLE: the middle button of a mouse
 * @RETRO_MOUSE_ID_HORIZ_WHEELUP: the horizontal up direction of a mouse wheel
 * @RETRO_MOUSE_ID_HORIZ_WHEELDOWN: the horizontal down direction of a mouse wheel
 *
 * Represents the inputs for the Libretro mouse.
 */
typedef enum
{
  RETRO_MOUSE_ID_X,
  RETRO_MOUSE_ID_Y,
  RETRO_MOUSE_ID_LEFT,
  RETRO_MOUSE_ID_RIGHT,
  RETRO_MOUSE_ID_WHEELUP,
  RETRO_MOUSE_ID_WHEELDOWN,
  RETRO_MOUSE_ID_MIDDLE,
  RETRO_MOUSE_ID_HORIZ_WHEELUP,
  RETRO_MOUSE_ID_HORIZ_WHEELDOWN,
} RetroMouseId;

G_END_DECLS

#endif /* RETRO_MOUSE_ID_H */
