// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-core.h"

#include "retro-gtk-internal.h"
#include "input/retro-keyboard-key.h"

/* Private */

#define RETRO_CORE_OBJECTS_LENGTH 32

static GRecMutex retro_core_r_mutex = { 0 };
static GRecMutex retro_core_w_mutex = { 0 };
static RetroCore *retro_core_objects[32];
static gint retro_core_i = 0;

// FIXME Make static as soon as possible.
void
retro_core_push_cb_data (RetroCore *self)
{
  g_return_if_fail (self != NULL);

  g_rec_mutex_lock (&retro_core_w_mutex);
  g_rec_mutex_lock (&retro_core_r_mutex);

  if (G_UNLIKELY (retro_core_i == RETRO_CORE_OBJECTS_LENGTH)) {
    g_critical ("RetroCore callback data stack overflow.");

    g_rec_mutex_unlock (&retro_core_r_mutex);
    g_assert_not_reached ();
  }

  retro_core_objects[retro_core_i] = self;
  retro_core_i++;

  g_rec_mutex_unlock (&retro_core_r_mutex);
}

// FIXME Make static as soon as possible.
void
retro_core_pop_cb_data (void)
{
  g_rec_mutex_lock (&retro_core_r_mutex);

  if (G_UNLIKELY (retro_core_i == 0)) {
    g_critical ("RetroCore callback data stack underflow.");

    g_rec_mutex_unlock (&retro_core_r_mutex);
    g_rec_mutex_unlock (&retro_core_w_mutex);
    g_assert_not_reached ();
  }
  retro_core_i--;

  retro_core_objects[retro_core_i] = NULL;

  g_rec_mutex_unlock (&retro_core_r_mutex);
  g_rec_mutex_unlock (&retro_core_w_mutex);
}

// FIXME Make static as soon as possible.
RetroCore *
retro_core_get_cb_data (void)
{
  RetroCore *result;

  g_rec_mutex_lock (&retro_core_r_mutex);

  if (retro_core_i == 0) {
    g_critical ("RetroCore callback data segmentation fault.");

    g_rec_mutex_unlock (&retro_core_r_mutex);
    g_assert_not_reached ();
  }

  result = retro_core_objects[retro_core_i - 1];
  g_rec_mutex_unlock (&retro_core_r_mutex);

  return result;
}

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
gboolean
retro_core_on_key_event (RetroCore   *self,
                         GdkEventKey *event)
{
  gboolean pressed;
  RetroKeyboardKey retro_key;
  RetroKeyboardModifierKey retro_modifier_key;
  guint32 character;

  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (!retro_core_get_is_initiated (self))
    return FALSE;

  pressed = event->type == GDK_KEY_PRESS;
  retro_key = retro_keyboard_key_converter (event->keyval);
  retro_modifier_key = retro_keyboard_modifier_key_converter (event->keyval, event->state);
  character = gdk_keyval_to_unicode (event->keyval);

  retro_core_push_cb_data (self);
  retro_core_send_input_key_event (self,
                                   pressed,
                                   retro_key,
                                   character,
                                   retro_modifier_key);
  retro_core_pop_cb_data ();

  return FALSE;
}

static void
retro_core_load_discs (RetroCore  *self,
                       GError    **error)
{
  RetroDiskControl *disk_control;
  RetroCoreEnvironmentInternal *internal;
  guint length;
  gboolean fullpath;
  GFile *file;
  gchar *path;
  RetroSystemInfo system_info = { 0 };
  guint index;
  RetroGameInfo game_info = { 0 };
  GError *tmp_error = NULL;

  g_return_if_fail (self != NULL);

  disk_control = retro_core_get_disk_control_interface (self);

  retro_disk_control_set_eject_state (disk_control, TRUE, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return;
  }

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);
  length = g_strv_length (internal->media_uris);
  while (retro_disk_control_get_num_images (disk_control, &tmp_error) < length &&
         (tmp_error != NULL)) {
    retro_disk_control_add_image_index (disk_control, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);

      return;
    }
  }

  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return;
  }

  retro_core_get_system_info (self, &system_info);
  fullpath = system_info.need_fullpath;
  for (index = 0; index < length; index++) {
    file = g_file_new_for_uri (internal->media_uris[index]);
    path = g_file_get_path (file);

    if (fullpath) {
      retro_game_info_destroy (&game_info);
      retro_game_info_init (&game_info, path);
    }
    else {
      retro_game_info_destroy (&game_info);
      retro_game_info_init_with_data (&game_info, path, &tmp_error);
      if (G_UNLIKELY (tmp_error != NULL)) {
        g_propagate_error (error, tmp_error);

        retro_game_info_destroy (&game_info);
        g_free (path);
        g_object_unref (file);
        return;
      }
    }

    retro_disk_control_replace_image_index (disk_control, index, &game_info, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);

      retro_game_info_destroy (&game_info);
      g_free (path);
      g_object_unref (file);

      return;
    }

    retro_game_info_destroy (&game_info);
    g_free (path);
    g_object_unref (file);
  }

  retro_disk_control_set_eject_state (disk_control, FALSE, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return;
  }
}

static gboolean
retro_core_load_game (RetroCore     *self,
                      RetroGameInfo *game)
{
  RetroCoreEnvironmentInternal *internal;
  RetroUnloadGame unload_game;
  RetroLoadGame load_game;
  RetroGetSystemAvInfo get_system_av_info;
  gboolean game_loaded;
  RetroSystemAvInfo info = {{ 0 }};

  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (game != NULL, FALSE);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  if (retro_core_get_game_loaded (self)) {
    retro_core_push_cb_data (self);
    unload_game = retro_module_get_unload_game (internal->module);
    unload_game ();
    retro_core_pop_cb_data ();
  }

  retro_core_push_cb_data (self);
  load_game = retro_module_get_load_game (internal->module);
  game_loaded = load_game (game);
  retro_core_set_game_loaded (self, game_loaded);
  get_system_av_info = retro_module_get_get_system_av_info (internal->module);
  get_system_av_info (&info);
  retro_core_set_system_av_info (self, &info);
  retro_core_pop_cb_data ();

  return game_loaded;
}

static gboolean
retro_core_prepare (RetroCore* self) {
  RetroCoreEnvironmentInternal *internal;
  RetroLoadGame load_game;
  RetroGetSystemAvInfo get_system_av_info;
  gboolean game_loaded;
  RetroSystemAvInfo info = {{ 0 }};

  g_return_val_if_fail (self != NULL, FALSE);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  retro_core_push_cb_data (self);
  load_game = retro_module_get_load_game (internal->module);
  game_loaded = load_game (NULL);
  retro_core_set_game_loaded (self, game_loaded);
  get_system_av_info = retro_module_get_get_system_av_info (internal->module);
  get_system_av_info (&info);
  retro_core_set_system_av_info (self, &info);
  retro_core_pop_cb_data ();

  return game_loaded;
}

static void
retro_core_load_medias (RetroCore* self,
                        GError** error)
{
  RetroCoreEnvironmentInternal *internal;
  guint length;
  gchar *uri;
  GFile *file;
  gchar *path;
  gboolean fullpath;
  RetroSystemInfo system_info = { 0 };
  RetroGameInfo game_info = { 0 };
  GError *tmp_error = NULL;

  g_return_if_fail (self != NULL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);
  length = g_strv_length (internal->media_uris);

  if (length == 0) {
    retro_core_prepare (self);

    return;
  }

  uri = g_strdup (internal->media_uris[0]);
  file = g_file_new_for_uri (uri);
  path = g_file_get_path (file);
  retro_core_get_system_info (self, &system_info);
  fullpath = system_info.need_fullpath;
  if (fullpath) {
    retro_game_info_destroy (&game_info);
    retro_game_info_init (&game_info, path);
  }
  else {
    retro_game_info_destroy (&game_info);
    retro_game_info_init_with_data (&game_info, path, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);
      retro_game_info_destroy (&game_info);
      g_free (path);
      g_object_unref (file);
      g_free (uri);

      return;
    }
  }
  if (!retro_core_load_game (self, &game_info)) {
    retro_game_info_destroy (&game_info);
    g_free (path);
    g_object_unref (file);
    g_free (uri);

    return;
  }
  if (retro_core_get_disk_control_interface (self) != NULL) {
    retro_core_load_discs (self, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);
      retro_game_info_destroy (&game_info);
      g_free (path);
      g_object_unref (file);
      g_free (uri);

      return;
    }
  }
  retro_game_info_destroy (&game_info);
  g_free (path);
  g_object_unref (file);
  g_free (uri);
}

// FIXME Make static as soon as possible.
guint
retro_core_get_api_version_real (RetroCore *self)
{
  RetroCoreEnvironmentInternal *internal;
  guint result;
  RetroApiVersion api_version;

  g_return_val_if_fail (self != NULL, 0U);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  retro_core_push_cb_data (self);
  api_version = retro_module_get_api_version (internal->module);
  result = api_version ();
  retro_core_pop_cb_data ();

  return result;
}

// FIXME Make static as soon as possible.
void
retro_core_get_system_info_real (RetroCore       *self,
                                 RetroSystemInfo *system_info)
{
  RetroCoreEnvironmentInternal *internal;
  RetroGetSystemInfo get_system_info;

  g_return_if_fail (self != NULL);
  g_return_if_fail (system_info != NULL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  retro_core_push_cb_data (self);
  get_system_info = retro_module_get_get_system_info (internal->module);
  get_system_info (system_info);
  retro_core_pop_cb_data ();
}

void retro_core_set_environment_interface (RetroCore *self);
void retro_core_set_callbacks (RetroCore *self);

// FIXME Make static as soon as possible.
void
retro_core_constructor (RetroCore   *self,
                        const gchar *file_name)
{
  RetroCoreEnvironmentInternal *internal;
  GFile *file;
  GFile *relative_path_file;
  gchar *libretro_path;

  g_return_if_fail (file_name != NULL);

  retro_core_set_file_name (self, file_name);

  internal = g_new0 (RetroCoreEnvironmentInternal, 1);
  self->environment_internal = internal;

  file = g_file_new_for_path (file_name);
  relative_path_file = g_file_resolve_relative_path (file, "");

  g_object_unref (file);

  libretro_path = g_file_get_path (relative_path_file);

  g_object_unref (relative_path_file);

  retro_core_set_libretro_path (self, libretro_path);
  internal->module = retro_module_new (libretro_path);

  g_free (libretro_path);

  retro_core_set_callbacks (self);
  self->variables_interface = RETRO_VARIABLES (retro_options_new ());
}

// FIXME Make static as soon as possible.
void
retro_core_destructor (RetroCore *self)
{
  RetroCoreEnvironmentInternal *internal;
  RetroUnloadGame unload_game;
  RetroDeinit deinit;

  g_return_if_fail (self != NULL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  retro_core_push_cb_data (self);
  if (retro_core_get_game_loaded (self)) {
    unload_game = retro_module_get_unload_game (internal->module);
    unload_game ();
  }
  deinit = retro_module_get_deinit (internal->module);
  deinit ();
  retro_core_pop_cb_data ();

  if (internal->media_uris != NULL)
    g_strfreev (internal->media_uris);

  g_free (self->environment_internal);
}

/* Public */

void
retro_core_init (RetroCore  *self,
                 GError    **error)
{
  RetroCoreEnvironmentInternal *internal;
  RetroInit init;
  GError *tmp_error = NULL;

  g_return_if_fail (self != NULL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  retro_core_set_environment_interface (self);

  retro_core_push_cb_data (self);
  init = retro_module_get_init (internal->module);
  init ();
  retro_core_pop_cb_data ();

  retro_core_init_input (self);

  retro_core_set_is_initiated (self, TRUE);

  retro_core_load_medias (self, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return;
  }
}

void
retro_core_set_medias (RetroCore  *self,
                       gchar     **uris)
{
  RetroCoreEnvironmentInternal *internal;

  g_return_if_fail (self != NULL);
  g_return_if_fail (!retro_core_get_is_initiated (self));

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  if (internal->media_uris != NULL)
    g_strfreev (internal->media_uris);

  internal->media_uris = g_strdupv (uris);
}

void
retro_core_set_current_media (RetroCore  *self,
                              guint       media_index,
                              GError    **error)
{
  RetroCoreEnvironmentInternal *internal;
  RetroDiskControl *disk_control;
  guint length;
  GError *tmp_error = NULL;

  g_return_if_fail (self != NULL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);
  length = g_strv_length (internal->media_uris);

  g_return_if_fail (media_index < length);

  disk_control = retro_core_get_disk_control_interface (self);

  if (disk_control == NULL)
    return;

  retro_disk_control_set_eject_state (disk_control, TRUE, &tmp_error);
  if (tmp_error != NULL) {
    g_propagate_error (error, tmp_error);

    return;
  }

  retro_disk_control_set_image_index (disk_control, media_index, &tmp_error);
  if (tmp_error != NULL) {
    g_propagate_error (error, tmp_error);

    return;
  }

  retro_disk_control_set_eject_state (disk_control, FALSE, &tmp_error);
  if (tmp_error != NULL) {
    g_propagate_error (error, tmp_error);

    return;
  }
}

void
retro_core_set_controller_port_device (RetroCore       *self,
                                       guint            port,
                                       RetroDeviceType  device)
{
  RetroCoreEnvironmentInternal *internal;
  RetroSetControllerPortDevice set_controller_port_device;

  g_return_if_fail (self != NULL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  retro_core_push_cb_data (self);
  set_controller_port_device = retro_module_get_set_controller_port_device (internal->module);
  set_controller_port_device (port, device);
  retro_core_pop_cb_data ();
}

void
retro_core_reset (RetroCore* self)
{
  RetroCoreEnvironmentInternal *internal;
  RetroReset reset;

  g_return_if_fail (self != NULL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  retro_core_push_cb_data (self);
  reset = retro_module_get_reset (internal->module);
  reset ();
  retro_core_pop_cb_data ();
}

void
retro_core_run (RetroCore* self)
{
  RetroCoreEnvironmentInternal *internal;
  RetroRun run;

  g_return_if_fail (self != NULL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  retro_core_push_cb_data (self);
  run = retro_module_get_run (internal->module);
  run ();
  retro_core_pop_cb_data ();
}

gboolean
retro_core_supports_serialization (RetroCore *self)
{
  RetroCoreEnvironmentInternal *internal;
  RetroSerializeSize serialize_size = NULL;
  gsize size;

  g_return_val_if_fail (self != NULL, FALSE);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  retro_core_push_cb_data (self);
  serialize_size = retro_module_get_serialize_size (internal->module);
  size = serialize_size ();
  retro_core_pop_cb_data ();

  return size > 0;
}

guint8 *
retro_core_serialize_state (RetroCore  *self,
                            gsize      *length,
                            GError    **error)
{
  RetroCoreEnvironmentInternal *internal;
  RetroSerializeSize serialize_size = NULL;
  RetroSerialize serialize = NULL;
  guint8 *data;
  gsize size;
  gboolean success;

  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (length != NULL, NULL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  serialize_size = retro_module_get_serialize_size (internal->module);

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

  serialize = retro_module_get_serialize (internal->module);
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
  RetroCoreEnvironmentInternal *internal;
  RetroSerializeSize serialize_size = NULL;
  RetroUnserialize unserialize = NULL;
  gsize size;
  gboolean success;

  g_return_if_fail (self != NULL);
  g_return_if_fail (data != NULL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  serialize_size = retro_module_get_serialize_size (internal->module);

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

  unserialize = retro_module_get_unserialize (internal->module);

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

gsize
retro_core_get_memory_size (RetroCore       *self,
                            RetroMemoryType  id)
{
  RetroCoreEnvironmentInternal *internal;
  gsize size;
  RetroGetMemorySize get_memory_size;

  g_return_val_if_fail (self != NULL, 0UL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  retro_core_push_cb_data (self);
  get_memory_size = retro_module_get_get_memory_size (internal->module);
  size = get_memory_size (id);
  retro_core_pop_cb_data ();

  return size;
}

guint8 *
retro_core_get_memory (RetroCore       *self,
                       RetroMemoryType  id,
                       gint            *length)
{
  RetroCoreEnvironmentInternal *internal;
  RetroGetMemoryData get_mem_data;
  RetroGetMemorySize get_mem_size;
  guint8 *data;
  gsize size;

  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (length != NULL, NULL);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  get_mem_data = retro_module_get_get_memory_data (internal->module);
  get_mem_size = retro_module_get_get_memory_size (internal->module);

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
  RetroCoreEnvironmentInternal *internal;
  RetroGetMemoryData get_mem_region;
  RetroGetMemorySize get_mem_region_size;
  guint8 *memory_region;
  gsize memory_region_size;

  g_return_if_fail (self != NULL);
  g_return_if_fail (data != NULL);
  g_return_if_fail (length > 0);

  internal = RETRO_CORE_ENVIRONMENT_INTERNAL (self);

  get_mem_region = retro_module_get_get_memory_data (internal->module);
  get_mem_region_size = retro_module_get_get_memory_size (internal->module);

  retro_core_push_cb_data (self);
  memory_region = get_mem_region (id);
  memory_region_size = get_mem_region_size (id);
  retro_core_pop_cb_data ();

  g_return_if_fail (memory_region != NULL);
  g_return_if_fail (memory_region_size == length);

  memcpy (memory_region, data, length);
}
