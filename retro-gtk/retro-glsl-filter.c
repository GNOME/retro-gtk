// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-glsl-filter-private.h"

struct _RetroGLSLFilter
{
  GObject parent_instance;
  GBytes *vertex;
  GBytes *fragment;
  GLenum wrap;
  GLenum filter;
  GLuint program;
};

G_DEFINE_TYPE (RetroGLSLFilter, retro_glsl_filter, G_TYPE_OBJECT)

#define GLSL_FILTER_GROUP "GLSL Filter"

#define RETRO_GLSL_FILTER_ERROR (retro_glsl_filter_error_quark ())

typedef enum {
  RETRO_GLSL_FILTER_ERROR_SHADER_COMPILATION,
  RETRO_GLSL_FILTER_ERROR_SHADER_LINK
} RetroGLSLFilterError;

G_DEFINE_QUARK (retro-glsl-filter-error, retro_glsl_filter_error)

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
  RetroGLSLFilter *self;
  GKeyFile *key_file;
  GFile *file;
  GFile *parent;
  GBytes *bytes;
  const gchar *value;
  GError *inner_error = NULL;

  g_return_val_if_fail (uri != NULL, NULL);

  file = g_file_new_for_uri (uri);
  bytes = g_file_try_read_bytes (file);
  if (G_UNLIKELY (bytes == NULL)) {
    g_object_unref (file);

    return NULL;
  }

  key_file = g_key_file_new ();
  g_key_file_load_from_bytes (key_file, bytes, G_KEY_FILE_NONE, &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_propagate_error (error, inner_error);
    g_object_unref (file);
    g_bytes_unref (bytes);
    g_key_file_unref (key_file);

    return NULL;
  }

  g_bytes_unref (bytes);

  self = g_object_new (RETRO_TYPE_GLSL_FILTER, NULL);

  value = g_key_file_try_get_string (key_file, GLSL_FILTER_GROUP, "Filter");
  if (g_strcmp0 (value, "Linear") == 0)
    self->filter = GL_LINEAR;
  else if (g_strcmp0 (value, "Nearest") == 0)
    self->filter = GL_NEAREST;
  else
    self->filter = GL_LINEAR;

  value = g_key_file_try_get_string (key_file, GLSL_FILTER_GROUP, "Wrap");
  if (g_strcmp0 (value, "Border") == 0)
    self->wrap = GL_CLAMP_TO_BORDER;
  else if (g_strcmp0 (value, "Edge") == 0)
    self->wrap = GL_CLAMP_TO_EDGE;
  else
    self->wrap = GL_CLAMP_TO_BORDER;

  parent = g_file_get_parent (file);
  g_object_unref (file);

  self->vertex = g_key_file_try_read_child_bytes (key_file,
                                                  GLSL_FILTER_GROUP,
                                                  "Vertex",
                                                  parent);

  if (self->vertex == NULL)
    self->vertex = g_file_try_read_child_bytes (parent, "sharp.vs");

  self->fragment = g_key_file_try_read_child_bytes (key_file,
                                                    GLSL_FILTER_GROUP,
                                                    "Fragment",
                                                    parent);

  if (self->fragment == NULL)
    self->fragment = g_file_try_read_child_bytes (parent, "sharp.fs");

  g_object_unref (parent);
  g_key_file_unref (key_file);

  return self;
}

static void
retro_glsl_filter_finalize (GObject *object)
{
  RetroGLSLFilter *self = (RetroGLSLFilter *) object;

  if (self->vertex != NULL)
    g_bytes_unref (self->vertex);
  if (self->fragment != NULL)
    g_bytes_unref (self->fragment);
  glDeleteProgram (self->program);
  self->program = 0;

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

void
retro_glsl_filter_apply_texture_params (RetroGLSLFilter *self)
{
  g_return_if_fail (RETRO_IS_GLSL_FILTER (self));

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, self->wrap);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, self->wrap);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, self->filter);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, self->filter);
}

static GLuint
create_shader (GBytes  *source_bytes,
               GLenum   shader_type,
               GError **error)
{
  const gchar *source;
  gint size;
  GLuint shader;
  gint status;
  gint log_length;
  gchar *buffer;

  source = g_bytes_get_data (source_bytes, NULL);
  size = g_bytes_get_size (source_bytes);
  shader = glCreateShader (shader_type);
  glShaderSource (shader, 1, &source, &size);
  glCompileShader (shader);

  status = 0;
  glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &log_length);
    buffer = g_malloc (log_length + 1);
    glGetShaderInfoLog (shader, log_length, NULL, buffer);

    g_set_error (error, RETRO_GLSL_FILTER_ERROR, RETRO_GLSL_FILTER_ERROR_SHADER_COMPILATION,
                 "Compilation failure in %s shader: %s",
                 shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment",
                 buffer);

    g_free (buffer);
    glDeleteShader (shader);

    return 0;
  }

  return shader;
}

void
retro_glsl_filter_prepare_program (RetroGLSLFilter  *self,
                                   GError          **error)
{
  gint status;
  gint log_length;
  gchar *buffer;
  GLuint vertex_shader;
  GLuint fragment_shader;
  GError *inner_error = NULL;

  g_return_if_fail (RETRO_IS_GLSL_FILTER (self));
  g_return_if_fail (self->program == 0);

  vertex_shader = create_shader (self->vertex, GL_VERTEX_SHADER, &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_propagate_error (error, inner_error);
    self->program = 0;

    return;
  }

  fragment_shader = create_shader (self->fragment, GL_FRAGMENT_SHADER, &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_propagate_error (error, inner_error);
    glDeleteShader (vertex_shader);
    self->program = 0;

    return;
  }

  self->program = glCreateProgram();
  glAttachShader (self->program, vertex_shader);
  glAttachShader (self->program, fragment_shader);
  glLinkProgram (self->program);

  status = 0;
  glGetProgramiv (self->program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    log_length = 0;
    glGetProgramiv (self->program, GL_INFO_LOG_LENGTH, &log_length);
    buffer = g_malloc (log_length + 1);
    glGetProgramInfoLog (self->program, log_length, NULL, buffer);

    g_set_error (error, RETRO_GLSL_FILTER_ERROR, RETRO_GLSL_FILTER_ERROR_SHADER_LINK,
                 "Linking failure in program: %s", buffer);
    g_free (buffer);

    glDeleteShader (vertex_shader);
    glDeleteShader (fragment_shader);
    glDeleteProgram (self->program);
    self->program = 0;

    return;
  }

  glDetachShader (self->program, vertex_shader);
  glDetachShader (self->program, fragment_shader);
}

void
retro_glsl_filter_use_program (RetroGLSLFilter *self)
{
  g_return_if_fail (RETRO_IS_GLSL_FILTER (self));
  g_return_if_fail (self->program != 0);

  glUseProgram (self->program);
}

void
retro_glsl_filter_set_attribute_pointer (RetroGLSLFilter *self,
                                         const gchar     *name,
                                         GLint            size,
                                         GLenum           type,
                                         GLboolean        normalized,
                                         GLsizei          stride,
                                         const GLvoid    *pointer)
{
  GLint location;

  g_return_if_fail (RETRO_IS_GLSL_FILTER (self));
  g_return_if_fail (self->program != 0);

  location = glGetAttribLocation (self->program, name);
  glVertexAttribPointer (location, size, type, normalized, stride, pointer);
  glEnableVertexAttribArray (location);
}


void
retro_glsl_filter_set_uniform_1f (RetroGLSLFilter *self,
                                  const gchar     *name,
                                  gfloat           v0)
{
  GLint location;

  g_return_if_fail (RETRO_IS_GLSL_FILTER (self));
  g_return_if_fail (self->program != 0);

  location = glGetUniformLocation (self->program, name);
  glUniform1f (location, v0);
}

void
retro_glsl_filter_set_uniform_4f (RetroGLSLFilter *self,
                                  const gchar     *name,
                                  gfloat           v0,
                                  gfloat           v1,
                                  gfloat           v2,
                                  gfloat           v3)
{
  GLint location;

  g_return_if_fail (RETRO_IS_GLSL_FILTER (self));
  g_return_if_fail (self->program != 0);

  location = glGetUniformLocation (self->program, name);
  glUniform4f (location, v0, v1, v2, v3);
}
