// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-controller-state-private.h"

#include <semaphore.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include "retro-controller-type.h"
#include "retro-input-private.h"

#define RETRO_CONTROLLER_TYPE_COUNT (RETRO_CONTROLLER_TYPE_POINTER + 1)

typedef struct {
  gboolean is_dirty;
  gboolean supports_rumble;
  gint64 available_types;
  gint16 joypad_data[RETRO_JOYPAD_ID_COUNT];
  gint16 mouse_data[RETRO_MOUSE_ID_COUNT];
  gint16 keyboard_data[RETRO_KEYBOARD_KEY_LAST];
  gint16 lightgun_data[RETRO_LIGHTGUN_ID_COUNT];
  gint16 analog_data[RETRO_ANALOG_ID_COUNT * RETRO_ANALOG_INDEX_COUNT];
  gint16 pointer_data[RETRO_POINTER_ID_COUNT];
} RetroControllerStateData;

typedef struct {
  sem_t semaphore;
  RetroControllerStateData data;
} RetroControllerStateSharedData;

struct _RetroControllerState
{
  GObject parent_instance;

  gint fd;
  RetroControllerStateSharedData *shared_data;
#ifdef RETRO_RUNNER_COMPILATION
  RetroControllerStateData snapshot;
#endif
};

G_DEFINE_TYPE (RetroControllerState, retro_controller_state, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_FD,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

static gint16 *
get_data_for_type (RetroControllerStateData *data,
                   RetroControllerType       type)
{
  switch (type) {
  case RETRO_CONTROLLER_TYPE_JOYPAD:
    return data->joypad_data;

  case RETRO_CONTROLLER_TYPE_MOUSE:
    return data->mouse_data;

  case RETRO_CONTROLLER_TYPE_KEYBOARD:
    return data->keyboard_data;

  case RETRO_CONTROLLER_TYPE_LIGHTGUN:
    return data->lightgun_data;

  case RETRO_CONTROLLER_TYPE_ANALOG:
    return data->analog_data;

  case RETRO_CONTROLLER_TYPE_POINTER:
    return data->pointer_data;

  case RETRO_CONTROLLER_TYPE_NONE:
  default:
    g_assert_not_reached ();
  }
}

static void
retro_controller_state_constructed (GObject *object)
{
  RetroControllerState *self = RETRO_CONTROLLER_STATE (object);

  G_OBJECT_CLASS (retro_controller_state_parent_class)->constructed (object);

  if (ftruncate (self->fd, sizeof (RetroControllerStateSharedData)) != 0)
    g_critical ("Couldn't truncate controller data: %s", g_strerror (errno));

  self->shared_data = mmap (NULL, sizeof (RetroControllerStateSharedData),
                            PROT_READ | PROT_WRITE, MAP_SHARED, self->fd, 0);

#ifndef RETRO_RUNNER_COMPILATION
  if (sem_init (&self->shared_data->semaphore, 1, 1) != 0)
    g_critical ("Couldn't init semaphore: %s", g_strerror (errno));
#endif
}

static void
retro_controller_state_finalize (GObject *object)
{
  RetroControllerState *self = (RetroControllerState *)object;

#ifndef RETRO_RUNNER_COMPILATION
  if (sem_destroy (&self->shared_data->semaphore) != 0)
    g_critical ("Couldn't destroy semaphore: %s", g_strerror (errno));
#endif

  if (self->shared_data) {
    munmap (self->shared_data, sizeof (RetroControllerStateSharedData));
    self->shared_data = NULL;
  }

  close (self->fd);

  G_OBJECT_CLASS (retro_controller_state_parent_class)->finalize (object);
}

static void
retro_controller_state_get_property (GObject    *object,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  RetroControllerState *self = RETRO_CONTROLLER_STATE (object);

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
retro_controller_state_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  RetroControllerState *self = RETRO_CONTROLLER_STATE (object);

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
retro_controller_state_class_init (RetroControllerStateClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = retro_controller_state_constructed;
  object_class->finalize = retro_controller_state_finalize;
  object_class->get_property = retro_controller_state_get_property;
  object_class->set_property = retro_controller_state_set_property;

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
retro_controller_state_init (RetroControllerState *self)
{
}

RetroControllerState *
retro_controller_state_new (gint fd)
{
  g_return_val_if_fail (fd >= 0, NULL);

  return g_object_new (RETRO_TYPE_CONTROLLER_STATE, "fd", fd, NULL);
}

gint
retro_controller_state_get_fd (RetroControllerState *self)
{
  g_return_val_if_fail (RETRO_IS_CONTROLLER_STATE (self), 0);

  return self->fd;
}

void
retro_controller_state_lock (RetroControllerState *self)
{
  g_return_if_fail (RETRO_IS_CONTROLLER_STATE (self));

  if (sem_wait (&self->shared_data->semaphore) != 0)
    g_critical ("Couldn't lock: %s", g_strerror (errno));
}

void
retro_controller_state_unlock (RetroControllerState *self)
{
  g_return_if_fail (RETRO_IS_CONTROLLER_STATE (self));

  if (sem_post (&self->shared_data->semaphore) != 0)
    g_critical ("Couldn't unlock: %s", g_strerror (errno));
}

#ifdef RETRO_RUNNER_COMPILATION

gboolean
retro_controller_state_has_type (RetroControllerState *self,
                                 RetroControllerType   type)
{
  gint64 available_types;

  g_return_val_if_fail (RETRO_IS_CONTROLLER_STATE (self), FALSE);
  g_return_val_if_fail (type > RETRO_CONTROLLER_TYPE_NONE, FALSE);
  g_return_val_if_fail (type < RETRO_CONTROLLER_TYPE_COUNT, FALSE);

  available_types = self->snapshot.available_types;

  return (available_types & (1 << type)) > 0;
}

gint16
retro_controller_state_get_input (RetroControllerState *self,
                                  RetroInput           *input)
{
  RetroControllerType type;
  guint id, index, stride;
  gint16 *data;

  g_return_val_if_fail (RETRO_IS_CONTROLLER_STATE (self), 0);
  g_return_val_if_fail (input != NULL, 0);

  type = input->any.type;
  id = input->any.id;
  index = input->any.index;

  stride = retro_controller_type_get_id_count (type);

  if (id > stride || index > retro_controller_type_get_index_count (type))
    return 0;

  data = get_data_for_type (&self->snapshot, type);

  return data[index * stride + id];
}

gboolean
retro_controller_state_get_supports_rumble (RetroControllerState *self)
{
  g_return_val_if_fail (RETRO_IS_CONTROLLER_STATE (self), FALSE);

  return self->snapshot.supports_rumble;
}

void
retro_controller_state_snapshot (RetroControllerState *self)
{
  g_return_if_fail (RETRO_IS_CONTROLLER_STATE (self));

  if (!self->shared_data->data.is_dirty)
    return;

  self->shared_data->data.is_dirty = FALSE;

  memcpy (&self->snapshot, &self->shared_data->data, sizeof (RetroControllerStateData));
}

#else

void
retro_controller_state_set_for_type (RetroControllerState *self,
                                     RetroControllerType   type,
                                     gint16               *state,
                                     gsize                 n_items)
{
  gint16 *data;

  g_return_if_fail (RETRO_IS_CONTROLLER_STATE (self));
  g_return_if_fail (state != NULL);

  data = get_data_for_type (&self->shared_data->data, type);

  memcpy (data, state, n_items * sizeof (gint16));
  self->shared_data->data.available_types |= (1 << type);
  self->shared_data->data.is_dirty = TRUE;
}

static gsize
get_data_size_for_type (RetroControllerType type)
{
  return retro_controller_type_get_id_count (type) *
         retro_controller_type_get_index_count (type);
}

void
retro_controller_state_clear_type (RetroControllerState *self,
                                   RetroControllerType   type)
{
  gint16 *data;

  g_return_if_fail (RETRO_IS_CONTROLLER_STATE (self));

  data = get_data_for_type (&self->shared_data->data, type);
  memset (data, 0, get_data_size_for_type (type) * sizeof (gint16));

  self->shared_data->data.available_types &= ~(1 << type);
  self->shared_data->data.is_dirty = TRUE;
}

void
retro_controller_state_set_supports_rumble (RetroControllerState *self,
                                            gboolean              supports_rumble)
{
  g_return_if_fail (RETRO_IS_CONTROLLER_STATE (self));

  self->shared_data->data.supports_rumble = supports_rumble;
}

#endif
