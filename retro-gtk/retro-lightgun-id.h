// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_LIGHTGUN_ID_H
#define RETRO_LIGHTGUN_ID_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_LIGHTGUN_ID (retro_lightgun_id_get_type ())

GType retro_lightgun_id_get_type (void) G_GNUC_CONST;

/**
 * RetroLightgunId:
 * @RETRO_LIGHTGUN_ID_X: the X axis of a lightgun
 * @RETRO_LIGHTGUN_ID_Y: the Y axis of a lightgun
 * @RETRO_LIGHTGUN_ID_TRIGGER: the trigger of a lightgun
 * @RETRO_LIGHTGUN_ID_CURSOR: the cursor of a lightgun
 * @RETRO_LIGHTGUN_ID_TURBO: the turbo button of a lightgun
 * @RETRO_LIGHTGUN_ID_PAUSE: the pause button of a lightgun
 * @RETRO_LIGHTGUN_ID_START: the start button of a lightgun
 *
 * Represents the inputs for the Libretro lightgun.
 */
typedef enum
{
  RETRO_LIGHTGUN_ID_X,
  RETRO_LIGHTGUN_ID_Y,
  RETRO_LIGHTGUN_ID_TRIGGER,
  RETRO_LIGHTGUN_ID_CURSOR,
  RETRO_LIGHTGUN_ID_TURBO,
  RETRO_LIGHTGUN_ID_PAUSE,
  RETRO_LIGHTGUN_ID_START,
} RetroLightgunId;

G_END_DECLS

#endif /* RETRO_LIGHTGUN_ID_H */
