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

namespace Memory {
	public static const uint TYPE_MASK = 0xff;
	
	public enum Type {
		SAVE_RAM,
		RTC,
		SYSTEM_RAM,
		VIDEO_RAM,
		SNES_BSX_RAM            = (1 << 8) | Type.SAVE_RAM,
		SNES_BSX_PRAM           = (2 << 8) | Type.SAVE_RAM,
		SNES_SUFAMI_TURBO_A_RAM = (3 << 8) | Type.SAVE_RAM,
		SNES_SUFAMI_TURBO_B_RAM = (4 << 8) | Type.SAVE_RAM,
		SNES_GAME_BOY_RAM       = (5 << 8) | Type.SAVE_RAM,
		SNES_GAME_BOY_RTC       = (6 << 8) | Type.RTC;
	
		public Type get_basic_Type () {
			var basic_type = this & TYPE_MASK;
			return basic_type;
		}
	}
}

}

