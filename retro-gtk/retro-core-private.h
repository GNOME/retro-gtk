// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_CORE_PRIVATE_H
#define RETRO_CORE_PRIVATE_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-core.h"
#include "retro-disk-control-callback.h"
#include "retro-input.h"
#include "retro-input-descriptor.h"
#include "retro-module.h"
#include "retro-options.h"
#include "retro-pixel-format.h"
#include "retro-rotation.h"

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
  GHashTable *controllers;
  GtkWidget *keyboard_widget;
  gulong key_press_event_id;
  gulong key_release_event_id;
  RetroOptions *options;
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

G_END_DECLS

#endif /* RETRO_CORE_PRIVATE_H */
