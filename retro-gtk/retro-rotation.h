// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_ROTATION_H
#define RETRO_ROTATION_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef enum _RetroRotation RetroRotation;

enum _RetroRotation
{
  NONE,
  COUNTERCLOCKWISE,
  UPSIDEDOWN,
  CLOCKWISE,
};

G_END_DECLS

#endif /* RETRO_ROTATION_H */
