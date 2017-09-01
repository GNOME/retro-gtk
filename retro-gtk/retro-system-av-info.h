// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_SYSTEM_AV_INFO_H
#define RETRO_SYSTEM_AV_INFO_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct _RetroGameGeometry RetroGameGeometry;
typedef struct _RetroSystemTiming RetroSystemTiming;
typedef struct _RetroSystemAvInfo RetroSystemAvInfo;

struct _RetroGameGeometry
{
   guint base_width;
   guint base_height;
   guint max_width;
   guint max_height;
   gfloat aspect_ratio;
};

struct _RetroSystemTiming
{
  gdouble fps;
  gdouble sample_rate;
};

struct _RetroSystemAvInfo
{
  RetroGameGeometry geometry;
  RetroSystemTiming timing;
};

G_END_DECLS

#endif /* RETRO_SYSTEM_AV_INFO_H */
