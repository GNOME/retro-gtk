// This file is part of RetroGtk. License: GPLv3

private class Retro.MotionParser : Object {
	private Gdk.Screen screen;

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
	public bool parse_event (Gdk.EventMotion event, out int x_movement, out int y_movement) {
		var device = event.device;

		Gdk.Screen s;
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

public class Retro.Mouse : Object, InputDevice {
	public Gtk.Widget widget { get; construct; }

	public bool parse {
		get { return parser.grab_pointer; }
		set { parser.grab_pointer = value; }
	}

	private HashTable<uint?, bool?> button_state;
	private MotionParser parser;
	private int16 x_delta;
	private int16 y_delta;

	private ulong ungrab_id;

	public Mouse (Gtk.Widget widget) {
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
			if (e.keyval == Gdk.Key.Escape && (bool) (e.state & Gdk.ModifierType.CONTROL_MASK))
				ungrab (e.time);
			return false;
		});

		widget.button_press_event.connect (on_button_press_event);

		widget.button_release_event.connect (on_button_release_event);

		button_state = new HashTable<uint?, bool?> (int_hash, int_equal);
	}

	public void poll () {}

	public int16 get_input_state (DeviceType device, uint index, uint id) {
		if (device != DeviceType.MOUSE) return 0;

		switch ((MouseId) id) {
			case MouseId.X:
				int16 result = x_delta;
				x_delta = 0;
				return result;
			case MouseId.Y:
				int16 result = y_delta;
				y_delta = 0;
				return result;
			case MouseId.LEFT:
				return get_button_state (1) ? int16.MAX : 0;
			case MouseId.RIGHT:
				return get_button_state (3) ? int16.MAX : 0;
			default:
				return 0;
		}
	}

	public DeviceType get_device_type () {
		return DeviceType.MOUSE;
	}

	public uint64 get_device_capabilities () {
		return 1 << DeviceType.MOUSE;
	}

	private bool on_button_press_event (Gtk.Widget source, Gdk.EventButton event) {
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

	private bool on_button_release_event (Gtk.Widget source, Gdk.EventButton event) {
		if (button_state.contains (event.button)) {
			button_state.replace (event.button, false);
		}

		return false;
	}

	/**
	 * Update the pointer's position
	 */
	private bool on_motion_notify_event (Gtk.Widget source, Gdk.EventMotion event) {
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
	private void grab (Gdk.Device device, Gdk.Window window, uint32 time) {
		// Save the pointer's position
		Gdk.Screen screen;
		int x, y;
		device.get_position (out screen, out x, out y);

		// Grab the device
		parse = true;
		var cursor = new Gdk.Cursor (Gdk.CursorType.BLANK_CURSOR);
		device.grab (window, Gdk.GrabOwnership.NONE, false, Gdk.EventMask.ALL_EVENTS_MASK, cursor, time);

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
