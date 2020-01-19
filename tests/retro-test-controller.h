/* retro-test-controller.h
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

#include <retro-gtk/retro-gtk.h>

G_BEGIN_DECLS

#define RETRO_TYPE_TEST_CONTROLLER (retro_test_controller_get_type())

G_DECLARE_FINAL_TYPE (RetroTestController, retro_test_controller, RETRO, TEST_CONTROLLER, GObject)

typedef struct {
  RetroControllerType type;
  guint id;
  guint index;
  gint16 value;
} RetroControllerState;

RetroTestController *retro_test_controller_new (RetroControllerType controller_type);
void retro_test_controller_reset (RetroTestController *self);
void retro_test_controller_set_input_state (RetroTestController  *self,
                                            RetroControllerState *state);
guint16 retro_test_controller_get_rumble_state (RetroTestController *self,
                                                RetroRumbleEffect    effect);

G_END_DECLS
