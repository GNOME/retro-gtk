// This file is part of retro-gtk. License: GPL-3.0+.

/**
 * SECTION:retro-video-filter
 * @short_description: A video filter enumeration
 * @title: RetroVideoFilter
 * @See_also: #RetroCoreView
 */

#include "retro-video-filter.h"

/**
 * retro_video_filter_from_string:
 * @filter: a filter name
 *
 * Gets a #RetroVideoFilter from it's name. It defaults to
 * RETRO_VIDEO_FILTER_SMOOTH in case of error.
 *
 * Returns: a #RetroVideoFilter
 */
RetroVideoFilter
retro_video_filter_from_string (const gchar *filter)
{
  GEnumClass* enum_class;
  RetroVideoFilter result;
  GEnumValue* eval = NULL;

  g_return_val_if_fail (filter != NULL, RETRO_VIDEO_FILTER_SMOOTH);

  enum_class = (GEnumClass *) g_type_class_ref (RETRO_TYPE_VIDEO_FILTER);
  eval = g_enum_get_value_by_nick (enum_class, filter);

  result = (eval == NULL) ?
    RETRO_VIDEO_FILTER_SMOOTH :
    (RetroVideoFilter) eval->value;

  g_type_class_unref (enum_class);

  return result;
}
