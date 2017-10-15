// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-gl-display.h"

#include <epoxy/gl.h>
#include "retro-pixdata.h"

struct _RetroGLDisplay
{
  GtkGLArea parent_instance;
  RetroCore *core;
  GdkPixbuf *pixbuf;
  RetroVideoFilter filter;
  gfloat aspect_ratio;
  gulong on_video_output_id;

  GLuint framebuffer;
};

G_DEFINE_TYPE (RetroGLDisplay, retro_gl_display, GTK_TYPE_GL_AREA)

enum {
  PROP_PIXBUF = 1,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

/* Private */

static void
retro_gl_display_get_video_box (RetroGLDisplay *self,
                                gdouble        *width,
                                gdouble        *height,
                                gdouble        *x,
                                gdouble        *y)
{
  gdouble w;
  gdouble h;
  gdouble display_ratio;
  gdouble allocated_ratio;

  g_return_if_fail (self != NULL);
  g_return_if_fail (width != NULL);
  g_return_if_fail (height != NULL);
  g_return_if_fail (x != NULL);
  g_return_if_fail (y != NULL);

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
retro_gl_display_realize (RetroGLDisplay *self)
{
  gtk_gl_area_make_current (GTK_GL_AREA (self));
}

static void
retro_gl_display_unrealize (RetroGLDisplay *self)
{
  gtk_gl_area_make_current (GTK_GL_AREA (self));

  glDeleteFramebuffers (1, &self->framebuffer);
}

static gboolean
retro_gl_display_render (RetroGLDisplay *self)
{
  gdouble w = 0.0;
  gdouble h = 0.0;
  gdouble x = 0.0;
  gdouble y = 0.0;
  GLenum filter;

  g_return_val_if_fail (self != NULL, FALSE);

  retro_gl_display_get_video_box (self, &w, &h, &x, &y);

  switch (self->filter) {
  case RETRO_VIDEO_FILTER_SHARP:
    filter = GL_NEAREST;

    break;
  default:
  case RETRO_VIDEO_FILTER_SMOOTH:
    filter = GL_LINEAR;

    break;
  }

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindFramebuffer (GL_READ_FRAMEBUFFER, self->framebuffer);
  glBlitFramebuffer (0, 0,
                     gdk_pixbuf_get_width (self->pixbuf),
                     gdk_pixbuf_get_height (self->pixbuf),
                     (GLint) x, (GLint) (y + h), (GLint) (x + w), (GLint) y,
                     GL_COLOR_BUFFER_BIT,
                     filter);
  glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);

  return FALSE;
}

static void
retro_gl_display_finalize (GObject *object)
{
  RetroGLDisplay *self = (RetroGLDisplay *) object;

  if (self->core != NULL)
    g_object_unref (self->core);
  if (self->pixbuf != NULL)
    g_object_unref (self->pixbuf);

  G_OBJECT_CLASS (retro_gl_display_parent_class)->finalize (object);
}

static void
retro_gl_display_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  RetroGLDisplay *self = RETRO_GL_DISPLAY (object);

  switch (prop_id) {
  case PROP_PIXBUF:
    g_value_set_object (value, retro_gl_display_get_pixbuf (self));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_gl_display_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  RetroGLDisplay *self = RETRO_GL_DISPLAY (object);

  switch (prop_id) {
  case PROP_PIXBUF:
    retro_gl_display_set_pixbuf (self, g_value_get_object (value));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_gl_display_class_init (RetroGLDisplayClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_gl_display_finalize;
  object_class->get_property = retro_gl_display_get_property;
  object_class->set_property = retro_gl_display_set_property;

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
}

static void
queue_draw (GObject    *sender,
            GParamSpec *pspec,
            gpointer    self)
{
  gtk_widget_queue_draw (GTK_WIDGET (self));
}

static void
retro_gl_display_init (RetroGLDisplay *self)
{
  g_signal_connect_object (G_OBJECT (self),
                           "realize",
                           (GCallback) retro_gl_display_realize,
                           GTK_WIDGET (self),
                           0);

  g_signal_connect_object (G_OBJECT (self),
                           "unrealize",
                           (GCallback) retro_gl_display_unrealize,
                           GTK_WIDGET (self),
                           0);

  g_signal_connect_object (G_OBJECT (self),
                           "render",
                           (GCallback) retro_gl_display_render,
                           GTK_WIDGET (self),
                           0);

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
retro_gl_display_on_video_output (RetroCore    *sender,
                                  RetroPixdata *pixdata,
                                  gpointer      user_data)
{
  RetroGLDisplay *self = RETRO_GL_DISPLAY (user_data);

  GdkPixbuf *pixbuf;

  g_return_if_fail (self != NULL);

  self->aspect_ratio = retro_pixdata_get_aspect_ratio (pixdata);
  pixbuf = retro_pixdata_to_pixbuf (pixdata);
  retro_gl_display_set_pixbuf (self, pixbuf);

  if (pixbuf != NULL)
    g_object_unref (pixbuf);
}

/* Public */

/**
 * retro_gl_display_set_core:
 * @self: a #RetroGLDisplay
 * @core: (nullable): a #RetroCore, or %NULL
 *
 * Sets @core as the #RetroCore displayed by @self.
 */
void
retro_gl_display_set_core (RetroGLDisplay *self,
                           RetroCore      *core)
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
    self->on_video_output_id = g_signal_connect_object (core, "video-output", (GCallback) retro_gl_display_on_video_output, self, 0);
  }
}

/**
 * retro_gl_display_get_pixbuf:
 * @self: a #RetroGLDisplay
 *
 * Gets the currently displayed video frame.
 *
 * Returns: (transfer none): a #GdkPixbuf
 */
GdkPixbuf *
retro_gl_display_get_pixbuf (RetroGLDisplay *self)
{
  g_return_val_if_fail (self != NULL, NULL);

  return self->pixbuf;
}

/**
 * retro_gl_display_set_pixbuf:
 * @self: a #RetroGLDisplay
 * @pixbuf: a #GdkPixbuf
 *
 * Sets @pixbuf as the currently displayed video frame.
 */
void
retro_gl_display_set_pixbuf (RetroGLDisplay *self,
                             GdkPixbuf      *pixbuf)
{
  g_return_if_fail (self != NULL);

  if (self->pixbuf == pixbuf)
    return;

  g_clear_object (&self->pixbuf);

  if (pixbuf != NULL) {
    self->pixbuf = g_object_ref (pixbuf);

    GLuint tex = 0;
    glGenTextures (1, &tex);
    glBindTexture (GL_TEXTURE_2D, tex);

    glTexImage2D (GL_TEXTURE_2D,
                  0,
                  GL_RGB,
                  gdk_pixbuf_get_width (self->pixbuf),
                  gdk_pixbuf_get_height (self->pixbuf),
                  0,
                  GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV,
                  gdk_pixbuf_get_pixels (self->pixbuf));

    glGenFramebuffers(1, &self->framebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, self->framebuffer);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, tex, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  }

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_PIXBUF]);
}

/**
 * retro_gl_display_set_filter:
 * @self: a #RetroGLDisplay
 * @filter: a #RetroVideoFilter
 *
 * Sets the video filter to use to render the core's video on @self.
 */
void
retro_gl_display_set_filter (RetroGLDisplay   *self,
                             RetroVideoFilter  filter)
{
  g_return_if_fail (self != NULL);

  self->filter = filter;
  gtk_widget_queue_draw (GTK_WIDGET (self));
}

/**
 * retro_gl_display_get_coordinates_on_display:
 * @self: a #RetroGLDisplay
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
retro_gl_display_get_coordinates_on_display (RetroGLDisplay *self,
                                             gdouble         widget_x,
                                             gdouble         widget_y,
                                             gdouble        *display_x,
                                             gdouble        *display_y)
{
  gdouble w = 0.0;
  gdouble h = 0.0;
  gdouble x = 0.0;
  gdouble y = 0.0;

  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (display_x != NULL, FALSE);
  g_return_val_if_fail (display_y != NULL, FALSE);

  retro_gl_display_get_video_box (self, &w, &h, &x, &y);

  // Return coordinates as a [-1.0, 1.0] scale, (0.0, 0.0) is the center.
  *display_x = ((widget_x - x) * 2.0 - w) / w;
  *display_y = ((widget_y - y) * 2.0 - h) / h;

  return (-1.0 <= *display_x) && (*display_x <= 1.0) &&
         (-1.0 <= *display_y) && (*display_y <= 1.0);
}

/**
 * retro_gl_display_new:
 *
 * Creates a new #RetroGLDisplay.
 *
 * Returns: (transfer full): a new #RetroGLDisplay
 */
RetroGLDisplay *
retro_gl_display_new (void)
{
  return g_object_new (RETRO_TYPE_GL_DISPLAY, NULL);
}
