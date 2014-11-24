/* Demo.vala  A simple demo.
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

using Retro;
using Gtk;

class Demo : Object {
	private Window window;

	public void run () {
		Gtk.Settings.get_default().set("gtk-application-prefer-dark-theme", true);

		window = new Window (get_libretro_modules ());
		window.show ();
		window.destroy.connect (() => { Gtk.main_quit(); } );

		var gamepad_window = new GamepadViewWindow ();
		gamepad_window.show ();
	}

	private string[] get_libretro_modules () {
		string[] modules = {};

		try {
			var dirpath = @"$PREFIX/lib/libretro";
			var directory = File.new_for_path (dirpath);
			var enumerator = directory.enumerate_children (FileAttribute.STANDARD_NAME, 0);

			FileInfo file_info;
			while ((file_info = enumerator.next_file ()) != null) {
				var name = file_info.get_name ();
				if (/libretro-.+\.so/.match (name))
					modules += @"$dirpath/$name";
			}

		} catch (Error e) {
			stderr.printf ("Error: %s\n", e.message);
		}

		return modules;
	}

	public static int main (string[] argv) {
		Gtk.init (ref argv);
		var clutter_error = Clutter.init (ref argv);
		if (clutter_error != Clutter.InitError.SUCCESS) {
			stderr.printf ("Clutter init error: %s\n", clutter_error.to_string ());
			return clutter_error;
		}

		var d = new Demo ();
		d.run ();

		Gtk.main ();

		return 0;
}

}

