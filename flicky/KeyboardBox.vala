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

public class KeyboardBox : EventBox, ControllerDevice {
	private HashTable<uint?, bool?> key_state;
	private Keymap keymap;
	
	private uint16[] joypad_keys;
	
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
		
		// Set the default keys for the joypad.
		
		joypad_keys = new uint16[16];
		set_joypad_key (Retro.Device.JoypadId.B,      39); // QWERTY S
		set_joypad_key (Retro.Device.JoypadId.Y,      38); // QWERTY A
		set_joypad_key (Retro.Device.JoypadId.SELECT, 22); // Backspace
		set_joypad_key (Retro.Device.JoypadId.START,  36); // Enter
		set_joypad_key (Retro.Device.JoypadId.UP,    111); // Up arrow
		set_joypad_key (Retro.Device.JoypadId.DOWN,  116); // Down arrow
		set_joypad_key (Retro.Device.JoypadId.LEFT,  113); // Left arrow
		set_joypad_key (Retro.Device.JoypadId.RIGHT, 114); // Right arrow
		set_joypad_key (Retro.Device.JoypadId.A,      40); // QWERTY D
		set_joypad_key (Retro.Device.JoypadId.X,      25); // QWERTY W
		set_joypad_key (Retro.Device.JoypadId.L,      24); // QWERTY Q
		set_joypad_key (Retro.Device.JoypadId.R,      26); // QWERTY E
		set_joypad_key (Retro.Device.JoypadId.L2,     52); // QWERTY Z
		set_joypad_key (Retro.Device.JoypadId.R2,     54); // QWERTY C
		set_joypad_key (Retro.Device.JoypadId.L3,     10); // QWERTY 1
		set_joypad_key (Retro.Device.JoypadId.R3,     12); // QWERTY 3
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
	
	private bool get_key_state (uint16 hardware_keycode) {
		if (key_state.contains (hardware_keycode)) {
			return key_state.lookup ((uint) hardware_keycode);
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
	
	public int16 get_input_state (Retro.Device.Type device, uint index, uint id) {
		switch (device) {
			case Retro.Device.Type.KEYBOARD:
				return get_keyboard_state (index, id);
			case Retro.Device.Type.JOYPAD:
				return get_joypad_state ((Retro.Device.JoypadId) id);
			default:
				return 0;
		}
	}
	
	private int16 get_keyboard_state (uint index, uint id) {
		return 0;
	}
	
	private int16 get_joypad_state (Retro.Device.JoypadId id) {
		return get_key_state (get_joypad_key (id)) ? int16.MAX : 0;
	}
	
	public uint64 get_device_capabilities () {
		return (1 << Retro.Device.Type.KEYBOARD) | (1 << Retro.Device.Type.JOYPAD);
	}
	
	public void set_joypad_key (Retro.Device.JoypadId id, uint16 hardware_keycode) {
		joypad_keys[id] = hardware_keycode;
	}
	
	public uint16 get_joypad_key (Retro.Device.JoypadId id) {
		return joypad_keys[id];
	}
}

}

