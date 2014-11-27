/* RetroGtk  Building blocks for a Retro frontend.
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

namespace RetroGtk {

public class Option : Object {
	public string key;
	public string description;
	public string[] values;

	private string _current;
	public string current {
		set {
			if (value in values) {
				_current = value.dup ();
			}
		}
		get {
			return _current;
		}
	}

	public Option (Variable variable) {
		var desc_and_vals = variable.value.split ("; ", 2);

		key = variable.key;
		description = desc_and_vals[0];
		values = desc_and_vals[1].split ("|");
		current = values[0];
	}
}

public class OptionsHandler: Object, VariablesHandler {
	public weak Core _core;
	public weak Core core {
		get { return _core; }
		set {
			if (_core == value) return;

			_core = value;

			if (_core != null && _core.variables_handler != this)
				_core.variables_handler = this;
		}
	}

	private HashTable<string,Option> table;

	public signal void value_changed (string key);
	private bool update;

	construct {
		update = false;
		table = new HashTable<string,Option> (str_hash, str_equal);
	}

	public void insert_multiple (Variable[] variables) {
		foreach (var variable in variables) {
			insert (variable.key, variable);
		}
	}

	public void insert (owned string key, owned Variable value) {
		table.insert (key, new Option (value));
		value_changed (key);
		update = true;
	}

	public void set_option (owned string key, owned string value) {
		table[key].current = value;
		value_changed (key);
		update = true;
	}

	public bool contains (string key) {
		return table.contains (key);
	}

	public unowned string lookup (string key) {
		return table[key].current;
	}

	public unowned string lookup_description (string key) {
		return table[key].description;
	}

	public unowned string[] lookup_values (string key) {
		return table[key].values;
	}

	public new unowned string @get (string key) {
		return lookup (key);
	}

	public new void @set (owned string key, owned string value) {
		set_option (key, value);
	}

	public List<unowned string> get_keys () {
		return table.get_keys ();
	}

	public List<unowned string> get_values () {
		var list = new List<string> ();

		foreach (var key in get_keys ()) {
			list.append (this[key]);
		}

		return list;
	}

	public void for_each (HFunc<string,string> func) {
		foreach (var key in get_keys ()) {
			func (key, this[key]);
		}
	}

	public void @foreach (HFunc<string,string> func) {
		for_each (func);
	}

	public uint size () {
		return table.size ();
	}

	public string? get_variable (string key) {
		return lookup (key);
	}

	public void set_variable (Variable[] variables) {
		insert_multiple (variables);
	}

	public bool get_variable_update () {
		if (update) {
			update = false;
			return true;
		}
		return false;
	}
}

}
