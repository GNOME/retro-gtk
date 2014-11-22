/* Flicky  Building blocks for a Retro frontend.
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

namespace Flicky {

public class KeyboardBoxJoypadAdapter : Object, ControllerDevice {
	public KeyboardBox keyboard { private get; construct; }

	private uint16[] joypad_keys;

	public KeyboardBoxJoypadAdapter (KeyboardBox keyboard) {
		Object (keyboard: keyboard);
	}

	construct {
		// Set the default keys for the joypad
		joypad_keys = new uint16[16];
		set_joypad_key (Retro.JoypadId.B,      39); // QWERTY S
		set_joypad_key (Retro.JoypadId.Y,      38); // QWERTY A
		set_joypad_key (Retro.JoypadId.SELECT, 22); // Backspace
		set_joypad_key (Retro.JoypadId.START,  36); // Enter
		set_joypad_key (Retro.JoypadId.UP,    111); // Up arrow
		set_joypad_key (Retro.JoypadId.DOWN,  116); // Down arrow
		set_joypad_key (Retro.JoypadId.LEFT,  113); // Left arrow
		set_joypad_key (Retro.JoypadId.RIGHT, 114); // Right arrow
		set_joypad_key (Retro.JoypadId.A,      40); // QWERTY D
		set_joypad_key (Retro.JoypadId.X,      25); // QWERTY W
		set_joypad_key (Retro.JoypadId.L,      24); // QWERTY Q
		set_joypad_key (Retro.JoypadId.R,      26); // QWERTY E
		set_joypad_key (Retro.JoypadId.L2,     52); // QWERTY Z
		set_joypad_key (Retro.JoypadId.R2,     54); // QWERTY C
		set_joypad_key (Retro.JoypadId.L3,     10); // QWERTY 1
		set_joypad_key (Retro.JoypadId.R3,     12); // QWERTY 3
	}

	public void  poll () {}

	public int16 get_input_state (Retro.DeviceType device, uint index, uint id) {
		if ((Retro.DeviceType) device != Retro.DeviceType.JOYPAD) return 0;

		return keyboard.get_key_state (get_joypad_key ((Retro.JoypadId) id)) ? int16.MAX : 0;
	}

	public uint64 get_device_capabilities () {
		return 1 << Retro.DeviceType.JOYPAD;
	}

	private void set_joypad_key (Retro.JoypadId id, uint16 hardware_keycode) {
		joypad_keys[id] = hardware_keycode;
	}

	private uint16 get_joypad_key (Retro.JoypadId id) {
		return joypad_keys[id];
	}
}

}

