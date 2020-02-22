// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-cairo-display-private.h"

#include "retro-pixbuf.h"
#include "retro-pixdata.h"

struct _RetroCairoDisplay
{
  GtkDrawingArea parent_instance;
  RetroCore *core;
  GdkPixbuf *pixbuf;
  RetroVideoFilter filter;
  gfloat aspect_ratio;
  gulong on_video_output_id;
};

G_DEFINE_TYPE (RetroCairoDisplay, retro_cairo_display, GTK_TYPE_DRAWING_AREA)

enum {
  PROP_PIXBUF = 1,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

/* Private */

static void
get_video_box (RetroCairoDisplay *self,
               gdouble           *width,
               gdouble           *height,
               gdouble           *x,
               gdouble           *y)
{
  gdouble w;
  gdouble h;
  gdouble display_ratio;
  gdouble allocated_ratio;

  g_assert (width != NULL);
  g_assert (height != NULL);
  g_assert (x != NULL);
  g_assert (y != NULL);

  w = (gdouble) gtk_widget_get_allocated_width (GTK_WIDGET (self));
  h = (gdouble) gtk_widget_get_allocated_height (GTK_WIDGET (self));

  // Set the size of the display.
  display_ratio = (gdouble) self->aspect_ratio;
  allocated_ratio = w / h;

  // If the screen is wider than the video…
  if (allocated_ratio > display_ratio) {
    *height = h;
    *width = (gdouble) (h * display_ratio);
  }
  else {
    *width = w;
    *height = (gdouble) (w / display_ratio);
  }

  // Set the position of the display.
  *x = (w - *width) / 2;
  *y = (h - *height) / 2;
}

static void
draw_background (RetroCairoDisplay *self,
                 cairo_t           *cr)
{
  g_assert (cr != NULL);

  cairo_set_source_rgb (cr, (gdouble) 0, (gdouble) 0, (gdouble) 0);
  cairo_paint (cr);
}

static gboolean
retro_cairo_display_draw (GtkWidget *widget,
                          cairo_t   *cr)
{
  RetroCairoDisplay *self = RETRO_CAIRO_DISPLAY (widget);
  GdkPixbuf *to_draw;
  gboolean has_alpha;
  gint width;
  gint height;

  cairo_surface_t *surface;
  gdouble w = 0.0;
  gdouble h = 0.0;
  gdouble x = 0.0;
  gdouble y = 0.0;
  gdouble xs;
  gdouble ys;
  cairo_pattern_t *source;

  draw_background (self, cr);

  if (self->pixbuf == NULL)
    return FALSE;

  if (gtk_widget_get_sensitive (GTK_WIDGET (self)))
    to_draw = g_object_ref (self->pixbuf);
  else {
    has_alpha = gdk_pixbuf_get_has_alpha (self->pixbuf);
    width = gdk_pixbuf_get_width (self->pixbuf);
    height = gdk_pixbuf_get_height (self->pixbuf);
    to_draw = gdk_pixbuf_new (GDK_COLORSPACE_RGB, has_alpha, 8, width, height);
    gdk_pixbuf_saturate_and_pixelate (self->pixbuf, to_draw, 0.0f, FALSE);
  }

  surface = gdk_cairo_surface_create_from_pixbuf (to_draw, 1, NULL);
  get_video_box (self, &w, &h, &x, &y);
  xs = w / gdk_pixbuf_get_width (to_draw);
  ys = h / gdk_pixbuf_get_height (to_draw);

  cairo_scale (cr, xs, ys);
  cairo_set_source_surface (cr, surface, x / xs, y / ys);
  source = cairo_get_source (cr);
  switch (self->filter) {
  case RETRO_VIDEO_FILTER_SHARP:
    cairo_pattern_set_filter (source, CAIRO_FILTER_NEAREST);

    break;
  default:
  case RETRO_VIDEO_FILTER_SMOOTH:
    cairo_pattern_set_filter (source, CAIRO_FILTER_BILINEAR);

    break;
  }
  cairo_paint (cr);

  cairo_surface_destroy (surface);
  g_object_unref (to_draw);

  return TRUE;
}

static void
retro_cairo_display_finalize (GObject *object)
{
  RetroCairoDisplay *self = (RetroCairoDisplay *) object;

  if (self->core != NULL)
    g_object_unref (self->core);
  if (self->pixbuf != NULL)
    g_object_unref (self->pixbuf);

  G_OBJECT_CLASS (retro_cairo_display_parent_class)->finalize (object);
}

static void
retro_cairo_display_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  RetroCairoDisplay *self = RETRO_CAIRO_DISPLAY (object);

  switch (prop_id) {
  case PROP_PIXBUF:
    g_value_set_object (value, retro_cairo_display_get_pixbuf (self));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_cairo_display_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  RetroCairoDisplay *self = RETRO_CAIRO_DISPLAY (object);

  switch (prop_id) {
  case PROP_PIXBUF:
    retro_cairo_display_set_pixbuf (self, g_value_get_object (value));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_cairo_display_class_init (RetroCairoDisplayClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  widget_class->draw = retro_cairo_display_draw;
  object_class->finalize = retro_cairo_display_finalize;
  object_class->get_property = retro_cairo_display_get_property;
  object_class->set_property = retro_cairo_display_set_property;

  properties[PROP_PIXBUF] =
    g_param_spec_object ("pixbuf",
                         "Pixbuf",
                         "The displayed pixbuf",
                         gdk_pixbuf_get_type (),
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB);
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_PIXBUF, properties[PROP_PIXBUF]);

  gtk_widget_class_set_css_name (widget_class, "retrocairodisplay");
}

static void
queue_draw (GObject    *sender,
            GParamSpec *pspec,
            gpointer    self)
{
  gtk_widget_queue_draw (GTK_WIDGET (self));
}

static void
retro_cairo_display_init (RetroCairoDisplay *self)
{
  self->filter = RETRO_VIDEO_FILTER_SMOOTH;
  g_signal_connect_object (G_OBJECT (self),
                           "notify::sensitive",
                           (GCallback) queue_draw,
                           GTK_WIDGET (self),
                           0);
  g_signal_connect_object (G_OBJECT (self),
                           "notify::pixbuf",
                           (GCallback) queue_draw,
                           GTK_WIDGET (self),
                           0);
}

static void
on_video_output (RetroCore    *sender,
                 RetroPixdata *pixdata,
                 gpointer      user_data)
{
  RetroCairoDisplay *self = RETRO_CAIRO_DISPLAY (user_data);

  GdkPixbuf *pixbuf;

  self->aspect_ratio = retro_pixdata_get_aspect_ratio (pixdata);
  pixbuf = retro_pixdata_to_pixbuf (pixdata);
  retro_cairo_display_set_pixbuf (self, pixbuf);

  if (pixbuf != NULL)
    g_object_unref (pixbuf);
}

/* Public */

/**
 * retro_cairo_display_set_core:
 * @self: a #RetroCairoDisplay
 * @core: (nullable): a #RetroCore, or %NULL
 *
 * Sets @core as the #RetroCore displayed by @self.
 */
void
retro_cairo_display_set_core (RetroCairoDisplay *self,
                              RetroCore         *core)
{
  g_return_if_fail (self != NULL);

  if (self->core == core)
    return;

  if (self->core != NULL) {
    g_signal_handler_disconnect (G_OBJECT (self->core), self->on_video_output_id);
    g_clear_object (&self->core);
  }

  if (core != NULL) {
    self->core = g_object_ref (core);
    self->on_video_output_id =
      g_signal_connect_object (core, "video-output",
                               (GCallback) on_video_output, self, 0);
  }
}

/**
 * retro_cairo_display_get_pixbuf:
 * @self: a #RetroCairoDisplay
 *
 * Gets the currently displayed video frame.
 *
 * Returns: (transfer none): a #GdkPixbuf
 */
GdkPixbuf *
retro_cairo_display_get_pixbuf (RetroCairoDisplay *self)
{
  g_return_val_if_fail (self != NULL, NULL);

  return self->pixbuf;
}

/**
 * retro_cairo_display_set_pixbuf:
 * @self: a #RetroCairoDisplay
 * @pixbuf: a #GdkPixbuf
 *
 * Sets @pixbuf as the currently displayed video frame.
 *
 * retro_pixbuf_set_aspect_ratio() can be used to specify the aspect ratio for
 * the pixbuf. Otherwise the core's aspect ratio will be used.
 */
void
retro_cairo_display_set_pixbuf (RetroCairoDisplay *self,
                                GdkPixbuf         *pixbuf)
{
  gfloat aspect_ratio;

  g_return_if_fail (self != NULL);

  if (self->pixbuf == pixbuf)
    return;

  g_clear_object (&self->pixbuf);

  if (pixbuf != NULL)
    self->pixbuf = g_object_ref (pixbuf);

  aspect_ratio = retro_pixbuf_get_aspect_ratio (pixbuf);
  if (aspect_ratio != 0.f)
    self->aspect_ratio = aspect_ratio;

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_PIXBUF]);
}

/**
 * retro_cairo_display_set_filter:
 * @self: a #RetroCairoDisplay
 * @filter: a #RetroVideoFilter
 *
 * Sets the video filter to use to render the core's video on @self.
 */
void
retro_cairo_display_set_filter (RetroCairoDisplay *self,
                                RetroVideoFilter   filter)
{
  g_return_if_fail (self != NULL);

  self->filter = filter;
  gtk_widget_queue_draw (GTK_WIDGET (self));
}

/**
 * retro_cairo_display_get_coordinates_on_display:
 * @self: a #RetroCairoDisplay
 * @widget_x: the abscissa on @self
 * @widget_y: the ordinate on @self
 * @display_x: return location for a the abscissa on the core's video display
 * @display_y: return location for a the ordinate on the core's video display
 *
 * Gets coordinates on the core's video output from coordinates on @self, and
 * whether the point is inside the core's video display.
 *
 * Returns: whether the coordinates are on the core's video display
 */
gboolean
retro_cairo_display_get_coordinates_on_display (RetroCairoDisplay *self,
                                                gdouble            widget_x,
                                                gdouble            widget_y,
                                                gdouble           *display_x,
                                                gdouble           *display_y)
{
  gdouble w = 0.0;
  gdouble h = 0.0;
  gdouble x = 0.0;
  gdouble y = 0.0;

  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (display_x != NULL, FALSE);
  g_return_val_if_fail (display_y != NULL, FALSE);

  get_video_box (self, &w, &h, &x, &y);

  // Return coordinates as a [-1.0, 1.0] scale, (0.0, 0.0) is the center.
  *display_x = ((widget_x - x) * 2.0 - w) / w;
  *display_y = ((widget_y - y) * 2.0 - h) / h;

  return (-1.0 <= *display_x) && (*display_x <= 1.0) &&
         (-1.0 <= *display_y) && (*display_y <= 1.0);
}

/**
 * retro_cairo_display_new:
 *
 * Creates a new #RetroCairoDisplay.
 *
 * Returns: (transfer full): a new #RetroCairoDisplay
 */
RetroCairoDisplay *
retro_cairo_display_new (void)
{
  return g_object_new (RETRO_TYPE_CAIRO_DISPLAY, NULL);
}
