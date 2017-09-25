// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_RUMBLE_EFFECT_H
#define RETRO_RUMBLE_EFFECT_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

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

#endif /* RETRO_RUMBLE_EFFECT_H */
