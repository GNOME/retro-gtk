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
 * Information on the system of a {@link Core}.
 */
public struct SystemInfo {
	/**
	 * The name of a core.
	 *
	 * Should not contain any version numbers, etc.
	 */
	public string library_name;

	/**
	 * The version of a core.
	 */
	public string library_version;

	/**
	 * Extensions of files that the core can run.
	 *
	 * A string listing probably rom extensions the core will be able to load, separated with pipe.
	 * I.e. "bin|rom|iso".
	 *
	 * Typically used for a GUI to filter out extensions.
	 */
	public string valid_extensions;

	/**
	 * Whether the core need to read the game files by itself.
	 *
	 * If true, {@link Core.load_game} and similar methods doesn't need {Retro.GameInfo.data}
	 * to be set but needs {Retro.GameInfo.path} to be set.
	 *
	 * If false, {Retro.GameInfo.data} must be valid, but as some implemetations still check for
	 * {Retro.GameInfo.path}, it is recommended to set it too.
	 *
	 * This is typically set to true for Libretro implementations that must load from file.
	 * Implementations should strive for setting this to false, as it allows the frontend to perform
	 * patching, etc.
	 */
	public bool need_fullpath;

	/**
	 * Whether the core need to extract archives by itself.
	 *
	 * If true, the frontend is not allowed to extract any archives before loading the real ROM.
	 * Necessary for certain Libretro implementations that load games from zipped archives.
	 */
	public bool block_extract;
}

}

