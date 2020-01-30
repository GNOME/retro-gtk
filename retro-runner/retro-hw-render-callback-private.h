// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <stdbool.h>
#include "retro-proc-address-private.h"

G_BEGIN_DECLS

#define RETRO_HW_FRAME_BUFFER_VALID ((gpointer) -1)

typedef struct _RetroHWRenderCallback RetroHWRenderCallback;

typedef void (*RetroHWRenderCallbackContextReset) (void);
typedef guintptr (*RetroHWRenderCallbackGetCurrentFramebuffer) (void);
typedef RetroProcAddress (*RetroHWRenderCallbackGetProcAddress) (const gchar *sym);

typedef enum {
  RETRO_HW_CONTEXT_NONE = 0,
  RETRO_HW_CONTEXT_OPENGL,
  RETRO_HW_CONTEXT_OPENGLES2,
  RETRO_HW_CONTEXT_OPENGL_CORE,
  RETRO_HW_CONTEXT_OPENGLES3,
  RETRO_HW_CONTEXT_OPENGLES_VERSION,
  RETRO_HW_CONTEXT_VULKAN,
  RETRO_HW_CONTEXT_DIRECT3D,
  RETRO_HW_CONTEXT_DUMMY = G_MAXINT
} RetroHWContextType;

struct _RetroHWRenderCallback {
  RetroHWContextType context_type;
  RetroHWRenderCallbackContextReset context_reset;
  RetroHWRenderCallbackGetCurrentFramebuffer get_current_framebuffer;
  RetroHWRenderCallbackGetProcAddress get_proc_address;
  bool depth;
  bool stencil;
  bool bottom_left_origin;
  guint version_major;
  guint version_minor;
  bool cache_context;
  RetroHWRenderCallbackContextReset context_destroy;
  bool debug_context;
};

G_END_DECLS
