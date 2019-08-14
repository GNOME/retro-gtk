// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-pixbuf.h"

/**
 * retro_pixbuf_get_aspect_ratio:
 * @pixbuf: a #GdkPixbuf
 *
 * Gets aspect ratio of @pixbuf by reading the 'aspect-ratio' pixbuf option.
 *
 * Returns: the aspect ratio
 */
gfloat
retro_pixbuf_get_aspect_ratio (GdkPixbuf *pixbuf)
{
  const gchar *aspect_ratio_str;
  gfloat result = 0.f;

  g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), 0.f);

  aspect_ratio_str = gdk_pixbuf_get_option (pixbuf, "aspect-ratio");

  if (!aspect_ratio_str)
    return 0.f;

  sscanf (aspect_ratio_str, "%g", &result);

  return result;
}

/**
 * retro_pixbuf_set_aspect_ratio:
 * @pixbuf: a #GdkPixbuf
 * @aspect_ratio: the aspect ratio value
 *
 * Sets aspect ratio of @pixbuf by changing the 'aspect-ratio' pixbuf option.
 * Use retro_pixbuf_get_aspect_ratio() to retrieve it.
 */
void
retro_pixbuf_set_aspect_ratio (GdkPixbuf *pixbuf,
                               gfloat     aspect_ratio)
{
  g_autofree gchar *aspect_ratio_string = NULL;

  g_return_if_fail (GDK_IS_PIXBUF (pixbuf));
  g_return_if_fail (aspect_ratio > 0);

  aspect_ratio_string = g_strdup_printf ("%g", aspect_ratio);

  gdk_pixbuf_remove_option (pixbuf, "aspect-ratio");

  gdk_pixbuf_set_option (pixbuf, "aspect-ratio", aspect_ratio_string);
}
