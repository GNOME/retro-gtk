// This file is part of RetroGtk. License: GPLv3

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <retro-gobject.h>

typedef struct _xrgb1555 xrgb1555;
struct _xrgb1555 {
  guint16 b: 5;
  guint16 g: 5;
  guint16 r: 5;
  guint16 x: 1;
};

typedef struct _xrgb8888 xrgb8888;
struct _xrgb8888 {
  guint32 b: 8;
  guint32 g: 8;
  guint32 r: 8;
  guint32 x: 8;
};

typedef struct _rgb565 rgb565;
struct _rgb565 {
  guint16 b: 5;
  guint16 g: 6;
  guint16 r: 5;
};

typedef struct _rgba8888 rgba8888;
struct _rgba8888 {
  guint32 a: 8;
  guint32 b: 8;
  guint32 g: 8;
  guint32 r: 8;
};

typedef rgba8888 (*GetRGBA8888) (const gpointer pixel);

static rgba8888
rgba8888_from_xrgb1555 (const gpointer data)
{
  xrgb1555 pixel = *(const xrgb1555 *) data;

  return (rgba8888) { pixel.r << 3 | pixel.r >> 2, pixel.g << 3 | pixel.g >> 2, pixel.b << 3 | pixel.b >> 2, 0xff };
}

static rgba8888
rgba8888_from_xrgb8888 (const gpointer data)
{
  xrgb8888 pixel = *(const xrgb8888 *) data;

  return (rgba8888) { pixel.r, pixel.g, pixel.b, 0xff };
}

static rgba8888
rgba8888_from_rgb565 (const gpointer data)
{
  rgb565 pixel = *(const rgb565 *) data;

  return (rgba8888) { pixel.r << 3 | pixel.r >> 2, pixel.g << 2 | pixel.g >> 4, pixel.b << 3 | pixel.b >> 2, 0xff };
}

static gboolean
get_interface_for_pixel_format (gint         pixel_format,
                                GetRGBA8888 *get_pixel,
                                gsize       *pixel_size)
{
  switch (pixel_format) {
  case RETRO_PIXEL_FORMAT_XRGB1555:
    *get_pixel = rgba8888_from_xrgb1555;
    *pixel_size = sizeof (xrgb1555);

    return TRUE;
  case RETRO_PIXEL_FORMAT_XRGB8888:
    *get_pixel = rgba8888_from_xrgb8888;
    *pixel_size = sizeof (rgba8888);

    return TRUE;
  case RETRO_PIXEL_FORMAT_RGB565:
    *get_pixel = rgba8888_from_rgb565;
    *pixel_size = sizeof (rgb565);

    return TRUE;
  default:
    return FALSE;
  }
}

/**
 * The destination buffer must be at least 'height * width * sizeof (rgba8888)' bytes long.
 */
static void
rgba8888_from_video (const gpointer  src,
                     rgba8888       *dst,
                     size_t          pixel_size,
                     guint           width,
                     guint           height,
                     gsize           pitch,
                     GetRGBA8888     get_pixel)
{
  gsize row, src_row, dst_row, col, src_col;

  for (row = 0 ; row < height ; row++) {
    src_row = row * pitch;
    dst_row = row * width;

    for (col = 0 ; col < width ; col++) {
      src_col = col * pixel_size;

      dst[dst_row + col] = get_pixel (src_row + src_col + src);
    }
  }
}

static rgba8888 *
new_rgba8888_from_video (const gpointer src,
                         guint          width,
                         guint          height,
                         gsize          pitch,
                         gint           pixel_format)
{
  GetRGBA8888 get_pixel;
  gsize pixel_size;
  rgba8888 *dst;

  if (!get_interface_for_pixel_format (pixel_format, &get_pixel, &pixel_size))
    return NULL;

  dst = g_new (rgba8888, height * width);
  rgba8888_from_video (src, dst, pixel_size, width, height, pitch, get_pixel);

  return dst;
}

static void
pixels_free (guchar *pixels, gpointer data) {
  g_free (pixels);
}

GdkPixbuf *
gdk_pixbuf_new_from_video (const gpointer src,
                           guint          width,
                           guint          height,
                           gsize          pitch,
                           gint           pixel_format)
{
  rgba8888 *dst;

  dst = new_rgba8888_from_video (src, width, height, pitch, pixel_format);
  if (dst == NULL)
    return NULL;

  GdkColorspace colorspace = GDK_COLORSPACE_RGB;
  gboolean has_alpha = TRUE;
  gint bits_per_sample = 8;
  gint rowstride = width * sizeof (rgba8888);

  GdkPixbuf *pb = gdk_pixbuf_new_from_data ((guchar *) dst, colorspace, has_alpha, bits_per_sample, width, height, rowstride, pixels_free, NULL);

  return pb;
}
