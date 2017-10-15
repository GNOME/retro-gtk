// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_VIDEO_FILTER_H
#define RETRO_VIDEO_FILTER_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_VIDEO_FILTER (retro_video_filter_get_type ())

GType retro_video_filter_get_type (void) G_GNUC_CONST;

/**
 * RetroVideoFilter:
 * @RETRO_VIDEO_FILTER_SMOOTH: a smooth but blurry video filer
 * @RETRO_VIDEO_FILTER_SHARP: a sharp video filter showing every pixel
 * @RETRO_VIDEO_FILTER_COUNT: the number of video filters
 *
 * Represents the filters that can be applied to the video output.
 */
typedef enum
{
  RETRO_VIDEO_FILTER_SMOOTH,
  RETRO_VIDEO_FILTER_SHARP,
  RETRO_VIDEO_FILTER_COUNT,
} RetroVideoFilter;

RetroVideoFilter retro_video_filter_from_string (const gchar *filter);

G_END_DECLS

#endif /* RETRO_VIDEO_FILTER_H */
