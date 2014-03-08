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

#include <glib.h>

typedef struct _RetroVariable RetroVariable;
struct _RetroVariable {
	gchar* key;
	gchar* value;
};

void retro_environment_set_string (gpointer data, const gchar *value) {
	*((const gchar **) data) = value;
}

void retro_environment_set_variable_value (gpointer data, const gchar *value) {
	((RetroVariable *) data)->value = value ? g_strdup (value): NULL;
}

gchar *retro_environment_get_variable_key (gpointer data) {
	return ((RetroVariable *) data)->key;
}

