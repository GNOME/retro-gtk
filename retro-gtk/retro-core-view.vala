// This file is part of retro-gtk. License: GPL-3.0+.

public class Retro.CoreView : Gtk.EventBox {
	private Core core;
	private CairoDisplay display;
	private InputDeviceManager input;
	private Keyboard keyboard;
	private Mouse mouse;

	construct {
		can_focus = true;

		display = new CairoDisplay ();
		display.visible = true;
		display.can_focus = false;
		add (display);

		input = new InputDeviceManager ();
		keyboard = new Keyboard (this);
		mouse = new Mouse (this);

		input.set_keyboard (keyboard);
		input.set_controller_device (0, mouse);
	}

	public void set_core (Core? core) {
		if (this.core != null) {
			display.set_core (null);
			core.input_interface = null;
		}

		this.core = core;

		if (core != null) {
			display.set_core (core);
			core.input_interface = input;
		}
	}
}
