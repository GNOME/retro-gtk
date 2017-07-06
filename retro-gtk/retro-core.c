// This file is part of Retro. License: GPLv3

#include "retro-core.h"

#include "retro-gtk-internal.h"

/* Private */

static void
init_controller_device (guint             port,
                        RetroInputDevice *device,
                        gpointer          data)
{
  RetroCore *self;
  RetroDeviceType device_type;

  self = RETRO_CORE (data);

  g_return_if_fail (self != NULL);
  g_return_if_fail (device != NULL);

  device_type = retro_input_device_get_device_type (device);
  retro_core_set_controller_port_device (self, port, device_type);
}

// FIXME Make static as soon as possible.
void
retro_core_init_input (RetroCore* self)
{
  RetroInput* input_interface;

  g_return_if_fail (self != NULL);

  input_interface = retro_core_get_input_interface (self);
  if (input_interface == NULL)
    return;

  retro_input_foreach_controller (input_interface, init_controller_device, self);
}

// FIXME Make static as soon as possible.
void
retro_core_on_input_controller_connected (RetroCore        *self,
                                          guint             port,
                                          RetroInputDevice *device)
{
  RetroDeviceType device_type;

  g_return_if_fail (self != NULL);
  g_return_if_fail (device != NULL);

  if (!retro_core_get_is_initiated (self))
    return;

  device_type = retro_input_device_get_device_type (device);
  retro_core_set_controller_port_device (self, port, device_type);
}

// FIXME Make static as soon as possible.
void
retro_core_on_input_controller_disconnected (RetroCore *self,
                                             guint      port)
{
  g_return_if_fail (self != NULL);

  if (!retro_core_get_is_initiated (self))
    return;

  retro_core_set_controller_port_device (self, port, RETRO_DEVICE_TYPE_NONE);
}

static void
retro_core_send_input_key_event (RetroCore                *self,
                                 gboolean                  down,
                                 RetroKeyboardKey          keycode,
                                 guint32                   character,
                                 RetroKeyboardModifierKey  key_modifiers)
{
  RetroCoreEnvironmentInternal *environment_internal;

  g_return_if_fail (self != NULL);

  environment_internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  if (environment_internal->keyboard_callback.callback == NULL)
    return;

  environment_internal->keyboard_callback.callback (down, keycode, character, key_modifiers);
}

// FIXME Make static as soon as possible.
void
retro_core_on_input_key_event (RetroCore                *self,
                               gboolean                  down,
                               RetroKeyboardKey          keycode,
                               guint32                   character,
                               RetroKeyboardModifierKey  key_modifiers)
{
  g_return_if_fail (self != NULL);

  if (!retro_core_get_is_initiated (self))
    return;

  retro_core_push_cb_data (self);
  retro_core_send_input_key_event (self, down, keycode, character, key_modifiers);
  retro_core_pop_cb_data ();
}

/* Public */

void
retro_core_set_controller_port_device (RetroCore       *self,
                                       guint            port,
                                       RetroDeviceType  device)
{
  RetroSetControllerPortDevice set_controller_port_device;

  g_return_if_fail (self != NULL);

  retro_core_push_cb_data (self);
  set_controller_port_device = retro_module_get_set_controller_port_device (self->module);
  set_controller_port_device (port, device);
  retro_core_pop_cb_data ();
}

gboolean
retro_core_supports_serialization (RetroCore *self)
{
  RetroSerializeSize serialize_size = NULL;
  gsize size;

  g_return_val_if_fail (self != NULL, FALSE);

  retro_core_push_cb_data (self);
  serialize_size = retro_module_get_serialize_size (self->module);
  size = serialize_size ();
  retro_core_pop_cb_data ();

  return size > 0;
}

guint8 *
retro_core_serialize_state (RetroCore  *self,
                            gsize      *length,
                            GError    **error)
{
  RetroSerializeSize serialize_size = NULL;
  RetroSerialize serialize = NULL;
  guint8 *data;
  gsize size;
  gboolean success;

  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (length != NULL, NULL);

  serialize_size = retro_module_get_serialize_size (self->module);

  retro_core_push_cb_data (self);
  size = serialize_size ();
  retro_core_pop_cb_data ();

  if (size <= 0) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_SERIALIZATION_NOT_SUPPORTED,
                 "Couldn't serialize the internal state: serialization not supported.");

    return NULL;
  }

  serialize = retro_module_get_serialize (self->module);
  data = g_new0 (guint8, size);

  retro_core_push_cb_data (self);
  success = serialize (data, size);
  retro_core_pop_cb_data ();

  if (!success) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_COULDNT_SERIALIZE,
                 "Couldn't serialize the internal state: serialization failed.");
    g_free (data);

    return NULL;
  }

  *length = size;

  return data;
}

void
retro_core_deserialize_state (RetroCore  *self,
                              guint8     *data,
                              gsize       length,
                              GError    **error)
{
  RetroSerializeSize serialize_size = NULL;
  RetroUnserialize unserialize = NULL;
  gsize size;
  gboolean success;

  g_return_if_fail (self != NULL);
  g_return_if_fail (data != NULL);

  serialize_size = retro_module_get_serialize_size (self->module);

  retro_core_push_cb_data (self);
  size = serialize_size ();
  retro_core_pop_cb_data ();

  if (size <= 0) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_SERIALIZATION_NOT_SUPPORTED,
                 "Couldn't deserialize the internal state: serialization not supported.");

    return;
  }

  if (length > size) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_COULDNT_DESERIALIZE,
                 "Couldn't deserialize the internal state: expected at most %"G_GSIZE_FORMAT" bytes, got %"G_GSIZE_FORMAT".", size, length);

    return;
  }

  unserialize = retro_module_get_unserialize (self->module);

  retro_core_push_cb_data (self);
  success = unserialize (data, length);
  retro_core_pop_cb_data ();

  if (!success) {
    g_set_error (error,
                 RETRO_CORE_ERROR,
                 RETRO_CORE_ERROR_COULDNT_DESERIALIZE,
                 "Couldn't deserialize the internal state: deserialization failed.");
  }
}

gboolean
retro_core_prepare (RetroCore* self) {
  RetroLoadGame load_game;
  RetroGetSystemAvInfo get_system_av_info;
  gboolean game_loaded;
  RetroSystemAvInfo info = {{ 0 }};

  g_return_val_if_fail (self != NULL, FALSE);

  retro_core_push_cb_data (self);
  load_game = retro_module_get_load_game (self->module);
  game_loaded = load_game (NULL);
  retro_core_set_game_loaded (self, game_loaded);
  get_system_av_info = retro_module_get_get_system_av_info (self->module);
  get_system_av_info (&info);
  retro_core_set_system_av_info (self, &info);
  retro_core_pop_cb_data ();

  return game_loaded;
}

gsize
retro_core_get_memory_size (RetroCore       *self,
                            RetroMemoryType  id)
{
  gsize size;
  RetroGetMemorySize get_memory_size;

  g_return_val_if_fail (self != NULL, 0UL);

  retro_core_push_cb_data (self);
  get_memory_size = retro_module_get_get_memory_size (self->module);
  size = get_memory_size (id);
  retro_core_pop_cb_data ();

  return size;
}

guint8 *
retro_core_get_memory (RetroCore       *self,
                       RetroMemoryType  id,
                       gint            *length)
{
  RetroGetMemoryData get_mem_data;
  RetroGetMemorySize get_mem_size;
  guint8 *data;
  gsize size;

  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (length != NULL, NULL);

  get_mem_data = retro_module_get_get_memory_data (self->module);
  get_mem_size = retro_module_get_get_memory_size (self->module);

  retro_core_push_cb_data (self);
  data = get_mem_data (id);
  size = get_mem_size (id);
  retro_core_pop_cb_data ();

  data = g_memdup (data, size);
  *length = (gint) (data != NULL ? size : 0);

  return data;
}

void
retro_core_set_memory (RetroCore       *self,
                       RetroMemoryType  id,
                       guint8          *data,
                       gint             length)
{
  RetroGetMemoryData get_mem_region;
  RetroGetMemorySize get_mem_region_size;
  guint8 *memory_region;
  gsize memory_region_size;

  g_return_if_fail (self != NULL);
  g_return_if_fail (data != NULL);
  g_return_if_fail (length > 0);

  get_mem_region = retro_module_get_get_memory_data (self->module);
  get_mem_region_size = retro_module_get_get_memory_size (self->module);

  retro_core_push_cb_data (self);
  memory_region = get_mem_region (id);
  memory_region_size = get_mem_region_size (id);
  retro_core_pop_cb_data ();

  g_return_if_fail (memory_region != NULL);
  g_return_if_fail (memory_region_size == length);

  memcpy (memory_region, data, length);
}

void
retro_core_environment_internal_setup (RetroCore *self)
{
  self->environment_internal = g_new0 (RetroCoreEnvironmentInternal, 1);
}

void
retro_core_environment_internal_release (RetroCore *self)
{
  g_free (self->environment_internal);
}
