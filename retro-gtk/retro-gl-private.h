// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <epoxy/gl.h>
#include <glib-object.h>

G_BEGIN_DECLS

static inline GLuint
retro_gl_steal_object (GLuint *ptr)
{
  GLuint ref = *ptr;

  *ptr = 0;

  return ref;
}

static inline void
retro_gl_autocleanup_delete_shader (GLuint *ptr)
{
  if (*ptr == 0)
    return;

  glDeleteShader (*ptr);
}

#define retro_gl_autodelete_shader __attribute__((cleanup(retro_gl_autocleanup_delete_shader)))

G_END_DECLS
