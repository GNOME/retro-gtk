// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-core.h"
#include "retro-disk-control-callback-private.h"
#include "retro-input.h"
#include "retro-input-descriptor-private.h"
#include "retro-module-private.h"
#include "retro-option-private.h"
#include "retro-pixel-format-private.h"
#include "retro-rotation-private.h"

G_BEGIN_DECLS

typedef struct {
  void (*callback) (guchar down, guint keycode, guint32 character, guint16 key_modifiers);
} RetroKeyboardCallback;

struct _RetroCore
{
  GObject parent_instance;
  gchar *filename;
  gchar *system_directory;
  gchar *libretro_path;
  gchar *content_directory;
  gchar *save_directory;
  gboolean is_initiated;
  gboolean game_loaded;
  gboolean support_no_game;
  gdouble frames_per_second;

  RetroModule *module;
  RetroDiskControlCallback *disk_control_callback;
  gchar **media_uris;
  RetroSystemInfo *system_info;
  gfloat aspect_ratio;
  gboolean overscan;
  RetroPixelFormat pixel_format;
  RetroRotation rotation;
  gdouble sample_rate;

  RetroKeyboardCallback keyboard_callback;
  RetroController *default_controllers[RETRO_CONTROLLER_TYPE_COUNT];
  GHashTable *controllers;
  GtkWidget *keyboard_widget;
  gulong key_press_event_id;
  gulong key_release_event_id;
  GHashTable *options;
  GHashTable *option_overrides;
  gboolean variable_updated;
  guint runahead;
  gssize run_remaining;
  gdouble speed_rate;
  glong main_loop;
  gint16 *audio_samples;
  gulong audio_samples_length;
};

void retro_core_push_cb_data (RetroCore *self);
void retro_core_pop_cb_data (void);
RetroCore *retro_core_get_cb_data (void);
const gchar *retro_core_get_libretro_path (RetroCore *self);
void retro_core_set_support_no_game (RetroCore *self,
                                     gboolean   support_no_game);
gchar *retro_core_get_name (RetroCore *self);
void retro_core_set_system_av_info (RetroCore         *self,
                                    RetroSystemAvInfo *system_av_info);
void retro_core_set_geometry (RetroCore         *self,
                              RetroGameGeometry *geometry);
void retro_core_poll_controllers (RetroCore *self);
gint16 retro_core_get_controller_input_state (RetroCore  *self,
                                              uint        port,
                                              RetroInput *input);
guint64 retro_core_get_controller_capabilities (RetroCore *self);
void retro_core_set_controller_port_device (RetroCore           *self,
                                            guint                port,
                                            RetroControllerType  controller_type);
void retro_core_set_controller_descriptors (RetroCore            *self,
                                            RetroInputDescriptor *input_descriptors,
                                            gsize                 length);
gboolean retro_core_is_running_ahead (RetroCore *self);
void retro_core_insert_variable (RetroCore           *self,
                                 const RetroVariable *variable);
gboolean retro_core_get_variable_update (RetroCore *self);
gdouble retro_core_get_sample_rate (RetroCore *self);

G_END_DECLS
