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

namespace Retro {

class OptionsDialog : Gtk.Dialog {
	private VariableHandler variables;
	private Gtk.Grid grid;
	
	public OptionsDialog (VariableHandler variables) {
		this.variables = variables;
		
		grid = new Gtk.Grid ();
		
		int i = 0;
		foreach (var key in variables.get_keys ()) {
			add_option (key, i);
			i++;
		}
		
		get_content_area ().pack_start (grid);
	}
	
	private void add_option (string key, int row) {
		var description = variables.lookup_description (key);
		var values = variables.lookup_values (key);
		var current = variables.lookup (key);
		
		var list_store = new Gtk.ListStore (1, typeof (string));
		Gtk.TreeIter? iter = null;
		
		foreach (var option in values) {
			list_store.append (out iter);
			list_store.set (iter, 0, option);
		}
		
		Gtk.ComboBox box = new Gtk.ComboBox.with_model (list_store);
		
		Gtk.CellRendererText renderer = new Gtk.CellRendererText ();
		box.pack_start (renderer, true);
		box.add_attribute (renderer, "text", 0);
		box.active = 0;
		
		box.changed.connect (() => {
			Value val;
			
			box.get_active_iter (out iter);
			list_store.get_value (iter, 0, out val);
			
			variables.set_option (key, (string) val);
		});
		
		// TODO set the combo's defaut value to the current one of the variable handler
		
		grid.attach (new Gtk.Label (description), 0, row, 1, 1);
		grid.attach (box, 1, row, 1, 1);
	}
}

}

