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

public class KeyboardState : Object {
	public Widget widget { get; construct; }

	private HashTable<uint?, bool?> key_state;
	private Keymap keymap;

	public signal void key_state_changed ();

	public KeyboardState (Widget widget) {
		Object (widget: widget);
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

			key_state_changed ();

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
	}

	private bool on_key_press_event (Widget source, EventKey event) {
		if (key_state.contains ((uint) event.hardware_keycode)) {
			key_state.replace ((uint) event.hardware_keycode, true);
		}
		else  {
			key_state.insert ((uint) event.hardware_keycode, true);
		}

		key_state_changed ();

		return false;
	}

	private bool on_key_release_event (Widget source, EventKey event) {
		if (key_state.contains ((uint) event.hardware_keycode)) {
			key_state.replace ((uint) event.hardware_keycode, false);
		}

		key_state_changed ();

		return false;
	}

	public bool get_key_state (uint16 hardware_keycode) {
		if (key_state.contains (hardware_keycode)) {
			return key_state.lookup ((uint) hardware_keycode);
		}

		return false;
	}
}

}

