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
 * A string key-value pair.
 * 
 * Used by a {@link Retro.Core} in its environment callback.
 */
public struct Variable {
	/**
	 * The key of the pair.
	 */
	public string? key;
	
	/**
	 * The value of the pair.
	 * 
	 * If the key is null while retrieving a variable,
	 * value should be the complete environment variables formatted as key-value
	 * pairs separated by semicolons, like so: "key1=value1;key2=value2;...".
	 * 
	 * If the key exists while retrieving a variable, value should be the value associated to
	 * the key.
	 * 
	 * If the key doesn't exist while retrieving a variable, the value should be null.
	 */
	public string? value;
}

}
