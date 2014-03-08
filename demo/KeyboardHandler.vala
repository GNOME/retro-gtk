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

public class KeyboardHandler : EventBox, ControllerDevice {
	// TODO use hardware key values instead of key values
	private HashTable<uint?, bool?> key_state;
	private Keymap keymap;
	
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
		print_event_key (event);
		
		if (key_state.contains ((uint) event.hardware_keycode)) {
			key_state.replace ((uint) event.hardware_keycode, true);
		}
		else  {
			key_state.insert ((uint) event.hardware_keycode, true);
		}
		
		return false;
	}
	
	private bool on_key_release_event (Widget source, EventKey event) {
		print_event_key (event);
		
		if (key_state.contains ((uint) event.hardware_keycode)) {
			key_state.replace ((uint) event.hardware_keycode, false);
		}
		
		return false;
	}
	
	private bool get_key_state (uint keyval) {
		if (key_state.contains (keyval)) {
			return key_state.lookup (keyval);
		}
		
		return false;
	}
	
	private static void print_event_key (EventKey event) {
		stdout.printf ("str: %s, send event: %d, time: %u, state: %d, keyval: %u, length: %d, hw keycode: %u, group: %u, is mod: %u\n", event.str, event.send_event, event.time, (int) event.state, event.keyval, event.length, event.hardware_keycode, event.group, event.is_modifier );
	}
	
	private string hardware_keycode_to_string (uint hardware_keycode) {
		KeymapKey[] keys;
		uint[] keyvals;
		keymap.get_entries_for_keycode (hardware_keycode, out keys, out keyvals);
		
		return (keyvals.length > 0) ? keyval_name (keyvals[0]) : "";
	}
	
	public void  poll () {}
	
	public int16 get_input_state (Device.Type device, uint index, uint id) {
		switch (device) {
			case Device.Type.KEYBOARD:
				return get_keyboard_state (index, id);
			case Device.Type.JOYPAD:
				return get_joypad_state ((Device.JoypadId) id);
			default:
				return 0;
		}
	}
	
	private int16 get_keyboard_state (uint index, uint id) {
		return 0;
	}
	
	private int16 get_joypad_state (Device.JoypadId id) {
		switch (id) {
			case Device.JoypadId.B:
				return (int16) get_key_state (39); // QWERTY S
			case Device.JoypadId.Y:
				return (int16) get_key_state (38); // QWERTY A
			case Device.JoypadId.SELECT:
				return (int16) get_key_state (22); // Backspace
			case Device.JoypadId.START:
				return (int16) get_key_state (36); // Enter
			case Device.JoypadId.UP:
				return (int16) get_key_state (111); // Up arrow
			case Device.JoypadId.DOWN:
				return (int16) get_key_state (116); // Down arrow
			case Device.JoypadId.LEFT:
				return (int16) get_key_state (113); // Left arrow
			case Device.JoypadId.RIGHT:
				return (int16) get_key_state (114); // Right arrow
			case Device.JoypadId.A:
				return (int16) get_key_state (40); // QWERTY D
			case Device.JoypadId.X:
				return (int16) get_key_state (25); // QWERTY W
			case Device.JoypadId.L:
				return (int16) get_key_state (24); // QWERTY Q
			case Device.JoypadId.R:
				return (int16) get_key_state (26); // QWERTY E
			case Device.JoypadId.L2:
				return (int16) get_key_state (52); // QWERTY Z
			case Device.JoypadId.R2:
				return (int16) get_key_state (54); // QWERTY C
			case Device.JoypadId.L3:
				return (int16) get_key_state (10); // QWERTY 1
			case Device.JoypadId.R3:
				return (int16) get_key_state (12); // 3
			default:
				return 0;
		}
	}
	
	public Device.Type  get_device_type () {
		return Device.Type.KEYBOARD;
	}
	
	public Device.Type[] get_device_capabilities () {
		return { Device.Type.KEYBOARD, Device.Type.JOYPAD };
	}
}

}

