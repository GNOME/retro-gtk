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
	
	public OptionsDialog (VariableHandler variables) {
		this.variables = variables;
		
		foreach (var key in variables.get_keys ()) {
			add_option (key);
		}
	}
	
	private void add_option (string key) {
		var description = variables.lookup_description (key);
		var values = variables.lookup_values (key);
		var current = variables.lookup (key);
		
		var line = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
		line.pack_start (new Gtk.Label (description));
		
		var list_store = new Gtk.ListStore (1, typeof (string));
		Gtk.TreeIter? iter = null;
		
		foreach (var option in values) {
			list_store.append (out iter);
			list_store.set (iter, 0, option);
		}
		
		Gtk.ComboBox box = new Gtk.ComboBox.with_model (list_store);
		line.pack_start (box);
		
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
		
		get_content_area ().pack_start (line);
	}
}

}

