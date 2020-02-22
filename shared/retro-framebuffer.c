// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-framebuffer-private.h"

#include <semaphore.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct {
  sem_t semaphore;
  gboolean is_dirty;
  RetroPixelFormat format;
  gsize rowstride;
  guint width;
  guint height;
  gfloat aspect_ratio;
} RetroFramebufferMetadata;

struct _RetroFramebuffer
{
  GObject parent_instance;

  gint fd;
  gsize size;
  gpointer shared_data;
  RetroFramebufferMetadata *metadata;
  gpointer framebuffer;
};

G_DEFINE_TYPE (RetroFramebuffer, retro_framebuffer, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_FD,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

static void
resize (RetroFramebuffer *self,
        gsize             size)
{
  size += sizeof (RetroFramebufferMetadata);

  if (G_LIKELY (size == self->size))
    return;

  if (self->shared_data) {
    munmap (self->shared_data, self->size);
    self->shared_data = NULL;
  }

  self->size = size;

  if (ftruncate (self->fd, size) != 0)
    g_critical ("Couldn't truncate framebuffer: %s", g_strerror (errno));

  self->shared_data = mmap (NULL, size,
                            PROT_READ | PROT_WRITE, MAP_SHARED, self->fd, 0);

  self->metadata = (RetroFramebufferMetadata *) self->shared_data;
  self->framebuffer = (self->shared_data + sizeof (RetroFramebufferMetadata));
}

static void
retro_framebuffer_constructed (GObject *object)
{
  RetroFramebuffer *self = RETRO_FRAMEBUFFER (object);

  G_OBJECT_CLASS (retro_framebuffer_parent_class)->constructed (object);

  resize (self, 0);

#ifdef RETRO_RUNNER_COMPILATION
  if (sem_init (&self->metadata->semaphore, 1, 1) != 0)
    g_critical ("Couldn't init semaphore: %s", g_strerror (errno));
#endif
}

static void
retro_framebuffer_finalize (GObject *object)
{
  RetroFramebuffer *self = (RetroFramebuffer *)object;

#ifdef RETRO_RUNNER_COMPILATION
  if (sem_destroy (&self->metadata->semaphore) != 0)
    g_critical ("Couldn't destroy semaphore: %s", g_strerror (errno));
#endif

  if (self->shared_data) {
    munmap (self->shared_data, self->size);
    self->shared_data = NULL;
  }

  close (self->fd);

  G_OBJECT_CLASS (retro_framebuffer_parent_class)->finalize (object);
}

static void
retro_framebuffer_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  RetroFramebuffer *self = RETRO_FRAMEBUFFER (object);

  switch (prop_id) {
  case PROP_FD:
    g_value_set_int (value, self->fd);

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_framebuffer_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  RetroFramebuffer *self = RETRO_FRAMEBUFFER (object);

  switch (prop_id) {
  case PROP_FD:
    self->fd = g_value_get_int (value);

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_framebuffer_class_init (RetroFramebufferClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = retro_framebuffer_constructed;
  object_class->finalize = retro_framebuffer_finalize;
  object_class->get_property = retro_framebuffer_get_property;
  object_class->set_property = retro_framebuffer_set_property;

  properties[PROP_FD] =
    g_param_spec_int ("fd",
                      "File descriptor",
                      "The file descriptor backing shared memory.",
                      -1,
                      G_MAXINT,
                      -1,
                      G_PARAM_READWRITE |
                      G_PARAM_CONSTRUCT_ONLY |
                      G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (G_OBJECT_CLASS (klass), N_PROPS, properties);
}

static void
retro_framebuffer_init (RetroFramebuffer *self)
{
}

RetroFramebuffer *
retro_framebuffer_new (gint fd)
{
  g_return_val_if_fail (fd >= 0, NULL);

  return g_object_new (RETRO_TYPE_FRAMEBUFFER, "fd", fd, NULL);
}

gint
retro_framebuffer_get_fd (RetroFramebuffer *self)
{
  g_return_val_if_fail (RETRO_IS_FRAMEBUFFER (self), 0);

  return self->fd;
}

void
retro_framebuffer_lock (RetroFramebuffer *self)
{
  g_return_if_fail (RETRO_IS_FRAMEBUFFER (self));

  if (sem_wait (&self->metadata->semaphore) != 0)
    g_critical ("Couldn't lock: %s", g_strerror (errno));
}

void
retro_framebuffer_unlock (RetroFramebuffer *self)
{
  g_return_if_fail (RETRO_IS_FRAMEBUFFER (self));

  if (sem_post (&self->metadata->semaphore) != 0)
    g_critical ("Couldn't unlock: %s", g_strerror (errno));
}

#ifdef RETRO_RUNNER_COMPILATION

void
retro_framebuffer_set_data (RetroFramebuffer *self,
                            RetroPixelFormat  format,
                            gsize             rowstride,
                            guint             width,
                            guint             height,
                            gfloat            aspect_ratio,
                            gpointer          data)
{
  gsize size;

  g_return_if_fail (RETRO_IS_FRAMEBUFFER (self));

  size = height * rowstride;

  resize (self, size);

  self->metadata->is_dirty = TRUE;
  self->metadata->format = format;
  self->metadata->rowstride = rowstride;
  self->metadata->width = width;
  self->metadata->height = height;
  self->metadata->aspect_ratio = aspect_ratio;

  if (size && data)
    memcpy (self->framebuffer, data, size);
}

#else

gboolean
retro_framebuffer_get_is_dirty (RetroFramebuffer *self)
{
  g_return_val_if_fail (RETRO_IS_FRAMEBUFFER (self), FALSE);

  return self->metadata->is_dirty;
}

RetroPixelFormat
retro_framebuffer_get_format (RetroFramebuffer *self)
{
  g_return_val_if_fail (RETRO_IS_FRAMEBUFFER (self), 0);

  return self->metadata->format;
}

gsize
retro_framebuffer_get_rowstride (RetroFramebuffer *self)
{
  g_return_val_if_fail (RETRO_IS_FRAMEBUFFER (self), 0);

  return self->metadata->rowstride;
}

guint
retro_framebuffer_get_width (RetroFramebuffer *self)
{
  g_return_val_if_fail (RETRO_IS_FRAMEBUFFER (self), 0);

  return self->metadata->width;
}

guint
retro_framebuffer_get_height (RetroFramebuffer *self)
{
  g_return_val_if_fail (RETRO_IS_FRAMEBUFFER (self), 0);

  return self->metadata->height;
}

gdouble
retro_framebuffer_get_aspect_ratio (RetroFramebuffer *self)
{
  g_return_val_if_fail (RETRO_IS_FRAMEBUFFER (self), 0.0);

  return self->metadata->aspect_ratio;
}

gpointer
retro_framebuffer_get_pixels (RetroFramebuffer *self)
{
  gsize size;

  g_return_val_if_fail (RETRO_IS_FRAMEBUFFER (self), NULL);

  size = self->metadata->height * self->metadata->rowstride;
  resize (self, size);

  self->metadata->is_dirty = FALSE;

  return self->framebuffer;
}

#endif
