// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

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
