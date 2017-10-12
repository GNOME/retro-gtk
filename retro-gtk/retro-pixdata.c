// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-pixdata-private.h"

G_DEFINE_BOXED_TYPE (RetroPixdata, retro_pixdata, retro_pixdata_copy, retro_pixdata_free)

/*
 * Because gdk-pixbuf saves dpi as integer we have to multiply it by big enough
 * number to represent aspect ratio precisely.
 */
#define RETRO_CAIRO_DISPLAY_Y_DPI (1000000.0f)

/* Private */

typedef struct {
  guint16 b: 5;
  guint16 g: 5;
  guint16 r: 5;
  guint16 x: 1;
} xrgb1555;

typedef struct {
  guint32 b: 8;
  guint32 g: 8;
  guint32 r: 8;
  guint32 x: 8;
} xrgb8888;

typedef struct {
  guint16 b: 5;
  guint16 g: 6;
  guint16 r: 5;
} rgb565;

typedef struct {
  guint32 a: 8;
  guint32 b: 8;
  guint32 g: 8;
  guint32 r: 8;
} rgba8888;

typedef rgba8888 (*GetRGBA8888) (gconstpointer pixel);

static rgba8888
rgba8888_from_xrgb1555 (gconstpointer data)
{
  xrgb1555 pixel = *(const xrgb1555 *) data;

  return (rgba8888) {
    pixel.r << 3 | pixel.r >> 2,
    pixel.g << 3 | pixel.g >> 2,
    pixel.b << 3 | pixel.b >> 2,
    0xff,
  };
}

static rgba8888
rgba8888_from_xrgb8888 (gconstpointer data)
{
  xrgb8888 pixel = *(const xrgb8888 *) data;

  return (rgba8888) {
    pixel.r,
    pixel.g,
    pixel.b,
    0xff,
  };
}

static rgba8888
rgba8888_from_rgb565 (gconstpointer data)
{
  rgb565 pixel = *(const rgb565 *) data;

  return (rgba8888) {
    pixel.r << 3 | pixel.r >> 2,
    pixel.g << 2 | pixel.g >> 4,
    pixel.b << 3 | pixel.b >> 2,
    0xff,
  };
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

/*
 * The destination buffer must be at least `height * width * sizeof (rgba8888)`
 * bytes long.
 */
static void
rgba8888_from_video (gconstpointer  src,
                     rgba8888      *dst,
                     size_t         pixel_size,
                     guint          width,
                     guint          height,
                     gsize          pitch,
                     GetRGBA8888    get_pixel)
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

static void
pixels_free (guchar   *pixels,
             gpointer  data)
{
  g_free (pixels);
}

/**
 * retro_pixdata_new:
 * @data: the video data
 * @width: the width
 * @height: the height
 * @pitch: the distance in bytes between rows
 * @pixel_format: the pixel format
 * @aspect_ratio: the aspect ratio to render the video
 *
 * Creates a new #RetroPixdata.
 *
 * Returns: (transfer full): a new #RetroPixdata, use retro_pixdata_free() to
 * free it
 */
static RetroPixdata *
retro_pixdata_new (gconstpointer    data,
                   RetroPixelFormat pixel_format,
                   gsize            rowstride,
                   gsize            width,
                   gsize            height,
                   gfloat           aspect_ratio)
{
  RetroPixdata *self;

  g_return_val_if_fail (data != NULL, NULL);
  g_return_val_if_fail (rowstride != 0, NULL);
  g_return_val_if_fail (width != 0, NULL);
  g_return_val_if_fail (height != 0, NULL);
  g_return_val_if_fail (aspect_ratio > 0.f, NULL);

  self = g_slice_new0 (RetroPixdata);
  self->data = g_memdup (data, rowstride * height);
  self->pixel_format = pixel_format;
  self->rowstride = rowstride;
  self->width = width;
  self->height = height;
  self->aspect_ratio = aspect_ratio;

  return self;
}

/* Public */

/**
 * retro_pixdata_init:
 * @self: a #RetroPixdata
 * @data: the video data
 * @width: the width
 * @height: the height
 * @pitch: the distance in bytes between rows
 * @pixel_format: the pixel format
 * @aspect_ratio: the aspect ratio to render the video
 *
 * Initializes @self with the given parameters.
 */
void
retro_pixdata_init (RetroPixdata     *self,
                    gconstpointer     data,
                    RetroPixelFormat  pixel_format,
                    gsize             rowstride,
                    gsize             width,
                    gsize             height,
                    gfloat            aspect_ratio)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (data != NULL);
  g_return_if_fail (rowstride != 0);
  g_return_if_fail (width != 0);
  g_return_if_fail (height != 0);
  g_return_if_fail (aspect_ratio > 0.f);

  self->data = (guint8 *) data;
  self->pixel_format = pixel_format;
  self->rowstride = rowstride;
  self->width = width;
  self->height = height;
  self->aspect_ratio = aspect_ratio;
}

/**
 * retro_pixdata_copy:
 * @self: a #RetroPixdata
 *
 * Copies @self into a new #RetroPixdata.
 *
 * Returns: (transfer full): a new #RetroPixdata, use retro_pixdata_free() to
 * free it
 */
RetroPixdata *
retro_pixdata_copy (RetroPixdata *self)
{
  g_return_val_if_fail (self != NULL, NULL);

  return retro_pixdata_new (self->data, self->pixel_format,
                            self->rowstride, self->width, self->height,
                            self->aspect_ratio);
}

/**
 * retro_pixdata_free:
 * @self: a #RetroPixdata
 *
 * Frees the given #RetroPixdata object.
 */
void
retro_pixdata_free (RetroPixdata *self)
{
  g_return_if_fail (self != NULL);

  g_free (self->data);

  g_slice_free (RetroPixdata, self);
}

/**
 * retro_pixdata_get_aspect_ratio:
 * @self: a #RetroCore
 *
 * Gets the aspect ratio the video should be rendered with.
 *
 * Returns: the aspect ratio
 */
gfloat
retro_pixdata_get_aspect_ratio (RetroPixdata *self)
{
  g_return_val_if_fail (self != NULL, 0.f);

  return self->aspect_ratio;
}

/**
 * retro_pixdata_to_pixbuf:
 * @self: the #RetroPixdata
 *
 * Creates a new #GdkPixbuf from @self.
 *
 * Returns: (transfer full): a new #RetroPixdata
 */
GdkPixbuf *
retro_pixdata_to_pixbuf (RetroPixdata *self)
{
  GetRGBA8888 get_pixel;
  gsize pixel_size;
  rgba8888 *rgba8888_data;
  GdkPixbuf *pixbuf;
  gfloat x_dpi;
  gchar *x_dpi_string;
  gchar *y_dpi_string;

  g_return_val_if_fail (self != NULL, NULL);

  if (!get_interface_for_pixel_format (self->pixel_format,
                                       &get_pixel,
                                       &pixel_size))
    return NULL;

  rgba8888_data = g_new (rgba8888, self->width * self->height);
  rgba8888_from_video (self->data, rgba8888_data, pixel_size,
                       self->width, self->height, self->rowstride,
                       get_pixel);

  if (rgba8888_data == NULL)
    return NULL;

  pixbuf = gdk_pixbuf_new_from_data ((guchar *) rgba8888_data,
                                     GDK_COLORSPACE_RGB, TRUE, 8,
                                     self->width, self->height,
                                     self->width * sizeof (rgba8888),
                                     pixels_free, NULL);

  x_dpi = self->aspect_ratio * RETRO_CAIRO_DISPLAY_Y_DPI;
  x_dpi_string = g_strdup_printf ("%g", x_dpi);
  y_dpi_string = g_strdup_printf ("%g", RETRO_CAIRO_DISPLAY_Y_DPI);
  gdk_pixbuf_set_option (pixbuf, "x-dpi", x_dpi_string);
  gdk_pixbuf_set_option (pixbuf, "y-dpi", y_dpi_string);
  g_free (y_dpi_string);
  g_free (x_dpi_string);

  return pixbuf;
}
