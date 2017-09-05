// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_ANALOG_INDEX_H
#define RETRO_ANALOG_INDEX_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_ANALOG_INDEX (retro_analog_index_get_type ())

GType retro_analog_index_get_type (void) G_GNUC_CONST;

/**
 * RetroAnalogIndex:
 * @RETRO_ANALOG_INDEX_LEFT: the left analog stick
 * @RETRO_ANALOG_INDEX_RIGHT: the right analog stick
 *
 * Represents the analog sticks for the Libretro gamepad.
 */
typedef enum
{
  RETRO_ANALOG_INDEX_LEFT,
  RETRO_ANALOG_INDEX_RIGHT,
} RetroAnalogIndex;

G_END_DECLS

#endif /* RETRO_ANALOG_INDEX_H */
