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

public struct SubsystemMemoryInfo {
	string extension;
	uint type;
}

public struct SubsystemRomInfo {
	string desc;
	string valid_extensions;
	bool need_fullpath;
	bool block_extract;
	bool required;
	SubsystemMemoryInfo? memory;
	uint num_memory;
}

public struct SubsystemInfo {
	string desc;
	string ident;
	SubsystemRomInfo? roms;
	uint num_roms;
	uint id;
}

}

