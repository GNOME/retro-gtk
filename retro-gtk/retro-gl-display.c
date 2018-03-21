// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-gl-display.h"

#include <epoxy/gl.h>
#include "retro-display.h"
#include "retro-glsl-filter.h"
#include "retro-pixdata.h"

struct _RetroGLDisplay
{
  GtkGLArea parent_instance;
  RetroCore *core;
  RetroPixdata *pixdata;
  GdkPixbuf *pixbuf;
  RetroVideoFilter filter;
  gfloat aspect_ratio;
  gulong on_video_output_id;

  RetroGLSLFilter *glsl_filter[RETRO_VIDEO_FILTER_COUNT];
  GLuint framebuffer;
  GLuint texture;
};

static void retro_display_interface_init (RetroDisplayInterface *iface);

G_DEFINE_TYPE_WITH_CODE (RetroGLDisplay, retro_gl_display, GTK_TYPE_GL_AREA,
                         G_IMPLEMENT_INTERFACE (RETRO_TYPE_DISPLAY,
                                                retro_display_interface_init))

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
  NULL,
  NULL,
  "resource:///org/gnome/Retro/glsl-filters/crt-simple.filter",
};

/* Private */

static void
retro_gl_display_clear_video (RetroGLDisplay *self)
{
  g_clear_object (&self->pixbuf);
  if (self->pixdata != NULL) {
    retro_pixdata_free (self->pixdata);
    self->pixdata = NULL;
  }
}

static void
retro_gl_display_set_pixdata (RetroGLDisplay *self,
                              RetroPixdata   *pixdata)
{
  if (self->pixdata == pixdata)
    return;

  retro_gl_display_clear_video (self);

  if (pixdata != NULL)
    self->pixdata = retro_pixdata_copy (pixdata);

  gtk_widget_queue_draw (GTK_WIDGET (self));
}

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

  g_return_if_fail (RETRO_IS_GL_DISPLAY (self));
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

static gboolean
retro_gl_display_load_texture (RetroGLDisplay *self,
                               gint           *texture_width,
                               gint           *texture_height)
{
  if (self->pixdata != NULL) {
    *texture_width = retro_pixdata_get_width (self->pixdata);
    *texture_height = retro_pixdata_get_height (self->pixdata);

    return retro_pixdata_load_gl_texture (self->pixdata);
  }

  if (retro_gl_display_get_pixbuf (self) == NULL)
    return FALSE;

  *texture_width = gdk_pixbuf_get_width (self->pixbuf),
  *texture_height = gdk_pixbuf_get_height (self->pixbuf),

  glTexImage2D (GL_TEXTURE_2D,
                0,
                GL_RGB,
                *texture_width,
                *texture_height,
                0,
                GL_RGBA, GL_UNSIGNED_BYTE,
                gdk_pixbuf_get_pixels (self->pixbuf));

  return TRUE;
}

static void
retro_gl_display_blit_texture (RetroGLDisplay *self,
                               GLenum          filter,
                               gint            texture_width,
                               gint            texture_height)
{
  gdouble w = 0.0;
  gdouble h = 0.0;
  gdouble x = 0.0;
  gdouble y = 0.0;

  retro_gl_display_get_video_box (self, &w, &h, &x, &y);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, self->framebuffer);
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, self->texture, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

  glBindFramebuffer (GL_READ_FRAMEBUFFER, self->framebuffer);
  glBlitFramebuffer (0, 0,
                     texture_width,
                     texture_height,
                     (GLint) x, (GLint) (y + h), (GLint) (x + w), (GLint) y,
                     GL_COLOR_BUFFER_BIT,
                     filter);
  glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
}

static void
retro_gl_display_draw_texture (RetroGLDisplay  *self,
                               RetroGLSLFilter *filter,
                               gint             texture_width,
                               gint             texture_height)
{
  GLfloat source_width, source_height;
  GLfloat target_width, target_height;
  GLfloat output_width, output_height;

  retro_glsl_filter_use_program (filter);

  retro_glsl_filter_apply_texture_params (filter);

  retro_glsl_filter_set_uniform_1f (filter, "relative_aspect_ratio",
    (gfloat) gtk_widget_get_allocated_width (GTK_WIDGET (self)) /
    (gfloat) gtk_widget_get_allocated_height (GTK_WIDGET (self)) /
    self->aspect_ratio);

  source_width = (GLfloat) texture_width;
  source_height = (GLfloat) texture_height;
  target_width = (GLfloat) gtk_widget_get_allocated_width (GTK_WIDGET (self));
  target_height = (GLfloat) gtk_widget_get_allocated_height (GTK_WIDGET (self));
  output_width = (GLfloat) gtk_widget_get_allocated_width (GTK_WIDGET (self));
  output_height = (GLfloat) gtk_widget_get_allocated_height (GTK_WIDGET (self));

  retro_glsl_filter_set_uniform_4f (filter, "sourceSize[0]",
                                    source_width, source_height,
                                    1.0f / source_width, 1.0f / source_height);

  retro_glsl_filter_set_uniform_4f (filter, "targetSize",
                                    target_width, target_height,
                                    1.0f / target_width, 1.0f / target_height);

  retro_glsl_filter_set_uniform_4f (filter, "outputSize",
                                    output_width, output_height,
                                    1.0f / output_width, 1.0f / output_height);

  glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

static void
retro_gl_display_realize (RetroGLDisplay *self)
{
  GLuint vertex_buffer_object;
  GLuint vertex_array_object;
  GLuint element_buffer_object;
  RetroVideoFilter filter;
  GError *inner_error = NULL;

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
    if (filter_uris[filter] == NULL)
      continue;

    self->glsl_filter[filter] = retro_glsl_filter_new (filter_uris[filter], NULL);
    retro_glsl_filter_prepare_program (self->glsl_filter[filter], &inner_error);
    if (G_UNLIKELY (inner_error != NULL)) {
      g_critical ("Shader program %s creation failed: %s",
                  filter_uris[filter],
                  inner_error->message);
      g_clear_object (&self->glsl_filter[filter]);
      g_clear_error (&inner_error);

      continue;
    }

    retro_glsl_filter_set_attribute_pointer (self->glsl_filter[filter],
                                             "position",
                                             sizeof (((RetroVertex *) NULL)->position) / sizeof (float),
                                             GL_FLOAT,
                                             GL_FALSE,
                                             sizeof (RetroVertex),
                                             (const GLvoid *) offsetof (RetroVertex, position));

    retro_glsl_filter_set_attribute_pointer (self->glsl_filter[filter],
                                             "texCoord",
                                             sizeof (((RetroVertex *) NULL)->texture_coordinates) / sizeof (float),
                                             GL_FLOAT,
                                             GL_FALSE,
                                             sizeof (RetroVertex),
                                             (const GLvoid *) offsetof (RetroVertex, texture_coordinates));
  }

  glDeleteFramebuffers (1, &self->framebuffer);
  self->framebuffer = 0;
  glGenFramebuffers(1, &self->framebuffer);

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

  glDeleteFramebuffers (1, &self->framebuffer);
  self->framebuffer = 0;
  glDeleteTextures (1, &self->texture);
  self->texture = 0;
  for (filter = 0; filter < RETRO_VIDEO_FILTER_COUNT; filter++)
    g_clear_object (&self->glsl_filter[filter]);
}

static gboolean
retro_gl_display_render (RetroGLDisplay *self)
{
  RetroVideoFilter filter;
  gint texture_width;
  gint texture_height;

  g_return_val_if_fail (RETRO_IS_GL_DISPLAY (self), FALSE);

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  filter = self->filter >= RETRO_VIDEO_FILTER_COUNT ?
    RETRO_VIDEO_FILTER_SMOOTH :
    self->filter;

  if (!retro_gl_display_load_texture (self, &texture_width, &texture_height))
    return FALSE;

  if (filter == RETRO_VIDEO_FILTER_SMOOTH) {
    retro_gl_display_blit_texture (self, GL_LINEAR, texture_width, texture_height);

    return FALSE;
  }

  if (filter == RETRO_VIDEO_FILTER_SHARP) {
    retro_gl_display_blit_texture (self, GL_NEAREST, texture_width, texture_height);

    return FALSE;
  }

  if (self->glsl_filter[filter] == NULL) {
    retro_gl_display_blit_texture (self, GL_LINEAR, texture_width, texture_height);

    return FALSE;
  }

  retro_gl_display_draw_texture (self, self->glsl_filter[filter], texture_width, texture_height);

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
retro_gl_display_class_init (RetroGLDisplayClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_gl_display_finalize;
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
}

static GdkPixbuf *
get_pixbuf (RetroDisplay *self) {
  return retro_gl_display_get_pixbuf (RETRO_GL_DISPLAY (self));
}

static void
set_pixbuf (RetroDisplay *self,
            GdkPixbuf    *pixbuf) {
  retro_gl_display_set_pixbuf (RETRO_GL_DISPLAY (self), pixbuf);
}

static void
set_core (RetroDisplay *self,
          RetroCore    *core) {
  retro_gl_display_set_core (RETRO_GL_DISPLAY (self), core);
}

static void
set_filter (RetroDisplay     *self,
            RetroVideoFilter  filter) {
  retro_gl_display_set_filter (RETRO_GL_DISPLAY (self), filter);
}

static gboolean
get_coordinates_on_display (RetroDisplay *self,
                            gdouble       widget_x,
                            gdouble       widget_y,
                            gdouble      *display_x,
                            gdouble      *display_y) {
  return retro_gl_display_get_coordinates_on_display (RETRO_GL_DISPLAY (self),
                                                      widget_x,
                                                      widget_y,
                                                      display_x,
                                                      display_y);
}

static void
retro_display_interface_init (RetroDisplayInterface *iface)
{
  iface->get_pixbuf = get_pixbuf;
  iface->set_pixbuf = set_pixbuf;
  iface->set_core = set_core;
  iface->set_filter = set_filter;
  iface->get_coordinates_on_display = get_coordinates_on_display;
}

static void
retro_gl_display_on_video_output (RetroCore    *sender,
                                  RetroPixdata *pixdata,
                                  gpointer      user_data)
{
  RetroGLDisplay *self = RETRO_GL_DISPLAY (user_data);

  g_return_if_fail (RETRO_IS_GL_DISPLAY (self));

  if (pixdata == NULL)
    return;

  self->aspect_ratio = retro_pixdata_get_aspect_ratio (pixdata);
  retro_gl_display_set_pixdata (self, pixdata);
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
  g_return_if_fail (RETRO_IS_GL_DISPLAY (self));

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
  g_return_val_if_fail (RETRO_IS_GL_DISPLAY (self), NULL);

  if (self->pixbuf != NULL)
    return self->pixbuf;

  if (self->pixdata != NULL)
    self->pixbuf = retro_pixdata_to_pixbuf (self->pixdata);

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
  g_return_if_fail (RETRO_IS_GL_DISPLAY (self));

  if (self->pixbuf == pixbuf)
    return;

  retro_gl_display_clear_video (self);

  if (pixbuf != NULL)
    self->pixbuf = g_object_ref (pixbuf);

  gtk_widget_queue_draw (GTK_WIDGET (self));
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
  g_return_if_fail (RETRO_IS_GL_DISPLAY (self));

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

  g_return_val_if_fail (RETRO_IS_GL_DISPLAY (self), FALSE);
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
