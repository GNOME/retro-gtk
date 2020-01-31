// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-core.h"
#include "retro-renderer-private.h"

G_BEGIN_DECLS

#define RETRO_TYPE_GL_RENDERER (retro_gl_renderer_get_type())

G_DECLARE_FINAL_TYPE (RetroGLRenderer, retro_gl_renderer, RETRO, GL_RENDERER, GObject)

RetroRenderer *retro_gl_renderer_new (RetroCore             *core,
                                      RetroHWRenderCallback *callback);

G_END_DECLS
