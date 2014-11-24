/* Engine.vala  A simple frontend for libretro.
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

using Gtk;
using Cairo;

namespace RetroGtk {

public enum GamepadButtonType {
	ACTION_DOWN,
	ACTION_LEFT,
	SELECT,
	START,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	ACTION_RIGHT,
	ACTION_UP,
	SHOULDER_L,
	SHOULDER_R,
	SHOULDER_L2,
	SHOULDER_R2,
	STICK_L,
	STICK_R,
	HOME;

	public static size_t size () {
		return 1 + (size_t) GamepadButtonType.HOME;
	}
}

}

