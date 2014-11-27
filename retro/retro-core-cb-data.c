/* Retro  GObject libretro wrapper.
 * Copyright (C) 2014  Adrien Plazas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#include "retro-core-cb-data.h"

#include <glib.h>

#define RETRO_CORE_CB_DATA_STACK_SIZE 32

static _Thread_local void *cb_data[RETRO_CORE_CB_DATA_STACK_SIZE];
static _Thread_local int i = 0;

void retro_core_push_cb_data (void *self) {
	if (i == RETRO_CORE_CB_DATA_STACK_SIZE) {
		g_printerr ("Error: RetroCore callback data stack overflow.\n");
		g_assert_not_reached ();
	}
	cb_data[i] = self;
	i++;
}

void retro_core_pop_cb_data (void *self) {
	if (i == 0) {
		g_printerr ("Error: RetroCore callback data stack underflow.\n");
		g_assert_not_reached ();
	}
	i--;
}

void *retro_core_get_cb_data () {
	if (i == 0) {
		g_printerr ("Error: RetroCore callback data segmentation fault.\n");
		g_assert_not_reached ();
	}
	return cb_data[i - 1];
}

