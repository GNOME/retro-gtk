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

#ifndef __RETRO_CORE_INTERFACES_H__
#define __RETRO_CORE_INTERFACES_H__

#include "retro-internal.h"

typedef struct {
	gpointer log;
} RetroLogCallback;

gboolean retro_core_set_callback_interfaces (RetroCore *self, RetroEnvironmentCommand cmd, gpointer data);

gboolean retro_core_set_log_callback (RetroCore *self, RetroLogCallback *cb);

#endif

