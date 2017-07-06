#ifndef RETRO_ROTATION_H
#define RETRO_ROTATION_H

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
