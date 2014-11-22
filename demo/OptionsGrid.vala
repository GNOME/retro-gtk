/* Window.vala  A simple display.
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
using RetroGtk;

public class OptionsGrid : Gtk.Grid {
	private OptionsHandler options;

	public OptionsGrid (OptionsHandler options) {
		this.options = options;

		margin_start = 12;
		margin_end = 12;
		margin_top = 6;
		margin_bottom = 6;
		column_spacing = 12;
		row_spacing = 6;

		int i = 0;
		foreach (var key in options.get_keys ()) {
			add_option (key, i);
			i++;
		}
	}

	private void add_option (string key, int row) {
		var description = options.lookup_description (key);
		var values = options.lookup_values (key);
		var current = options.lookup (key);

		var list_store = new Gtk.ListStore (1, typeof (string));
		Gtk.TreeIter? iter = null;

		int current_i = 0;

		int i = 0;
		foreach (var option in values) {
			if (option == current) current_i = i;

			list_store.append (out iter);
			list_store.set (iter, 0, option);
			i++;
		}

		var box = new Gtk.ComboBox.with_model (list_store);

		var renderer = new Gtk.CellRendererText ();
		box.pack_start (renderer, true);
		box.add_attribute (renderer, "text", 0);

		box.active = 0;
		box.set_active (current_i);

		box.changed.connect (() => {
			Value val;

			box.get_active_iter (out iter);
			list_store.get_value (iter, 0, out val);

			options.set_option (key, (string) val);
		});


		attach (new Gtk.Label (description), 0, row, 1, 1);
		attach (box, 1, row, 1, 1);
	}
}

