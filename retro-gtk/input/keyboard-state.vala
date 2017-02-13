// This file is part of RetroGtk. License: GPLv3

public class Retro.KeyboardState : Object {
	public Gtk.Widget widget { get; construct; }

	private HashTable<uint?, bool?> key_state;
	private Gdk.Keymap keymap;

	public signal void key_state_changed ();

	public KeyboardState (Gtk.Widget widget) {
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

		keymap = Gdk.Keymap.get_default ();
	}

	private bool on_key_press_event (Gtk.Widget source, Gdk.EventKey event) {
		if (key_state.contains ((uint) event.hardware_keycode)) {
			key_state.replace ((uint) event.hardware_keycode, true);
		}
		else  {
			key_state.insert ((uint) event.hardware_keycode, true);
		}

		key_state_changed ();

		return false;
	}

	private bool on_key_release_event (Gtk.Widget source, Gdk.EventKey event) {
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
