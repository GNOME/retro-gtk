// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_RUMBLE_EFFECT (retro_rumble_effect_get_type ())

GType retro_rumble_effect_get_type (void) G_GNUC_CONST;

/**
 * RetroRumbleEffect:
 * @RETRO_RUMBLE_EFFECT_STRONG: the strong rumble effect
 * @RETRO_RUMBLE_EFFECT_WEAK: the weak rumble effect
 *
 * Represents the strength of the rumble effect.
 */
typedef enum
{
  RETRO_RUMBLE_EFFECT_STRONG,
  RETRO_RUMBLE_EFFECT_WEAK,
} RetroRumbleEffect;

G_END_DECLS
