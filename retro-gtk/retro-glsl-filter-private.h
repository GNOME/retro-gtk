// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#include <epoxy/gl.h>
#include <gio/gio.h>
#include <glib-object.h>

#include "retro-glsl-shader-private.h"
G_BEGIN_DECLS

#define RETRO_TYPE_GLSL_FILTER (retro_glsl_filter_get_type())

G_DECLARE_FINAL_TYPE (RetroGLSLFilter, retro_glsl_filter, RETRO, GLSL_FILTER, GObject)

RetroGLSLFilter *retro_glsl_filter_new (const char  *uri,
                                        GError     **error);
RetroGLSLShader *retro_glsl_filter_get_shader (RetroGLSLFilter *self);

G_END_DECLS
