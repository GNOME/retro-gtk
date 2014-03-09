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

namespace Retro {

/**
 * Store information on a game.
 * 
 * Used by a {@link Retro.Core} to load a game.
 * 
 * If the core needs a full path to the game file, then it doesn't need the data
 * as it will load it itself: use the default constructor.
 * 
 * If the store doesn't need a full path, the path still be saved as some
 * implementations still need it, and the file will be read and stored into the
 * data field.
 */
public struct GameInfo {
	/**
	 * Path to game, UTF-8 encoded.
	 * 
	 * Usually used as a reference.
	 * May be null if rom was loaded from stdin or similar.
	 * {@link Retro.SystemInfo.need_fullpath} guaranteed that this path is
	 * valid.
	 */
	public string path;
	
	/**
	 * Memory buffer of loaded game.
	 * 
	 * Will be null if {@link Retro.SystemInfo.need_fullpath} was set.
	 */
	[CCode (array_length_type = "gsize")]
	public uint8[] data;
	
	/**
	 * String of implementation specific meta-data.
	 */
	[Deprecated (since = "1.0")]
	public string meta;
	
	/**
	 * Sets a from a game file name.
	 * 
	 * Use this constructor if the {@link Retro.Core} needs a full path to the
	 * game and not its data.
	 * 
	 * @param file_name the game to store information from
	 */
	public GameInfo (string file_name) {
		path = file_name;
		data = {};
	}
	
	/**
	 * Sets a from a game file name.
	 * 
	 * Use this constructor if the {@link Retro.Core} needs the data of the
	 * game.
	 * 
	 * @param file_name the game to store information from
	 * @throws FileError the file can't be read
	 */
	public GameInfo.with_data (string file_name) throws FileError {
		path = file_name;
		FileUtils.get_data (file_name, out data);
	}
}

}

