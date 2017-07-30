// This file is part of retro-gtk. License: GPL-3.0+.

public class Retro.CoreView : Gtk.EventBox {
	private Core core;
	private CairoDisplay display;

	construct {
		can_focus = true;

		display = new CairoDisplay ();
		display.visible = true;
		display.can_focus = false;
		add (display);
	}

	public void set_core (Core? core) {
		if (this.core != null)
			display.set_core (null);

		this.core = core;

		if (core != null)
			display.set_core (core);
	}
}
