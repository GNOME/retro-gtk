// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include "retro-controller-iterator.h"
#include "retro-memory-type.h"
#include "retro-option-iterator.h"

G_BEGIN_DECLS

#define RETRO_TYPE_CORE (retro_core_get_type())

G_DECLARE_FINAL_TYPE (RetroCore, retro_core, RETRO, CORE, GObject)

RetroCore *retro_core_new (const gchar *filename);
guint retro_core_get_api_version (RetroCore *self);
const gchar *retro_core_get_filename (RetroCore *self);
const gchar *retro_core_get_system_directory (RetroCore *self);
void retro_core_set_system_directory (RetroCore   *self,
                                      const gchar *system_directory);
const gchar *retro_core_get_content_directory (RetroCore *self);
void retro_core_set_content_directory (RetroCore   *self,
                                       const gchar *content_directory);
const gchar *retro_core_get_save_directory (RetroCore *self);
void retro_core_set_save_directory (RetroCore   *self,
                                    const gchar *save_directory);
gboolean retro_core_get_is_initiated (RetroCore *self);
gboolean retro_core_get_game_loaded (RetroCore *self);
gboolean retro_core_get_support_no_game (RetroCore *self);
gdouble retro_core_get_frames_per_second (RetroCore *self);
void retro_core_boot (RetroCore  *self,
                      GError    **error);
void retro_core_set_medias (RetroCore           *self,
                            const gchar * const *uris);
void retro_core_set_current_media (RetroCore  *self,
                                   guint       media_index,
                                   GError    **error);
void retro_core_run (RetroCore *self);
void retro_core_stop (RetroCore *self);
void retro_core_reset (RetroCore *self);
void retro_core_iteration (RetroCore *self);
gboolean retro_core_get_can_access_state (RetroCore *self);
void retro_core_save_state (RetroCore    *self,
                            const gchar  *filename,
                            GError      **error);
void retro_core_load_state (RetroCore    *self,
                            const gchar  *filename,
                            GError      **error);
gsize retro_core_get_memory_size (RetroCore       *self,
                                  RetroMemoryType  memory_type);
void retro_core_save_memory (RetroCore        *self,
                             RetroMemoryType   memory_type,
                             const gchar      *filename,
                             GError          **error);
void retro_core_load_memory (RetroCore        *self,
                             RetroMemoryType   memory_type,
                             const gchar      *filename,
                             GError          **error);
void retro_core_set_default_controller (RetroCore           *self,
                                        RetroControllerType  controller_type,
                                        RetroController     *controller);
void retro_core_set_controller (RetroCore       *self,
                                guint            port,
                                RetroController *controller);
void retro_core_set_keyboard (RetroCore *self,
                              GtkWidget *widget);
RetroControllerIterator *retro_core_iterate_controllers (RetroCore *self);
guint retro_core_get_runahead (RetroCore *self);
void retro_core_set_runahead (RetroCore *self,
                              guint      runahead);
gdouble retro_core_get_speed_rate (RetroCore *self);
void retro_core_set_speed_rate (RetroCore *self,
                                gdouble    speed_rate);
gboolean retro_core_has_option (RetroCore   *self,
                                const gchar *key);
RetroOption *retro_core_get_option (RetroCore   *self,
                                    const gchar *key);
RetroOptionIterator *retro_core_iterate_options (RetroCore *self);
void retro_core_override_option_default (RetroCore   *self,
                                         const gchar *key,
                                         const gchar *value);

G_END_DECLS
