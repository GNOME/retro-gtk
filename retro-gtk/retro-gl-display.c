// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-gl-display.h"

#include <epoxy/gl.h>
#include "retro-glsl-filter.h"
#include "retro-pixdata.h"

struct _RetroGLDisplay
{
  GtkGLArea parent_instance;
  RetroCore *core;
  GdkPixbuf *pixbuf;
  RetroVideoFilter filter;
  gfloat aspect_ratio;
  gulong on_video_output_id;

  RetroGLSLFilter *glsl_filter[RETRO_VIDEO_FILTER_COUNT];
  GLuint texture;
};

G_DEFINE_TYPE (RetroGLDisplay, retro_gl_display, GTK_TYPE_GL_AREA)

enum {
  PROP_PIXBUF = 1,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

typedef struct {
  struct {
    float x, y;
  } position;
  struct {
    float x, y;
  } texture_coordinates;
} RetroVertex;

static float vertices[] = {
  -1.0f,  1.0f, 0.0f, 0.0f, // Top-left
   1.0f,  1.0f, 1.0f, 0.0f, // Top-right
   1.0f, -1.0f, 1.0f, 1.0f, // Bottom-right
  -1.0f, -1.0f, 0.0f, 1.0f, // Bottom-left
};

static GLuint elements[] = {
    0, 1, 2,
    2, 3, 0,
};

static const gchar *filter_uris[] = {
  "resource:///org/gnome/Retro/glsl-filters/bicubic.filter",
  "resource:///org/gnome/Retro/glsl-filters/sharp.filter",
};

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
  GLuint vertex_buffer_object;
  GLuint vertex_array_object;
  GLuint element_buffer_object;
  RetroVideoFilter filter;

  gtk_gl_area_make_current (GTK_GL_AREA (self));

  glGenBuffers (1, &vertex_buffer_object);
  glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_object);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

  glGenVertexArrays (1, &vertex_array_object);
  glBindVertexArray (vertex_array_object);

  glGenBuffers (1, &element_buffer_object);
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
  glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (elements), elements, GL_STATIC_DRAW);

  for (filter = 0; filter < RETRO_VIDEO_FILTER_COUNT; filter++) {
    self->glsl_filter[filter] = retro_glsl_filter_new (filter_uris[filter], NULL);
    retro_glsl_filter_prepare_program (self->glsl_filter[filter]);

    retro_glsl_filter_set_attribute_pointer (self->glsl_filter[filter],
                                             "position",
                                             sizeof (((RetroVertex *) NULL)->position) / sizeof (float),
                                             GL_FLOAT,
                                             GL_FALSE,
                                             sizeof (RetroVertex),
                                             offsetof (RetroVertex, position));

    retro_glsl_filter_set_attribute_pointer (self->glsl_filter[filter],
                                             "texCoord",
                                             sizeof (((RetroVertex *) NULL)->texture_coordinates) / sizeof (float),
                                             GL_FLOAT,
                                             GL_FALSE,
                                             sizeof (RetroVertex),
                                             offsetof (RetroVertex, texture_coordinates));
  }

  glDeleteTextures (1, &self->texture);
  self->texture = 0;
  glGenTextures (1, &self->texture);
  glBindTexture (GL_TEXTURE_2D, self->texture);

  filter = self->filter >= RETRO_VIDEO_FILTER_COUNT ?
    RETRO_VIDEO_FILTER_SMOOTH :
    self->filter;

  retro_glsl_filter_use_program (self->glsl_filter[filter]);
}

static void
retro_gl_display_unrealize (RetroGLDisplay *self)
{
  RetroVideoFilter filter;

  gtk_gl_area_make_current (GTK_GL_AREA (self));

  glDeleteTextures (1, &self->texture);
  self->texture = 0;
  for (filter = 0; filter < RETRO_VIDEO_FILTER_COUNT; filter++)
    g_clear_object (&self->glsl_filter[filter]);
}

static gboolean
retro_gl_display_render (RetroGLDisplay *self)
{
  gdouble w = 0.0;
  gdouble h = 0.0;
  gdouble x = 0.0;
  gdouble y = 0.0;
  GLfloat source_width, source_height;
  GLfloat target_width, target_height;
  GLfloat output_width, output_height;
  RetroVideoFilter filter;

  g_return_val_if_fail (self != NULL, FALSE);

  retro_gl_display_get_video_box (self, &w, &h, &x, &y);

  filter = self->filter >= RETRO_VIDEO_FILTER_COUNT ?
    RETRO_VIDEO_FILTER_SMOOTH :
    self->filter;

  retro_glsl_filter_use_program (self->glsl_filter[filter]);

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (self->pixbuf == NULL)
    return FALSE;

  glTexImage2D (GL_TEXTURE_2D,
                0,
                GL_RGB,
                gdk_pixbuf_get_width (self->pixbuf),
                gdk_pixbuf_get_height (self->pixbuf),
                0,
                GL_RGBA, GL_UNSIGNED_BYTE,
                gdk_pixbuf_get_pixels (self->pixbuf));

  retro_glsl_filter_apply_texture_params (self->glsl_filter[filter]);

  retro_glsl_filter_set_uniform_1f (self->glsl_filter[filter], "relative_aspect_ratio",
    (gfloat) gtk_widget_get_allocated_width (GTK_WIDGET (self)) /
    (gfloat) gtk_widget_get_allocated_height (GTK_WIDGET (self)) /
    self->aspect_ratio);

  source_width = (GLfloat) gdk_pixbuf_get_width (self->pixbuf);
  source_height = (GLfloat) gdk_pixbuf_get_height (self->pixbuf);
  target_width = (GLfloat) gtk_widget_get_allocated_width (GTK_WIDGET (self));
  target_height = (GLfloat) gtk_widget_get_allocated_height (GTK_WIDGET (self));
  output_width = (GLfloat) gtk_widget_get_allocated_width (GTK_WIDGET (self));
  output_height = (GLfloat) gtk_widget_get_allocated_height (GTK_WIDGET (self));

  retro_glsl_filter_set_uniform_4f (self->glsl_filter[filter], "sourceSize[0]",
                                    source_width, source_height,
                                    1.0f / source_width, 1.0f / source_height);

  retro_glsl_filter_set_uniform_4f (self->glsl_filter[filter], "targetSize",
                                    target_width, target_height,
                                    1.0f / target_width, 1.0f / target_height);

  retro_glsl_filter_set_uniform_4f (self->glsl_filter[filter], "outputSize",
                                    output_width, output_height,
                                    1.0f / output_width, 1.0f / output_height);

  glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  return FALSE;
}

static void
retro_gl_display_finalize (GObject *object)
{
  RetroGLDisplay *self = (RetroGLDisplay *) object;
  RetroVideoFilter filter;

  glDeleteTextures (1, &self->texture);
  self->texture = 0;
  for (filter = 0; filter < RETRO_VIDEO_FILTER_COUNT; filter++)
    g_clear_object (&self->glsl_filter[filter]);
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

  if (pixbuf != NULL)
    self->pixbuf = g_object_ref (pixbuf);

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
