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

using Gtk;
using Gdk;

namespace RetroGtk {

public class Gamepad : Object, ControllerDevice {
	public Widget widget { get; construct; }
	public GamepadConfiguration configuration { get; construct set; }

	private HashTable<uint?, bool?> key_state;
	private Keymap keymap;

	public Gamepad (Widget widget) {
		Object (widget: widget);
	}

	public Gamepad.with_configuration (Widget widget, GamepadConfiguration configuration) {
		Object (widget: widget, configuration: configuration);
	}

	construct {
		widget.set_can_focus (true);

		widget.focus_in_event.connect (() => {
			widget.has_focus = true;
			return false;
		});

		widget.focus_out_event.connect (() => {
			widget.has_focus = false;

			foreach (var key in key_state.get_keys ())
				key_state[key] = false;

			return false;
		});

		widget.button_press_event.connect (() => {
			widget.grab_focus ();
			return false;
		});

		widget.key_press_event.connect (on_key_press_event);

		widget.key_release_event.connect (on_key_release_event);

		key_state = new HashTable<uint?, bool?> (int_hash, int_equal);

		keymap = Keymap.get_default ();

		if (configuration == null) {
			configuration = new GamepadConfiguration ();
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
		uint16 hardware_keycode = configuration.get_button_key (button);

		if (key_state.contains (hardware_keycode)) {
			return key_state.lookup ((uint) hardware_keycode);
		}

		return false;
	}

	private bool on_key_press_event (Widget source, EventKey event) {
		if (key_state.contains ((uint) event.hardware_keycode)) {
			key_state.replace ((uint) event.hardware_keycode, true);
		}
		else  {
			key_state.insert ((uint) event.hardware_keycode, true);
		}

		return false;
	}

	private bool on_key_release_event (Widget source, EventKey event) {
		if (key_state.contains ((uint) event.hardware_keycode)) {
			key_state.replace ((uint) event.hardware_keycode, false);
		}

		return false;
	}
}

}

