// This file is part of retro-gtk. License: GPL-3.0+.

public class Retro.Keyboard : Object {
	public Gtk.Widget widget { get; construct; }

	public signal bool key_event (Gdk.EventKey event);

	public Keyboard (Gtk.Widget widget) {
		Object (widget: widget);
	}

	construct {
		widget.key_press_event.connect ((w, e) => key_event (e));
		widget.key_release_event.connect ((w, e) => key_event (e));
	}
}
