// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <epoxy/gl.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef void (*RetroGlDestroyFunc) (GLuint object);

typedef void (*RetroGlArrayDestroyFunc) (GLsizei       n_objects,
                                         const GLuint *objects);

static inline GLuint
retro_gl_steal_object (GLuint *object_p)
{
  GLuint object = *object_p;

  *object_p = 0;

  return object;
}

static inline void
retro_gl_clear_object (GLuint             *object_p,
                       RetroGlDestroyFunc  destroy)
{
  GLuint object = *object_p;

  if (object) {
    *object_p = 0;
    destroy (object);
  }
}

static inline void
retro_gl_clear_object_n (GLuint                  *objects_p,
                         GLsizei                  n_objects,
                         RetroGlArrayDestroyFunc  destroy)
{
  GLuint objects = *objects_p;

  if (objects) {
    *objects_p = 0;
    destroy (n_objects, &objects);
  }
}

static inline void
retro_gl_autocleanup_delete_shader (GLuint *object_p)
{
  if (*object_p == 0)
    return;

  glDeleteShader (*object_p);
}

#define retro_gl_autodelete_shader __attribute__((cleanup(retro_gl_autocleanup_delete_shader)))

G_END_DECLS
