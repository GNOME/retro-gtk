/* KeyboardHandler.vala  Handle keyboard input.
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

namespace Retro {

public class KeyboardHandler : EventBox {
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
	}
	
	private bool on_key_press_event (Widget source, EventKey event) {
		print_event_key (event);
		
		return false;
	}
	
	private bool on_key_release_event (Widget source, EventKey event) {
		print_event_key (event);
		
		return false;
	}
	
	private static void print_event_key (EventKey event) {
		stdout.printf ("str: %s, send event: %d, time: %u, state: %d, keyval: %u, length: %d, hw keycode: %u, group: %u, is mod: %u\n", event.str, event.send_event, event.time, (int) event.state, event.keyval, event.length, event.hardware_keycode, event.group, event.is_modifier );
	}
}

}

