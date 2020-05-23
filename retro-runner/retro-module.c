// This file is part of retro-gtk. License: GPL-3.0+.

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <gio/gio.h>
#include <gmodule.h>
#include <stdio.h>
#include "retro-module-private.h"

static GHashTable *retro_module_loaded_modules;

struct _RetroModule
{
  GObject parent_instance;
  gchar *file_name;
  gboolean is_a_copy;
  GFile *tmp_file;
  GModule *module;
  RetroCallbackSetter set_environment;
  RetroCallbackSetter set_video_refresh;
  RetroCallbackSetter set_audio_sample;
  RetroCallbackSetter set_audio_sample_batch;
  RetroCallbackSetter set_input_poll;
  RetroCallbackSetter set_input_state;
  RetroInit init;
  RetroDeinit deinit;
  RetroApiVersion api_version;
  RetroGetSystemInfo get_system_info;
  RetroGetSystemAvInfo get_system_av_info;
  RetroSetControllerPortDevice set_controller_port_device;
  RetroReset reset;
  RetroRun run;
  RetroSerializeSize serialize_size;
  RetroSerialize serialize;
  RetroUnserialize unserialize;
  RetroLoadGame load_game;
  RetroUnloadGame unload_game;
  RetroGetMemoryData get_memory_data;
  RetroGetMemorySize get_memory_size;
};

G_DEFINE_TYPE (RetroModule, retro_module, G_TYPE_OBJECT)

#define fetch_function(self, name) \
  g_module_symbol (self->module, "retro_"#name, (gpointer) &self->name)

#define define_function_getter(Type, name) \
  Type \
  retro_module_get_##name (RetroModule *self) \
  { \
    g_return_val_if_fail (self != NULL, NULL); \
    return self->name; \
  } \

/* Private */

static gchar *
get_absolute_path (const gchar *file_name)
{
  g_autoptr (GFile) file = g_file_new_for_path (file_name);
  g_autoptr (GFile) absolute_path_file = g_file_resolve_relative_path (file, "");

  return g_file_get_path (absolute_path_file);
}

static void
try_delete_and_unref_file (GFile *file)
{
  g_autoptr (GError) inner_error = NULL;

  g_file_delete (file, NULL, &inner_error);
  if (G_UNLIKELY (inner_error != NULL))
    g_debug ("%s", inner_error->message);

  g_object_unref (file);
}

static void
load_module (RetroModule *self,
             const gchar *file_name)
{
  self->module = g_module_open (file_name, G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
}

static void
retro_module_finalize (GObject *object)
{
  RetroModule *self = RETRO_MODULE (object);

  g_clear_pointer (&self->tmp_file, try_delete_and_unref_file);

  if (!self->is_a_copy)
    g_hash_table_remove (retro_module_loaded_modules, self->file_name);

  g_free (self->file_name);
  g_module_close (self->module);

  G_OBJECT_CLASS (retro_module_parent_class)->finalize (object);
}

static void
retro_module_class_init (RetroModuleClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_module_finalize;

  retro_module_loaded_modules = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_free);
}

static void
retro_module_init (RetroModule *self)
{
}

const gchar *
retro_module_get_file_name (RetroModule *self)
{
  g_return_val_if_fail (self != NULL, NULL);

  return self->file_name;
}

/* Public */

RetroModule *
retro_module_new (const gchar *file_name)
{
  RetroModule *self = NULL;
  GError *inner_error = NULL;

  g_return_val_if_fail (file_name != NULL, NULL);

  self = (RetroModule*) g_object_new (RETRO_TYPE_MODULE, NULL);

  self->file_name = get_absolute_path (file_name);
  self->is_a_copy = FALSE;

  if (g_hash_table_contains (retro_module_loaded_modules, self->file_name)) {
    g_autoptr (GFile) file = g_file_new_for_path (self->file_name);
    g_autoptr (GFileIOStream) ios = NULL;

    self->tmp_file = g_file_new_tmp (NULL, &ios, &inner_error);
    if (G_UNLIKELY (inner_error != NULL)) {
      g_debug ("%s", inner_error->message);

      g_clear_pointer (&self->tmp_file, try_delete_and_unref_file);

      self->is_a_copy = FALSE;
      load_module (self, self->file_name);
      g_clear_error (&inner_error);
    }
    g_file_copy (file, self->tmp_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &inner_error);
    if (G_UNLIKELY (inner_error != NULL)) {
      g_debug ("%s", inner_error->message);

      g_clear_pointer (&self->tmp_file, try_delete_and_unref_file);

      self->is_a_copy = FALSE;
      load_module (self, self->file_name);
      g_clear_error (&inner_error);
    }
    self->is_a_copy = TRUE;
    load_module (self, self->file_name);
  }
  else {
    g_hash_table_add (retro_module_loaded_modules, g_strdup (self->file_name));
    load_module (self, self->file_name);
  }

  fetch_function (self, set_environment);
  fetch_function (self, set_video_refresh);
  fetch_function (self, set_audio_sample);
  fetch_function (self, set_audio_sample_batch);
  fetch_function (self, set_input_poll);
  fetch_function (self, set_input_state);
  fetch_function (self, init);
  fetch_function (self, deinit);
  fetch_function (self, api_version);
  fetch_function (self, get_system_info);
  fetch_function (self, get_system_av_info);
  fetch_function (self, set_controller_port_device);
  fetch_function (self, reset);
  fetch_function (self, run);
  fetch_function (self, serialize_size);
  fetch_function (self, serialize);
  fetch_function (self, unserialize);
  fetch_function (self, load_game);
  fetch_function (self, unload_game);
  fetch_function (self, get_memory_data);
  fetch_function (self, get_memory_size);

  return self;
}

define_function_getter (RetroCallbackSetter, set_environment)
define_function_getter (RetroCallbackSetter, set_video_refresh)
define_function_getter (RetroCallbackSetter, set_audio_sample)
define_function_getter (RetroCallbackSetter, set_audio_sample_batch)
define_function_getter (RetroCallbackSetter, set_input_poll)
define_function_getter (RetroCallbackSetter, set_input_state)
define_function_getter (RetroInit, init)
define_function_getter (RetroDeinit, deinit)
define_function_getter (RetroApiVersion, api_version)
define_function_getter (RetroGetSystemInfo, get_system_info)
define_function_getter (RetroGetSystemAvInfo, get_system_av_info)
define_function_getter (RetroSetControllerPortDevice, set_controller_port_device)
define_function_getter (RetroReset, reset)
define_function_getter (RetroRun, run)
define_function_getter (RetroSerializeSize, serialize_size)
define_function_getter (RetroSerialize, serialize)
define_function_getter (RetroUnserialize, unserialize)
define_function_getter (RetroLoadGame, load_game)
define_function_getter (RetroUnloadGame, unload_game)
define_function_getter (RetroGetMemoryData, get_memory_data);
define_function_getter (RetroGetMemorySize, get_memory_size);
