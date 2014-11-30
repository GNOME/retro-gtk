/* Copyright (C) 2014  Adrien Plazas
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

private class MotionParser : Object {
	private Screen screen;

	private bool _grab_pointer;
	public bool grab_pointer {
		get { return _grab_pointer; }
		set {
			_grab_pointer = value;
			screen = null;
		}
	}

	private int x_last;
	private int y_last;

	/*
	 * Return wether a movement happened or not
	 */
	public bool parse_event (EventMotion event, out int x_movement, out int y_movement) {
		var device = event.device;

		Screen s;
		int x, y;
		device.get_position (out s, out x, out y);

		int x_center = s.get_width ()/2;
		int y_center = s.get_height ()/2;

		if (s != screen) {
			screen = s;
			x_movement = y_movement = 0;
			// The pointer may be warped to set it correctly
			if (grab_pointer) {
				device.warp (s, x_center, y_center);
				x_last = x_center;
				y_last = y_center;
			}
			else {
				x_last = x;
				y_last = y;
			}
			return false;
		}

		screen = s;

		x_movement = x - x_last;
		y_movement = y - y_last;

		if (x_movement == 0 && y_movement == 0) return false;

		// Motion hapened: the pointer may be warped
		if (grab_pointer) {
			device.warp (s, x_center, y_center);
			x_last = x_center;
			y_last = y_center;
		}
		else {
			x_last = x;
			y_last = y;
		}
		return true;
	}
}

public class Mouse : Object, ControllerDevice {
	public Widget widget { get; construct; }

	public bool parse {
		get { return parser.grab_pointer; }
		set { parser.grab_pointer = value; }
	}

	private HashTable<uint?, bool?> button_state;
	private MotionParser parser;
	private int16 x_delta;
	private int16 y_delta;

	private ulong ungrab_id;

	public Mouse (Widget widget) {
		Object (widget: widget);
	}

	construct {
		parser = new MotionParser ();

		widget.button_press_event.connect (on_button_press_event);
		widget.motion_notify_event.connect (on_motion_notify_event);

		// Ungrab on focus out event
		widget.focus_out_event.connect ((w, e) => {
			ungrab (0);
			return false;
		});

		// Ungrab on press of Escape
		widget.key_press_event.connect ((w, e) => {
			if (e.keyval == Key.Escape && (bool) (e.state & ModifierType.CONTROL_MASK))
				ungrab (e.time);
			return false;
		});

		widget.button_press_event.connect (on_button_press_event);

		widget.button_release_event.connect (on_button_release_event);

		button_state = new HashTable<uint?, bool?> (int_hash, int_equal);
	}

	public void poll () {}

	public int16 get_input_state (Retro.DeviceType device, uint index, uint id) {
		if (device != Retro.DeviceType.MOUSE) return 0;

		switch ((Retro.MouseId) id) {
			case Retro.MouseId.X:
				int16 result = x_delta;
				x_delta = 0;
				return result;
			case Retro.MouseId.Y:
				int16 result = y_delta;
				y_delta = 0;
				return result;
			case Retro.MouseId.LEFT:
				return get_button_state (1) ? int16.MAX : 0;
			case Retro.MouseId.RIGHT:
				return get_button_state (3) ? int16.MAX : 0;
			default:
				return 0;
		}
	}

	public uint64 get_device_capabilities () {
		return 1 << Retro.DeviceType.MOUSE;
	}

	private bool on_button_press_event (Widget source, EventButton event) {
		if (!parse) {
			grab (event.device, event.window, event.time);
			return false;
		}

		if (button_state.contains (event.button)) {
			button_state.replace (event.button, true);
		}
		else  {
			button_state.insert (event.button, true);
		}

		return false;
	}

	private bool on_button_release_event (Widget source, EventButton event) {
		if (button_state.contains (event.button)) {
			button_state.replace (event.button, false);
		}

		return false;
	}

	/**
	 * Update the pointer's position
	 */
	private bool on_motion_notify_event (Widget source, EventMotion event) {
		if (!parse) return false;

		int x, y;
		if (parser.parse_event (event, out x, out y)) {
			x_delta += (int16) x;
			y_delta += (int16) y;
		}

		return false;
	}

	public bool get_button_state (uint button) {
		if (button_state.contains (button)) {
			return button_state.lookup (button);
		}

		return false;
	}

	/**
	 * Release the pointer
	 */
	private signal void ungrab (uint32 time);

	/**
	 * Grab the poiner
	 */
	private void grab (Device device, Gdk.Window window, uint32 time) {
		// Save the pointer's position
		Screen screen;
		int x, y;
		device.get_position (out screen, out x, out y);

		// Grab the device
		parse = true;
		var cursor = new Cursor (CursorType.BLANK_CURSOR);
		device.grab (window, GrabOwnership.NONE, false, EventMask.ALL_EVENTS_MASK, cursor, time);

		// Ungrab the device when asked
		ungrab_id = ungrab.connect ((time) => {
			if (parse) {
				parse = false;
				device.ungrab (time);

				// Restore the pointer's position
				device.warp (screen, x, y);
				disconnect (ungrab_id);
			}
		});
	}
}

}
