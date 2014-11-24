/* RetroGtk  Building blocks for a Retro frontend.
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

namespace RetroGtk {

public class KeyboardGamepadAdapter : Object, ControllerDevice {
	public Keyboard keyboard { get; construct; }
	public KeyboardGamepadConfiguration configuration { get; private construct set; }

	public KeyboardGamepadAdapter (Keyboard keyboard) {
		Object (keyboard: keyboard);
	}

	public KeyboardGamepadAdapter.with_configuration (Keyboard keyboard, KeyboardGamepadConfiguration configuration) {
		Object (keyboard: keyboard, configuration: configuration);
	}

	construct {
		if (configuration == null) {
			configuration = new KeyboardGamepadConfiguration ();
			configuration.set_to_default ();
		}
	}

	public void poll () {}

	public int16 get_input_state (Retro.DeviceType device, uint index, uint id) {
		if ((Retro.DeviceType) device != Retro.DeviceType.JOYPAD) return 0;

		return get_button_pressed ((GamepadButtonType) id) ? int16.MAX : 0;
	}

	public uint64 get_device_capabilities () {
		return 1 << Retro.DeviceType.JOYPAD;
	}

	public bool get_button_pressed (GamepadButtonType button) {
		return keyboard.get_key_state (configuration.get_button_key (button));
	}
}

}

