/* Device.vala  Input device description.
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

namespace Retro.Device {

public static const uint TYPE_MASK = 0xff;

public enum Type {
	NONE,
	JOYPAD,
	MOUSE,
	KEYBOARD,
	LIGHTGUN,
	ANALOG,
	POINTER,
	SENSOR_ACCELEROMETER,
	JOYPAD_MULTITAP      = (1 << 8) | Type.JOYPAD,
	LIGHTGUN_SUPER_SCOPE = (1 << 8) | Type.LIGHTGUN,
	LIGHTGUN_JUSTIFIER   = (2 << 8) | Type.LIGHTGUN,
	LIGHTGUN_JUSTIFIERS  = (3 << 8) | Type.LIGHTGUN;
	
	public Type get_basic_type () {
		var basic_type = this & TYPE_MASK;
		return basic_type > Type.SENSOR_ACCELEROMETER ? Type.NONE : basic_type;
	}
}

public enum JoypadId {
	B,
	Y,
	SELECT,
	START,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	A,
	X,
	L,
	R,
	L2,
	R2,
	L3,
	R3
}

public enum AnalogIndex {
	LEFT,
	RIGHT
}

public enum AnalogId {
	X,
	Y
}

public enum MouseId {
	X,
	Y,
	LEFT,
	RIGHT
}

public enum LightgunId {
	X,
	Y,
	TRIGGER,
	CURSOR,
	TURBO,
	PAUSE,
	START
}

public enum PointerId {
	X,
	Y,
	PRESSED
}

public enum SensorId {
	ACCELEROMETER_X,
	ACCELEROMETER_Y,
	ACCELEROMETER_Z
}

public struct InputDescriptor {
   uint port;
   Device.Type device;
   uint index;
   uint id;
	
   string description;
}

}

