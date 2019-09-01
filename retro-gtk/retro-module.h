// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-controller-type.h"
#include "retro-game-info.h"
#include "retro-memory-type.h"
#include "retro-system-av-info.h"
#include "retro-system-info.h"

G_BEGIN_DECLS

typedef gboolean (*RetroLoadGame) (RetroGameInfo *game);
typedef gboolean (*RetroSerialize) (guint8 *data, gsize size);
typedef gboolean (*RetroUnserialize) (guint8 *data, gsize size);
typedef gsize (*RetroGetMemorySize) (RetroMemoryType id);
typedef gsize (*RetroSerializeSize) ();
typedef guint (*RetroApiVersion) ();
typedef void (*RetroCallbackSetter) (gpointer cb);
typedef void (*RetroDeinit) ();
typedef gpointer (*RetroGetMemoryData) (RetroMemoryType id);
typedef void (*RetroGetSystemAvInfo) (RetroSystemAvInfo *info);
typedef void (*RetroGetSystemInfo) (RetroSystemInfo *info);
typedef void (*RetroInit) ();
typedef void (*RetroReset) ();
typedef void (*RetroRun) ();
typedef void (*RetroSetControllerPortDevice) (guint port, RetroControllerType controller_type);
typedef void (*RetroUnloadGame) ();

#define RETRO_TYPE_MODULE (retro_module_get_type())

G_DECLARE_FINAL_TYPE (RetroModule, retro_module, RETRO, MODULE, GObject)

RetroModule *retro_module_new (const gchar *file_name);
const gchar *retro_module_get_file_name (RetroModule *self);
RetroCallbackSetter retro_module_get_set_environment (RetroModule *self);
RetroCallbackSetter retro_module_get_set_video_refresh (RetroModule *self);
RetroCallbackSetter retro_module_get_set_audio_sample (RetroModule *self);
RetroCallbackSetter retro_module_get_set_audio_sample_batch (RetroModule *self);
RetroCallbackSetter retro_module_get_set_input_poll (RetroModule *self);
RetroCallbackSetter retro_module_get_set_input_state (RetroModule *self);
RetroInit retro_module_get_init (RetroModule *self);
RetroDeinit retro_module_get_deinit (RetroModule *self);
RetroApiVersion retro_module_get_api_version (RetroModule *self);
RetroGetSystemInfo retro_module_get_get_system_info (RetroModule *self);
RetroGetSystemAvInfo retro_module_get_get_system_av_info (RetroModule *self);
RetroSetControllerPortDevice retro_module_get_set_controller_port_device (RetroModule *self);
RetroReset retro_module_get_reset (RetroModule *self);
RetroRun retro_module_get_run (RetroModule *self);
RetroSerializeSize retro_module_get_serialize_size (RetroModule *self);
RetroSerialize retro_module_get_serialize (RetroModule *self);
RetroUnserialize retro_module_get_unserialize (RetroModule *self);
RetroLoadGame retro_module_get_load_game (RetroModule *self);
RetroUnloadGame retro_module_get_unload_game (RetroModule *self);
RetroGetMemoryData retro_module_get_get_memory_data (RetroModule *self);
RetroGetMemorySize retro_module_get_get_memory_size (RetroModule *self);

G_END_DECLS
