// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-glsl-filter-private.h"

struct _RetroGLSLFilter
{
  GObject parent_instance;
  RetroGLSLShader *shader;
};

G_DEFINE_TYPE (RetroGLSLFilter, retro_glsl_filter, G_TYPE_OBJECT)

#define GLSL_FILTER_GROUP "GLSL Filter"

static const gchar *
g_key_file_try_get_string (GKeyFile    *key_file,
                           const gchar *group,
                           const gchar *key)
{
  const gchar *value;
  GError *inner_error = NULL;

  value = g_key_file_get_string (key_file, group, key, &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_debug ("%s", inner_error->message);
    g_clear_error (&inner_error);

    return NULL;
  }

  return value;
}

static GBytes *
g_file_try_read_bytes (GFile *file)
{
  GFileInputStream *stream;
  goffset size;
  GBytes *bytes;
  GError *inner_error = NULL;

  stream = g_file_read (file, NULL, &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_debug ("%s", inner_error->message);
    g_clear_error (&inner_error);

    return NULL;
  }

  g_seekable_seek (G_SEEKABLE (stream), 0, G_SEEK_END, NULL, &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_debug ("%s", inner_error->message);
    g_clear_error (&inner_error);
    g_object_unref (stream);

    return NULL;
  }

  size = g_seekable_tell (G_SEEKABLE (stream));

  g_seekable_seek (G_SEEKABLE (stream), 0, G_SEEK_SET, NULL, &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_debug ("%s", inner_error->message);
    g_clear_error (&inner_error);
    g_object_unref (stream);

    return NULL;
  }

  bytes = g_input_stream_read_bytes (G_INPUT_STREAM (stream), size, NULL, &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_debug ("%s", inner_error->message);
    g_clear_error (&inner_error);
    g_object_unref (stream);

    return NULL;
  }

  g_object_unref (stream);

  return bytes;
}

static GBytes *
g_file_try_read_child_bytes (GFile       *parent,
                             const gchar *child_filename)
{
  GFile *file;
  GBytes *bytes;

  file = g_file_get_child (parent, child_filename);
  bytes = g_file_try_read_bytes (file);
  g_object_unref (file);

  return bytes;
}

static GBytes *
g_key_file_try_read_child_bytes (GKeyFile    *key_file,
                                 const gchar *group,
                                 const gchar *key,
                                 GFile       *parent)
{
  const gchar *value;

  value = g_key_file_try_get_string (key_file, group, key);
  if (value == NULL)
    return NULL;

  return g_file_try_read_child_bytes (parent, value);
}

RetroGLSLFilter *
retro_glsl_filter_new (const char  *uri,
                       GError     **error)
{
  g_autoptr (RetroGLSLFilter) self = NULL;
  g_autoptr (GBytes) vertex = NULL;
  g_autoptr (GBytes) fragment = NULL;
  GLenum wrap;
  GLenum filter;

  g_autoptr (GKeyFile) key_file = NULL;
  g_autoptr (GFile) file = NULL;
  g_autoptr (GFile) parent = NULL;
  g_autoptr (GBytes) bytes = NULL;
  const gchar *value;
  GError *inner_error = NULL;

  g_return_val_if_fail (uri != NULL, NULL);

  file = g_file_new_for_uri (uri);
  bytes = g_file_try_read_bytes (file);
  if (G_UNLIKELY (bytes == NULL))
    return NULL;

  key_file = g_key_file_new ();
  g_key_file_load_from_bytes (key_file, bytes, G_KEY_FILE_NONE, &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_propagate_error (error, inner_error);

    return NULL;
  }

  self = g_object_new (RETRO_TYPE_GLSL_FILTER, NULL);

  value = g_key_file_try_get_string (key_file, GLSL_FILTER_GROUP, "Filter");
  if (g_strcmp0 (value, "Linear") == 0)
    filter = GL_LINEAR;
  else if (g_strcmp0 (value, "Nearest") == 0)
    filter = GL_NEAREST;
  else
    filter = GL_LINEAR;

  value = g_key_file_try_get_string (key_file, GLSL_FILTER_GROUP, "Wrap");
  if (g_strcmp0 (value, "Border") == 0)
    wrap = GL_CLAMP_TO_BORDER;
  else if (g_strcmp0 (value, "Edge") == 0)
    wrap = GL_CLAMP_TO_EDGE;
  else
    wrap = GL_CLAMP_TO_BORDER;

  parent = g_file_get_parent (file);

  vertex = g_key_file_try_read_child_bytes (key_file,
                                            GLSL_FILTER_GROUP,
                                            "Vertex",
                                            parent);

  if (vertex == NULL)
    vertex = g_file_try_read_child_bytes (parent, "sharp.vs");

  fragment = g_key_file_try_read_child_bytes (key_file,
                                              GLSL_FILTER_GROUP,
                                              "Fragment",
                                              parent);

  if (fragment == NULL)
    fragment = g_file_try_read_child_bytes (parent, "sharp.fs");

  self->shader = retro_glsl_shader_new (vertex,
                                        fragment,
                                        wrap,
                                        filter,
                                        &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_propagate_error (error, inner_error);

    return NULL;
  }

  return g_steal_pointer (&self);
}

static void
retro_glsl_filter_finalize (GObject *object)
{
  RetroGLSLFilter *self = (RetroGLSLFilter *) object;

  g_clear_object (&self->shader);

  G_OBJECT_CLASS (retro_glsl_filter_parent_class)->finalize (object);
}

static void
retro_glsl_filter_class_init (RetroGLSLFilterClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_glsl_filter_finalize;
}

static void
retro_glsl_filter_init (RetroGLSLFilter *self)
{
}

RetroGLSLShader *
retro_glsl_filter_get_shader (RetroGLSLFilter *self)
{
  g_return_val_if_fail (RETRO_IS_GLSL_FILTER (self), NULL);

  return self->shader;
}
