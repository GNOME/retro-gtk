#pragma once

#include <gio/gio.h>
#define RETRO_GTK_USE_UNSTABLE_API
#include <retro-gtk/retro-gtk.h>

G_BEGIN_DECLS

#define RETRO_TYPE_REFTEST_FILE (retro_reftest_file_get_type())

G_DECLARE_FINAL_TYPE (RetroReftestFile, retro_reftest_file, RETRO, REFTEST_FILE, GObject)

RetroReftestFile *retro_reftest_file_new (GFile *file);
const gchar *retro_reftest_file_peek_path (RetroReftestFile *self);
RetroCore *retro_reftest_file_get_core (RetroReftestFile  *self,
                                        GError           **error);
GList *retro_reftest_file_get_frames (RetroReftestFile *self);
gchar **retro_reftest_file_get_tests (RetroReftestFile  *self,
                                      guint              frame,
                                      gsize             *length,
                                      GError           **error);
GFile *retro_reftest_file_get_video (RetroReftestFile  *self,
                                     guint              frame,
                                     GError           **error);

G_END_DECLS
