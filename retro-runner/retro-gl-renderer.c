// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-gl-renderer-private.h"

#include <gio/gio.h>
#include "epoxy/egl.h"

#define MAX_EGL_ATTRS 30

struct _RetroGLRenderer
{
  GObject parent_instance;
  RetroCore *core;
  RetroHWRenderCallback *callback;

  EGLDisplay display;
  EGLContext context;
  GModule *gl_module;

  guint framebuffer;
  guint renderbuffer;
  guint texture;

  guint8 *buf_flip;
  gsize last_size;
};

static void retro_renderer_interface_init (RetroRendererInterface *iface);

G_DEFINE_TYPE_WITH_CODE (RetroGLRenderer, retro_gl_renderer, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (RETRO_TYPE_RENDERER,
                                                retro_renderer_interface_init))

static void
check_egl_errors (const gchar *msg)
{
  EGLint err;

  while ((err = eglGetError ()) != EGL_SUCCESS)
    g_critical ("EGL error 0x%x at %s()", err, msg);
}

static void
check_gl_errors (const gchar *msg)
{
  GLenum err;

  while ((err = glGetError ()) != GL_NO_ERROR)
    g_critical ("OpenGL error 0x%x at %s", err, msg);
}

static void
init_framebuffer (RetroGLRenderer *self,
                  guint            width,
                  guint            height)
{
  GLenum status;
  GLint max_fbo_size, max_rb_size;

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, &max_fbo_size);
  glGetIntegerv (GL_MAX_RENDERBUFFER_SIZE, &max_rb_size);

  width = MIN (width, MIN (max_fbo_size, max_rb_size));
  height = MIN (height, MIN (max_fbo_size, max_rb_size));

  glGenFramebuffers (1, &self->framebuffer);
  glBindFramebuffer (GL_FRAMEBUFFER, self->framebuffer);

  glGenTextures (1, &self->texture);
  glBindTexture (GL_TEXTURE_2D, self->texture);
  glTexStorage2D (GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
  glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                          GL_TEXTURE_2D, self->texture, 0);

  if (self->callback->depth) {
    glGenRenderbuffers (1, &self->renderbuffer);
    glBindRenderbuffer (GL_RENDERBUFFER, self->renderbuffer);
    if (self->callback->stencil)
      glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    else
      glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glBindRenderbuffer (GL_RENDERBUFFER, 0);

    if (self->callback->stencil)
      glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                 GL_RENDERBUFFER, self->renderbuffer);
    else
      glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                 GL_RENDERBUFFER, self->renderbuffer);
  } else {
    self->renderbuffer = 0;
  }

  check_gl_errors ("init_framebuffer");

  status = glCheckFramebufferStatus (GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
     g_critical ("Framebuffer not complete: %d", status);

  glClearColor (0, 0, 0, 1);
  if (self->callback->depth && self->callback->stencil)
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  else if (self->callback->depth)
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  else
    glClear (GL_COLOR_BUFFER_BIT);

  glBindTexture (GL_TEXTURE_2D, 0);
  glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

static void
retro_gl_renderer_realize (RetroRenderer *renderer,
                           guint          width,
                           guint          height)
{
  RetroGLRenderer *self = RETRO_GL_RENDERER (renderer);

  eglMakeCurrent (self->display, EGL_NO_SURFACE, EGL_NO_SURFACE, self->context);
  check_egl_errors ("eglMakeCurrent");

  init_framebuffer (self, width, height);

  self->callback->context_reset ();
}

static RetroProcAddress
retro_gl_renderer_get_proc_address (RetroRenderer *renderer,
                                    const gchar   *sym)
{
  RetroGLRenderer *self = RETRO_GL_RENDERER (renderer);
  RetroProcAddress *out = NULL;
  g_autofree gchar *sym_mangled = NULL;

  sym_mangled = g_strdup_printf ("epoxy_%s", sym);
  g_module_symbol (self->gl_module, sym_mangled, (gpointer) &out);

  if (!out)
    return NULL;

  return *out;
}

static guintptr
retro_gl_renderer_get_current_framebuffer (RetroRenderer *renderer)
{
  RetroGLRenderer *self = RETRO_GL_RENDERER (renderer);

  return self->framebuffer;
}

static void
retro_gl_renderer_snapshot (RetroRenderer    *renderer,
                            RetroPixelFormat  pixel_format,
                            guint             width,
                            guint             height,
                            gsize             rowstride,
                            guint8           *data)
{
  RetroGLRenderer *self = RETRO_GL_RENDERER (renderer);
  gsize size;
  GLenum format, type;

  if (!retro_pixel_format_to_gl (pixel_format, &format, &type, NULL))
    return;

  size = rowstride * height;

  if (!self->framebuffer)
    return;

  if (self->callback->bottom_left_origin && size != self->last_size) {
    g_clear_pointer (&self->buf_flip, g_free);
    self->buf_flip = g_malloc0 (size);
    self->last_size = size;
  }

  glBindFramebuffer (GL_FRAMEBUFFER, self->framebuffer);
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glReadnPixels (0, 0, width, height, format, type, size,
                 self->callback->bottom_left_origin ? self->buf_flip : data);
  glBindFramebuffer (GL_FRAMEBUFFER, 0);

  check_gl_errors ("snapshot");

  if (self->callback->bottom_left_origin) {
    gsize i;

    for (i = 0; i < size; i += rowstride)
      memcpy (&data[i], &self->buf_flip[size - i - rowstride], rowstride);
  }

  eglSwapBuffers (self->display, self->context);
}

static void
retro_gl_renderer_finalize (GObject *object)
{
  RetroGLRenderer *self = RETRO_GL_RENDERER (object);

  /* Ideally we need to cleanly deinit the core and call this earlier,
   * so that it actually has a chance to run */
  self->callback->context_destroy ();

  if (self->texture) {
    glDeleteTextures (1, &self->texture);
    self->texture = 0;
  }

  if (self->renderbuffer) {
    glDeleteRenderbuffers (1, &self->renderbuffer);
    self->renderbuffer = 0;
  }

  if (self->framebuffer) {
    glDeleteFramebuffers (1, &self->framebuffer);
    self->framebuffer = 0;
  }

  eglDestroyContext (self->display, self->context);
  self->context = EGL_NO_CONTEXT;

  eglTerminate (self->display);
  self->display = EGL_NO_DISPLAY;

  g_module_close (self->gl_module);
  g_clear_object (&self->gl_module);

  self->core = NULL;
  self->callback = NULL;

  g_clear_pointer (&self->buf_flip, g_free);
  self->last_size = 0;

  G_OBJECT_CLASS (retro_gl_renderer_parent_class)->finalize (object);
}

static void
retro_gl_renderer_class_init (RetroGLRendererClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_gl_renderer_finalize;
}

static void
retro_gl_renderer_init (RetroGLRenderer *self)
{
}

static void
retro_renderer_interface_init (RetroRendererInterface *iface)
{
  iface->realize = retro_gl_renderer_realize;
  iface->get_proc_address = retro_gl_renderer_get_proc_address;
  iface->get_current_framebuffer = retro_gl_renderer_get_current_framebuffer;
  iface->snapshot = retro_gl_renderer_snapshot;
}

static EGLConfig
get_egl_config (EGLDisplay  display)
{
  g_autofree EGLConfig *configs = NULL;
  EGLint count;
  EGLint attrs[] = {
    EGL_SURFACE_TYPE,      EGL_WINDOW_BIT,
    EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
    EGL_RED_SIZE,          1,
    EGL_GREEN_SIZE,        1,
    EGL_BLUE_SIZE,         1,
    EGL_ALPHA_SIZE,        0,
    EGL_DEPTH_SIZE,        0,
    EGL_RENDERABLE_TYPE,   EGL_OPENGL_BIT,
    EGL_NONE
  };

  if (!eglChooseConfig (display, attrs, NULL, 0, &count) || count < 1)
    return NULL;

  configs = g_new (EGLConfig, count);

  if (!eglChooseConfig (display, attrs, configs, count, &count) || count < 1)
    return NULL;

  return configs[0];
}

RetroRenderer *
retro_gl_renderer_new (RetroCore             *core,
                       RetroHWRenderCallback *callback)
{
  RetroGLRenderer *self = NULL;
  EGLConfig config;
  EGLint context_attribs[MAX_EGL_ATTRS];
  gboolean is_opengl_es;
  gboolean use_compat_profile;
  gint major_version, minor_version, i;

  g_return_val_if_fail (RETRO_IS_CORE (core), NULL);
  g_return_val_if_fail (callback != NULL, NULL);

  self = g_object_new (RETRO_TYPE_GL_RENDERER, NULL);
  self->core = core;
  self->callback = callback;

  is_opengl_es = FALSE;
  use_compat_profile = FALSE;
  minor_version = 0;

  switch (callback->context_type) {
  case RETRO_HW_CONTEXT_OPENGL:
    use_compat_profile = TRUE;
    major_version = 2;
    break;
  case RETRO_HW_CONTEXT_OPENGL_CORE:
    major_version = callback->version_major;
    minor_version = callback->version_minor;
    break;
  case RETRO_HW_CONTEXT_OPENGLES2:
    is_opengl_es = TRUE;
    major_version = 2;
    break;
  case RETRO_HW_CONTEXT_OPENGLES3:
    is_opengl_es = TRUE;
    major_version = 3;
    break;
  case RETRO_HW_CONTEXT_OPENGLES_VERSION:
    is_opengl_es = TRUE;
    major_version = callback->version_major;
    minor_version = callback->version_minor;
    break;
  default:
    g_assert_not_reached ();
  }

  self->display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
  check_egl_errors ("eglGetDisplay");

  eglInitialize (self->display, NULL, NULL);
  check_egl_errors ("eglInitialize");

  config = get_egl_config (self->display);
  check_egl_errors ("get_egl_config");

  if (!config)
    g_error ("Cannot find EGL config");

  eglBindAPI (is_opengl_es ? EGL_OPENGL_ES_API : EGL_OPENGL_API);
  check_egl_errors ("eglBindAPI");

  i = 0;
  context_attribs[i++] = EGL_CONTEXT_MAJOR_VERSION;
  context_attribs[i++] = major_version;
  context_attribs[i++] = EGL_CONTEXT_MINOR_VERSION;
  context_attribs[i++] = minor_version;
  context_attribs[i++] = EGL_CONTEXT_OPENGL_PROFILE_MASK;
  context_attribs[i++] = use_compat_profile ?
    EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT :
    EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;

  if (callback->debug_context) {
    context_attribs[i++] = EGL_CONTEXT_FLAGS_KHR;
    context_attribs[i++] = EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
  }

  context_attribs[i++] = EGL_NONE;

  self->context = eglCreateContext (self->display, config, EGL_NO_CONTEXT, context_attribs);
  check_egl_errors ("eglCreateContext");

  self->gl_module = g_module_open (NULL, G_MODULE_BIND_LAZY);

  return RETRO_RENDERER (self);
}
