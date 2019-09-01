#pragma once

#include <epoxy/gl.h>
#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_GLSL_FILTER (retro_glsl_filter_get_type())

G_DECLARE_FINAL_TYPE (RetroGLSLFilter, retro_glsl_filter, RETRO, GLSL_FILTER, GObject)

RetroGLSLFilter *retro_glsl_filter_new (const char  *uri,
                                        GError     **error);
void retro_glsl_filter_apply_texture_params (RetroGLSLFilter *self);
void retro_glsl_filter_prepare_program (RetroGLSLFilter  *self,
                                        GError          **error);
void retro_glsl_filter_use_program (RetroGLSLFilter *self);
void retro_glsl_filter_set_attribute_pointer (RetroGLSLFilter *self,
                                              const gchar     *name,
                                              GLint            size,
                                              GLenum           type,
                                              GLboolean        normalized,
                                              GLsizei          stride,
                                              const GLvoid    *pointer);
void retro_glsl_filter_set_uniform_1f (RetroGLSLFilter *self,
                                       const gchar     *name,
                                       gfloat           v0);
void retro_glsl_filter_set_uniform_4f (RetroGLSLFilter *self,
                                       const gchar     *name,
                                       gfloat           v0,
                                       gfloat           v1,
                                       gfloat           v2,
                                       gfloat           v3);

G_END_DECLS
