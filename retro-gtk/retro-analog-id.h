// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_ANALOG_ID_H
#define RETRO_ANALOG_ID_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_ANALOG_ID (retro_analog_id_get_type ())

GType retro_analog_id_get_type (void) G_GNUC_CONST;

/**
 * RetroAnalogId:
 * @RETRO_ANALOG_ID_X: the X axis of an analog stick
 * @RETRO_ANALOG_ID_Y: the Y axis of an analog stick
 *
 * Represents the axes of the analog sticks for the Libretro gamepad.
 */
typedef enum
{
  RETRO_ANALOG_ID_X,
  RETRO_ANALOG_ID_Y,
} RetroAnalogId;

G_END_DECLS

#endif /* RETRO_ANALOG_ID_H */
