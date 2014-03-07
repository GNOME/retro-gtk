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

public struct GameInfo {
	/**
	 * Path to game, UTF-8 encoded.
	 * 
	 * Usually used as a reference.
	 * May be null if rom was loaded from stdin or similar.
	 * {@link [Retro.SystemInfo.need_fullpath]} guaranteed that this path is
	 * valid.
	 */
	public string  path;
	
	/**
	 * Memory buffer of loaded game.
	 * 
	 * Will be null if {@link [Retro.SystemInfo.need_fullpath]} was set.
	 */
	[CCode (array_length_type = "gsize")]
	public uint8[] data;
	
	/**
	 * String of implementation specific meta-data.
	 */
	public string  meta;
	
	public GameInfo (string filename) throws FileError {
		path = filename;
		FileUtils.get_data (filename, out data);
	}
}

}

