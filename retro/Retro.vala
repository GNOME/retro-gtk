/* Copyright (C) 2014  Adrien Plazas
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

namespace Retro {

/**
 * The version of libretro implemented by Retro.
 */
public static const uint API_VERSION = 1;

/**
 * Error type thrown by interfaces when accessing one of their Core's
 * callback.
 */
public errordomain CbError {
	NO_CORE,
	NO_CALLBACK
}

/**
 * Gets a module's system informations in a simple and light way.
 *
 * @param module_name the file name of the module to get the system
 * informations
 * @return the system infos of the module or //null//
 */
public SystemInfo? get_system_info (string module_name) {
	var module = GLib.Module.open (module_name, ModuleFlags.BIND_LAZY | ModuleFlags.BIND_LOCAL);

	void *function;
	module.symbol ("retro_get_system_info", out function);

	if (function == null) return null;

	unowned SystemInfo info;
	((GetSystemInfo) function) (out info);

	return info;
}

}

