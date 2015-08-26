// This file is part of RetroGtk. License: GPLv3

namespace RetroGtk {

/**
 * Initialize the frameworks used by Retrotk
 *
 * It initializes te following frameworks
 *  * Gtk
 *  * Clutter
 */
[Deprecated (since = "0.2")]
public void init (ref unowned string[]? argv) {
	Gtk.init (ref argv);
	var clutter_error = Clutter.init (ref argv);
	if (clutter_error != Clutter.InitError.SUCCESS) {
		stderr.printf ("Clutter init error: %s\n", clutter_error.to_string ());
	}
}

}

