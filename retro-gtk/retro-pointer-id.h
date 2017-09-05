// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_POINTER_ID_H
#define RETRO_POINTER_ID_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_POINTER_ID (retro_pointer_id_get_type ())

GType retro_pointer_id_get_type (void) G_GNUC_CONST;

/**
 * RetroPointerId:
 * @RETRO_POINTER_ID_X: the X axis of a pointer
 * @RETRO_POINTER_ID_Y: the Y axis of a pointer
 * @RETRO_POINTER_ID_PRESSED: the pression of a pointer
 *
 * Represents the inputs for the Libretro pointer.
 */
typedef enum
{
  RETRO_POINTER_ID_X,
  RETRO_POINTER_ID_Y,
  RETRO_POINTER_ID_PRESSED,
} RetroPointerId;

G_END_DECLS

#endif /* RETRO_POINTER_ID_H */
