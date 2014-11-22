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

public enum MemoryType {
	TYPE_MASK               = 0xff,
	SAVE_RAM                = 0,
	RTC                     = 1,
	SYSTEM_RAM              = 2,
	VIDEO_RAM               = 3,
	SNES_BSX_RAM            = (1 << 8) | MemoryType.SAVE_RAM,
	SNES_BSX_PRAM           = (2 << 8) | MemoryType.SAVE_RAM,
	SNES_SUFAMI_TURBO_A_RAM = (3 << 8) | MemoryType.SAVE_RAM,
	SNES_SUFAMI_TURBO_B_RAM = (4 << 8) | MemoryType.SAVE_RAM,
	SNES_GAME_BOY_RAM       = (5 << 8) | MemoryType.SAVE_RAM,
	SNES_GAME_BOY_RTC       = (6 << 8) | MemoryType.RTC;

	public MemoryType get_basic_type () {
		var basic_type = this & MemoryType.TYPE_MASK;
		return basic_type;
	}
}

}

