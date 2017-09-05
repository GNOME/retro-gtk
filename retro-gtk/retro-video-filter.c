// This file is part of retro-gtk. License: GPL-3.0+.

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

GType
retro_video_filter_get_type (void)
{
  static volatile gsize retro_video_filter_type = 0;

  if (g_once_init_enter (&retro_video_filter_type)) {
    static const GEnumValue values[] = {
      { RETRO_VIDEO_FILTER_SMOOTH, "RETRO_VIDEO_FILTER_SMOOTH", "smooth" },
      { RETRO_VIDEO_FILTER_SHARP, "RETRO_VIDEO_FILTER_SHARP", "sharp" },
      { 0, NULL, NULL },
    };
    GType type;

    type = g_enum_register_static ("RetroVideoFilter", values);

    g_once_init_leave (&retro_video_filter_type, type);
  }

  return retro_video_filter_type;
}
