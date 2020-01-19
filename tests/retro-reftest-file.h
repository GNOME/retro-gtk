/* retro-reftest-file.h
 *
 * Copyright 2018 Adrien Plazas <kekun.plazas@laposte.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <gio/gio.h>
#include <retro-gtk/retro-gtk.h>

G_BEGIN_DECLS

#define RETRO_TYPE_REFTEST_FILE (retro_reftest_file_get_type())

G_DECLARE_FINAL_TYPE (RetroReftestFile, retro_reftest_file, RETRO, REFTEST_FILE, GObject)

RetroReftestFile *retro_reftest_file_new (GFile *file);
const gchar *retro_reftest_file_peek_path (RetroReftestFile *self);
RetroCore *retro_reftest_file_get_core (RetroReftestFile  *self,
                                        GError           **error);
GArray *retro_reftest_file_get_controllers (RetroReftestFile  *self,
                                            gsize             *length,
                                            GError           **error);
gboolean retro_reftest_file_has_options (RetroReftestFile *self);
GHashTable *retro_reftest_file_get_options (RetroReftestFile  *self,
                                            GError           **error);
GList *retro_reftest_file_get_frames (RetroReftestFile *self);
gboolean retro_reftest_file_has_state (RetroReftestFile  *self,
                                       guint              frame,
                                       GError           **error);
gchar *retro_reftest_file_get_state (RetroReftestFile  *self,
                                     guint              frame,
                                     GError           **error);
gboolean retro_reftest_file_has_video (RetroReftestFile  *self,
                                       guint              frame,
                                       GError           **error);
GFile *retro_reftest_file_get_video (RetroReftestFile  *self,
                                     guint              frame,
                                     GError           **error);
GHashTable *retro_reftest_file_get_controller_states (RetroReftestFile  *self,
                                                      guint              frame,
                                                      GError           **error);

G_END_DECLS
