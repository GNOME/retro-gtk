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

public enum DeviceType {
	TYPE_MASK = 0xff,
	
	/**
	 * No device type.
	 */
	NONE = 0,
	
	/**
	 * A classic joypad.
	 * 
	 * The joypad is called RetroPad.
	 * 
	 * It is essentially a Super Nintendo controller,
	 * but with additional L2/R2/L3/R3 buttons, similar to a PlayStation DualShock.
	 */
	JOYPAD = 1,
	
	/**
	 * A simple mouse.
	 * 
	 * It is similar to Super Nintendo's mouse (with two axes and two buttons).
	 * 
	 * X and Y coordinates are reported relatively to last poll (poll callback).
	 * It is up to the libretro implementation to keep track of where the mouse pointer
	 * is supposed to be on the screen.
	 * 
	 * The frontend must make sure not to interfere with its own hardware mouse pointer.
	 */
	MOUSE = 2,
	
	/**
	 * A keyboard.
	 * 
	 * Keyboard device lets one poll for hardware key pressed.
	 * 
	 * It is poll based, so input callback will return with the current pressed state.
	 */
	KEYBOARD = 3,
	
	/**
	 * A lightgun.
	 * 
	 * X/Y coordinates are reported relatively to last poll, similar to mouse.
	 */
	LIGHTGUN = 4,
	
	/**
	 * An analog joypad.
	 * 
	 * It is an extension to the joypad (RetroPad).
	 * 
	 * Similar to DualShock it adds two analog sticks.
	 * 
	 * This is treated as a separate device type as it returns values in the full analog range
	 * of [-0x8000, 0x7fff]. Positive X axis is right. Positive Y axis is down.
	 * Only use the analog type when polling for analog values of the axes.
	 */
	ANALOG = 5,
	
	/**
	 * An abstract pointing mecanism (e.g. touch).
	 * 
	 * This allows libretro to query in absolute coordinates where on the screen a pointer
	 * (a mouse or something similar) is being placed.
	 * For a touch centric device, coordinates reported are the coordinates of the press.
	 * 
	 * Coordinates in X and Y are reported as:
	 * [-0x7fff, 0x7fff]: -0x7fff corresponds to the far left/top of the screen,
	 * and 0x7fff corresponds to the far right/bottom of the screen.
	 * 
	 * The "screen" is here defined as area that is passed to the frontend and later displayed on
	 * the monitor.
	 * The frontend is free to scale/resize this screen as it sees fit, however,
	 * (X, Y) = (-0x7fff, -0x7fff) will correspond to the top-left pixel of the game image, etc.
	 * 
	 * To check if the pointer coordinates are valid (e.g. a touch display actually being touched),
	 * {@link Retro.Device.PointerId.PRESSED} returns 1 or 0.
	 * If using a mouse, {@link Retro.Device.PointerId.PRESSED} will usually correspond to the left
	 * mouse button.
	 * {@link Retro.Device.PointerId.PRESSED} will only return 1 if the pointer is inside the game
	 * screen.
	 * 
	 * For multi-touch, the index argument of {@link Retro.Core.InputState} can be used to
	 * successively query more presses.
	 * If index = 0 returns 1 for {@link Retro.Device.PointerId.PRESSED}, coordinates can be extracted
	 * with {@link Retro.Device.PointerId.X}, {@link Retro.Device.PointerId.Y} for index = 0.
	 * One can then query {@link Retro.Device.PointerId.PRESSED}, {@link Retro.Device.PointerId.X},
	 * {@link Retro.Device.PointerId.Y} with index = 1, and so on.
	 * Eventually {@link Retro.Device.PointerId.PRESSED} will return 0 for an index. No further
	 * presses are registered at this point.
	 */
	POINTER = 6,
	
	/**
	 * A joypad multitap.
	 * 
	 * It is a specialization of the {@link Retro.Device.Type.JOYPAD}.
	 * 
	 * It should only be used in {@link Retro.Core.set_controller_port_device} to inform libretro
	 * implementations about use of a joypad multitap.
	 * 
	 * In input state callback, however, the device will be {@link Retro.Device.Type.JOYPAD}.
	 */
	JOYPAD_MULTITAP = (1 << 8) | DeviceType.JOYPAD,
	
	/**
	 * A Super Nintendo Super Scope.
	 * 
	 * It is a specialization of the {@link Retro.Device.Type.LIGHTGUN}.
	 * 
	 * It should only be used in {@link Retro.Core.set_controller_port_device} to inform libretro
	 * implementations about use of a Super Nintendo Super Scope.
	 * 
	 * In input state callback, however, the device will be {@link Retro.Device.Type.LIGHTGUN}.
	 */
	LIGHTGUN_SUPER_SCOPE = (1 << 8) | DeviceType.LIGHTGUN,
	
	/**
	 * A Konami Justifier.
	 * 
	 * It is a specialization of the {@link Retro.Device.Type.LIGHTGUN}.
	 * 
	 * It should only be used in {@link Retro.Core.set_controller_port_device} to inform libretro
	 * implementations about use of a Konami Justifier.
	 * 
	 * In input state callback, however, the device will be {@link Retro.Device.Type.LIGHTGUN}.
	 */
	LIGHTGUN_JUSTIFIER = (2 << 8) | DeviceType.LIGHTGUN,
	
	/**
	 * Konami Justifiers.
	 * 
	 * It is a specialization of the {@link Retro.Device.Type.LIGHTGUN}.
	 * 
	 * It should only be used in {@link Retro.Core.set_controller_port_device} to inform libretro
	 * implementations about use of Konami Justifiers.
	 * 
	 * In input state callback, however, the device will be {@link Retro.Device.Type.LIGHTGUN}.
	 */
	LIGHTGUN_JUSTIFIERS = (3 << 8) | DeviceType.LIGHTGUN;
	
	/**
	 * Gets the basic type of a device type.
	 * 
	 * If the device type is already basic, it will return the same type.
	 * E.g Type.JOYPAD_MULTITAP.get_basic_type () returns Type.JOYPAD,
	 * and Type.JOYPAD.get_basic_type () also returns Type.JOYPAD.
	 * 
	 * @return the basic type of a device type
	 */
	public DeviceType get_basic_type () {
		return this & DeviceType.TYPE_MASK;
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

public struct InputDescriptor {
   uint port;
   DeviceType device;
   uint index;
   uint id;
	
   string description;
}

}

