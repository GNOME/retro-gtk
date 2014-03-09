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

namespace Retro.Sensor {
	public enum Action {
		ACCELEROMETER_ENABLE,
		ACCELEROMETER_DISABLE
	}
	
	public enum Accelerometer {
		X,
		Y,
		Z
	}
	
	[CCode (has_target = false)]
	public delegate bool SetState (uint port, Action action, uint rate);
	
	[CCode (has_target = false)]
	public delegate float GetInput (uint port, uint id);
	
	public struct Interface {
		SetState set_sensor_state;
		GetInput get_sensor_input;
	}
}

