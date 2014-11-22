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

using Gtk;
using Gdk;

namespace Flicky {

public class KeyboardBox : EventBox {
	private HashTable<uint?, bool?> key_state;
	private Keymap keymap;

	public bool verbose { set; get; default = false; }

	construct {
		set_can_focus (true);

		focus_in_event.connect (() => {
			has_focus = true;
			return false;
		});

		focus_out_event.connect (() => {
			has_focus = false;
			return false;
		});

		button_press_event.connect (() => {
			grab_focus ();
			return false;
		});

		key_press_event.connect (on_key_press_event);

		key_release_event.connect (on_key_release_event);

		key_state = new HashTable<uint?, bool?> (int_hash, int_equal);

		keymap = Keymap.get_default ();
	}

	private bool on_key_press_event (Widget source, EventKey event) {
		if (verbose) print_event_key (event);

		if (key_state.contains ((uint) event.hardware_keycode)) {
			key_state.replace ((uint) event.hardware_keycode, true);
		}
		else  {
			key_state.insert ((uint) event.hardware_keycode, true);
		}

		return false;
	}

	private bool on_key_release_event (Widget source, EventKey event) {
		if (verbose) print_event_key (event);

		if (key_state.contains ((uint) event.hardware_keycode)) {
			key_state.replace ((uint) event.hardware_keycode, false);
		}

		return false;
	}

	public bool get_key_state (uint16 hardware_keycode) {
		if (key_state.contains (hardware_keycode)) {
			return key_state.lookup ((uint) hardware_keycode);
		}

		return false;
	}

	private static void print_event_key (EventKey event) {
		stdout.printf ("str: %s, send event: %d, time: %u, state: %d, keyval: %u, length: %d, hw keycode: %u, group: %u, is mod: %u\n", event.str, event.send_event, event.time, (int) event.state, event.keyval, event.length, event.hardware_keycode, event.group, event.is_modifier );
	}
}

}

