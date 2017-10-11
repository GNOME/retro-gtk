// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_CORE_H
#define RETRO_CORE_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include "retro-controller-iterator.h"
#include "retro-controller-type.h"
#include "retro-memory-type.h"

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
void retro_core_set_medias (RetroCore    *self,
                            const gchar **uris);
void retro_core_set_current_media (RetroCore  *self,
                                   guint       media_index,
                                   GError    **error);
void retro_core_reset (RetroCore *self);
void retro_core_run (RetroCore *self);
gboolean retro_core_get_can_access_state (RetroCore *self);
guint8 *retro_core_get_state (RetroCore  *self,
                              gsize      *length,
                              GError    **error);
void retro_core_set_state (RetroCore     *self,
                           const guint8  *data,
                           gsize          length,
                           GError       **error);
gsize retro_core_get_memory_size (RetroCore       *self,
                                  RetroMemoryType  memory_type);
guint8 *retro_core_get_memory (RetroCore       *self,
                              RetroMemoryType  memory_type,
                              gsize           *length);
void retro_core_set_memory (RetroCore       *self,
                            RetroMemoryType  memory_type,
                            guint8          *data,
                            gsize            length);
void retro_core_poll_controllers (RetroCore *self);
gint16 retro_core_get_controller_input_state (RetroCore           *self,
                                              uint                 port,
                                              RetroControllerType  controller_type,
                                              guint                index,
                                              guint                id);
guint64 retro_core_get_controller_capabilities (RetroCore *self);
void retro_core_set_controller (RetroCore       *self,
                                guint            port,
                                RetroController *controller);
void retro_core_set_keyboard (RetroCore *self,
                              GtkWidget *widget);
void retro_core_remove_controller (RetroCore *self,
                                   guint      port);
RetroControllerIterator *retro_core_iterate_controllers (RetroCore *self);

G_END_DECLS

#endif /* RETRO_CORE_H */
