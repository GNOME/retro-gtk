// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-glsl-shader-private.h"

struct _RetroGLSLShader
{
  GObject parent_instance;
  GBytes *vertex;
  GBytes *fragment;
  GLenum wrap;
  GLenum filter;
  GLuint program;
};

G_DEFINE_TYPE (RetroGLSLShader, retro_glsl_shader, G_TYPE_OBJECT)

#define RETRO_GLSL_SHADER_ERROR (retro_glsl_shader_error_quark ())

typedef enum {
  RETRO_GLSL_SHADER_ERROR_COULDNT_COMPILE,
  RETRO_GLSL_SHADER_ERROR_COULDNT_LINK
} RetroGLSLShaderError;

G_DEFINE_QUARK (retro-glsl-shader-error, retro_glsl_shader_error)

static GLuint
create_shader (GBytes  *source_bytes,
               GLenum   shader_type,
               GError **error)
{
  const gchar *source;
  gint size;
  GLuint shader;
  gint status;

  source = g_bytes_get_data (source_bytes, NULL);
  size = g_bytes_get_size (source_bytes);
  shader = glCreateShader (shader_type);
  glShaderSource (shader, 1, &source, &size);
  glCompileShader (shader);

  glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    g_autofree gchar *buffer = NULL;
    gint log_length = 0;

    glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &log_length);
    buffer = g_malloc (log_length + 1);
    glGetShaderInfoLog (shader, log_length, NULL, buffer);

    g_set_error (error, RETRO_GLSL_SHADER_ERROR, RETRO_GLSL_SHADER_ERROR_COULDNT_COMPILE,
                 "Compilation failure in %s shader: %s",
                 shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment",
                 buffer);

    glDeleteShader (shader);

    return 0;
  }

  return shader;
}

static void
retro_glsl_shader_finalize (GObject *object)
{
  RetroGLSLShader *self = (RetroGLSLShader *) object;

  if (self->program != 0) {
    glDeleteProgram (self->program);
    self->program = 0;
  }

  G_OBJECT_CLASS (retro_glsl_shader_parent_class)->finalize (object);
}

static void
retro_glsl_shader_class_init (RetroGLSLShaderClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_glsl_shader_finalize;
}

static void
retro_glsl_shader_init (RetroGLSLShader *self)
{
}

RetroGLSLShader *
retro_glsl_shader_new (GBytes  *vertex,
                       GBytes  *fragment,
                       GLenum   wrap,
                       GLenum   filter,
                       GError **error)
{
  g_autoptr (RetroGLSLShader) self = NULL;
  gint status = 0;
  GLuint vertex_shader;
  GLuint fragment_shader;
  GError *inner_error = NULL;

  g_return_val_if_fail (vertex != NULL, NULL);
  g_return_val_if_fail (fragment != NULL, NULL);
  g_return_val_if_fail (wrap == GL_CLAMP_TO_BORDER || wrap == GL_CLAMP_TO_EDGE, NULL);
  g_return_val_if_fail (filter == GL_LINEAR || filter == GL_NEAREST, NULL);

  self = g_object_new (RETRO_TYPE_GLSL_SHADER, NULL);

  self->vertex = vertex;
  self->fragment = fragment;
  self->wrap = wrap;
  self->filter = filter;

  vertex_shader = create_shader (self->vertex, GL_VERTEX_SHADER, &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_propagate_error (error, inner_error);

    return NULL;
  }

  fragment_shader = create_shader (self->fragment, GL_FRAGMENT_SHADER, &inner_error);
  if (G_UNLIKELY (inner_error != NULL)) {
    g_propagate_error (error, inner_error);
    glDeleteShader (vertex_shader);

    return NULL;
  }

  self->program = glCreateProgram ();
  glAttachShader (self->program, vertex_shader);
  glAttachShader (self->program, fragment_shader);
  glLinkProgram (self->program);

  glGetProgramiv (self->program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    g_autofree gchar *buffer = NULL;
    gint log_length = 0;

    glGetProgramiv (self->program, GL_INFO_LOG_LENGTH, &log_length);
    buffer = g_malloc (log_length + 1);
    glGetProgramInfoLog (self->program, log_length, NULL, buffer);

    g_set_error (error, RETRO_GLSL_SHADER_ERROR, RETRO_GLSL_SHADER_ERROR_COULDNT_LINK,
                 "Linking failure in program: %s", buffer);

    glDeleteShader (vertex_shader);
    glDeleteShader (fragment_shader);

    return NULL;
  }

  glDetachShader (self->program, vertex_shader);
  glDetachShader (self->program, fragment_shader);

  return g_steal_pointer (&self);
}

void
retro_glsl_shader_use_program (RetroGLSLShader *self)
{
  g_return_if_fail (RETRO_IS_GLSL_SHADER (self));
  g_return_if_fail (self->program != 0);

  glUseProgram (self->program);
}

void
retro_glsl_shader_apply_texture_params (RetroGLSLShader *self)
{
  g_return_if_fail (RETRO_IS_GLSL_SHADER (self));

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, self->wrap);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, self->wrap);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, self->filter);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, self->filter);
}

void
retro_glsl_shader_set_attribute_pointer (RetroGLSLShader *self,
                                         const gchar     *name,
                                         GLint            size,
                                         GLenum           type,
                                         GLboolean        normalized,
                                         GLsizei          stride,
                                         const GLvoid    *pointer)
{
  GLint location;

  g_return_if_fail (RETRO_IS_GLSL_SHADER (self));
  g_return_if_fail (self->program != 0);

  location = glGetAttribLocation (self->program, name);
  glVertexAttribPointer (location, size, type, normalized, stride, pointer);
  glEnableVertexAttribArray (location);
}

void
retro_glsl_shader_set_uniform_1f (RetroGLSLShader *self,
                                  const gchar     *name,
                                  gfloat           v0)
{
  GLint location;

  g_return_if_fail (RETRO_IS_GLSL_SHADER (self));
  g_return_if_fail (self->program != 0);

  location = glGetUniformLocation (self->program, name);
  glUniform1f (location, v0);
}

void
retro_glsl_shader_set_uniform_4f (RetroGLSLShader *self,
                                  const gchar     *name,
                                  gfloat           v0,
                                  gfloat           v1,
                                  gfloat           v2,
                                  gfloat           v3)
{
  GLint location;

  g_return_if_fail (RETRO_IS_GLSL_SHADER (self));
  g_return_if_fail (self->program != 0);

  location = glGetUniformLocation (self->program, name);
  glUniform4f (location, v0, v1, v2, v3);
}
