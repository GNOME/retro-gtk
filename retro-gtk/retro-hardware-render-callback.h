// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_HARDWARE_RENDER_CALLBACK_H
#define RETRO_HARDWARE_RENDER_CALLBACK_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib.h>
#include <stdbool.h>
#include <stdint.h>

#include "retro-proc-address.h"

typedef enum _RetroHardwareContextType RetroHardwareContextType;
typedef struct _RetroHardwareRenderCallback RetroHardwareRenderCallback;

enum _RetroHardwareContextType
{
  RETRO_HW_CONTEXT_NONE = 0,
  /* OpenGL 2.x. Driver can choose to use latest compatibility context. */
  RETRO_HW_CONTEXT_OPENGL = 1,
  /* OpenGL ES 2.0. */
  RETRO_HW_CONTEXT_OPENGLES2 = 2,
  /* Modern desktop core GL context. Use version_major/
   * version_minor fields to set GL version. */
  RETRO_HW_CONTEXT_OPENGL_CORE = 3,
  /* OpenGL ES 3.0 */
  RETRO_HW_CONTEXT_OPENGLES3 = 4,
  /* OpenGL ES 3.1+. Set version_major/version_minor. For GLES2 and GLES3,
   * use the corresponding enums directly. */
  RETRO_HW_CONTEXT_OPENGLES_VERSION = 5,

  /* Vulkan, see RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE. */
  RETRO_HW_CONTEXT_VULKAN = 6,

  /* Direct3D, set version_major to select the type of interface
   * returned by RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE */
  RETRO_HW_CONTEXT_DIRECT3D = 7,

  RETRO_HW_CONTEXT_DUMMY = G_MAXINT,
};

/* Invalidates the current HW context.
 * Any GL state is lost, and must not be deinitialized explicitly.
 * If explicit deinitialization is desired by the libretro core,
 * it should implement context_destroy callback.
 * If called, all GPU resources must be reinitialized.
 * Usually called when frontend reinits video driver.
 * Also called first time video driver is initialized,
 * allowing libretro core to initialize resources.
 */
typedef void (*RetroHardwareContextReset) (void);

/* Gets current framebuffer which is to be rendered to.
 * Could change every frame potentially.
 */
typedef uintptr_t (*RetroHardwareGetCurrentFramebuffer) (void);

/* Get a symbol from HW context. */
typedef RetroProcAddress (*RetroHardwareGetProcAddress) (const gchar *symbol);

struct _RetroHardwareRenderCallback
{
  /* Which API to use. Set by libretro core. */
  RetroHardwareContextType context_type;

  /* Called when a context has been created or when it has been reset.
   * An OpenGL context is only valid after context_reset() has been called.
   *
   * When context_reset is called, OpenGL resources in the libretro
   * implementation are guaranteed to be invalid.
   *
   * It is possible that context_reset is called multiple times during an
   * application lifecycle.
   * If context_reset is called without any notification (context_destroy),
   * the OpenGL context was lost and resources should just be recreated
   * without any attempt to "free" old resources.
   */
  RetroHardwareContextReset context_reset;

  RetroHardwareGetCurrentFramebuffer get_current_framebuffer;
  RetroHardwareGetProcAddress get_proc_address;

  /* Set if render buffers should have depth component attached.
   * TODO: Obsolete. */
  bool depth;

  /* Set if stencil buffers should be attached.
   * TODO: Obsolete. */
  bool stencil;

  /* If depth and stencil are true, a packed 24/8 buffer will be added.
   * Only attaching stencil is invalid and will be ignored. */

  /* Use conventional bottom-left origin convention. If false,
   * standard libretro top-left origin semantics are used.
   * TODO: Move to GL specific interface. */
  bool bottom_left_origin;

  /* Major version number for core GL context or GLES 3.1+. */
  guint version_major;

  /* Minor version number for core GL context or GLES 3.1+. */
  guint version_minor;

  /* If this is true, the frontend will go very far to avoid
   * resetting context in scenarios like toggling fullscreen, etc.
   * TODO: Obsolete? Maybe frontend should just always assume this ...
   */
  bool cache_context;

  /* The reset callback might still be called in extreme situations
   * such as if the context is lost beyond recovery.
   *
   * For optimal stability, set this to false, and allow context to be
   * reset at any time.
   */

  /* A callback to be called before the context is destroyed in a
   * controlled way by the frontend. */
  RetroHardwareContextReset context_destroy;

  /* OpenGL resources can be deinitialized cleanly at this step.
   * context_destroy can be set to NULL, in which resources will
   * just be destroyed without any notification.
   *
   * Even when context_destroy is non-NULL, it is possible that
   * context_reset is called without any destroy notification.
   * This happens if context is lost by external factors (such as
   * notified by GL_ARB_robustness).
   *
   * In this case, the context is assumed to be already dead,
   * and the libretro implementation must not try to free any OpenGL
   * resources in the subsequent context_reset.
   */

  /* Creates a debug context. */
  bool debug_context;
};

#endif /* RETRO_HARDWARE_RENDER_CALLBACK_H */
