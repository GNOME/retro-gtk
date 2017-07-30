// This file is part of retro-gtk. License: GPL-3.0+.

public class Retro.CoreView : Gtk.EventBox {
	private Core core;
	private CairoDisplay display;

	internal HashTable<uint?, bool?> key_state;

	construct {
		can_focus = true;

		display = new CairoDisplay ();
		display.visible = true;
		display.can_focus = false;
		add (display);

		key_state = new HashTable<uint?, bool?> (int_hash, int_equal);

		key_press_event.connect (on_key_press_event);
		key_release_event.connect (on_key_release_event);
	}

	public void set_core (Core? core) {
		if (this.core != null)
			display.set_core (null);

		this.core = core;

		if (core != null)
			display.set_core (core);
	}

	public extern InputDevice as_input_device (DeviceType device_type);
	internal extern int16 get_input_state (DeviceType device, uint index, uint id);
	internal extern uint64 get_device_capabilities ();
	internal extern bool on_key_press_event (Gtk.Widget source, Gdk.EventKey event);
	internal extern bool on_key_release_event (Gtk.Widget source, Gdk.EventKey event);
}
