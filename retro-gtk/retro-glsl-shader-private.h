// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#include <epoxy/gl.h>
#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_GLSL_SHADER (retro_glsl_shader_get_type())

G_DECLARE_FINAL_TYPE (RetroGLSLShader, retro_glsl_shader, RETRO, GLSL_SHADER, GObject)

RetroGLSLShader *retro_glsl_shader_new (GBytes  *vertex,
                                        GBytes  *fragment,
                                        GLenum   wrap,
                                        GLenum   filter,
                                        GError **error);
void retro_glsl_shader_use_program (RetroGLSLShader *self);
void retro_glsl_shader_apply_texture_params (RetroGLSLShader *self);
void retro_glsl_shader_set_attribute_pointer (RetroGLSLShader *self,
                                              const gchar     *name,
                                              GLint            size,
                                              GLenum           type,
                                              GLboolean        normalized,
                                              GLsizei          stride,
                                              const GLvoid    *pointer);
void retro_glsl_shader_set_uniform_1f (RetroGLSLShader *self,
                                       const gchar     *name,
                                       gfloat           v0);
void retro_glsl_shader_set_uniform_4f (RetroGLSLShader *self,
                                       const gchar     *name,
                                       gfloat           v0,
                                       gfloat           v1,
                                       gfloat           v2,
                                       gfloat           v3);

G_END_DECLS
