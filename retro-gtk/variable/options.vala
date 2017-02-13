// This file is part of retro-gtk. License: GPLv3

namespace Retro {

private class Option : Object {
	public string key;
	public string description;
	public string[] values;

	private string _current;
	public string current {
		get { return _current; }
		set {
			if (value in values)
				_current = value.dup ();
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

/**
 * Manage the options of a {@link Core}.
 *
 * The options are defined by the core's variables.
 */
private class Options: Object, Variables {
	/**
	 * The core whose variables will be handled.
	 */
	public Core _core;
	public Core core {
		get { return _core; }
		set {
			if (_core == value) return;

			_core = value;

			if (_core != null && _core.variables_interface != this)
				_core.variables_interface = this;
		}
	}

	private HashTable<string,Option> table;

	/**
	 * Notifies the change of an option's current value.
	 *
	 * @param key the key of the option whose value changed
	 */
	public signal void value_changed (string key);

	private bool update;

	construct {
		update = false;
		table = new HashTable<string,Option> (str_hash, str_equal);
	}

	private void insert_multiple (Variable[] variables) {
		foreach (var variable in variables) {
			insert (variable.key, variable);
		}
	}

	private void insert (owned string key, owned Variable value) {
		table.insert (key, new Option (value));
		value_changed (key);
		update = true;
	}

	/**
	 * Sets the current value of an option.
	 *
	 * @param key the key of the option
	 * @param value the value of the option
	 */
	public void set_option (owned string key, owned string value) {
		table[key].current = value;
		value_changed (key);
		update = true;
	}

	public new void @set (owned string key, owned string value) {
		set_option (key, value);
	}

	/**
	 * Checks if an option exists for a key.
	 *
	 * @param key the key of the option
	 * @return //true// if the option exists, //false// otherwise
	 */
	public bool contains (string key) {
		return table.contains (key);
	}

	/**
	 * Gets the current value of an option.
	 *
	 * @param key the key of the option
	 */
	public unowned string lookup (string key) {
		return table[key].current;
	}

	/**
	 * Gets the description of an option.
	 *
	 * @param key the key of the option
	 */
	public unowned string lookup_description (string key) {
		return table[key].description;
	}

	/**
	 * Gets the possible values of an option.
	 *
	 * @param key the key of the option
	 */
	public unowned string[] lookup_values (string key) {
		return table[key].values;
	}

	public new unowned string @get (string key) {
		return lookup (key);
	}

	/**
	 * Gets the keys of the available options.
	 *
	 * @return the keys of the available options
	 */
	public List<unowned string> get_keys () {
		return table.get_keys ();
	}

	/**
	 * Iterates through the options.
	 *
	 * @param func the function to apply to each option
	 */
	public void for_each (HFunc<string,string> func) {
		foreach (var key in get_keys ()) {
			func (key, this[key]);
		}
	}

	public void @foreach (HFunc<string,string> func) {
		for_each (func);
	}

	/**
	 * Gets the number of options offered by the {@link core}
	 *
	 * @return the number of options offered by the {@link core}
	 */
	public uint size () {
		return table.size ();
	}

	/**
	 * Gets the value for a variable.
	 *
	 * @param key the key of the variable whose value should be returned
	 * @return the value of the variable or //null// if the key doesn't exist
	 */
	public string? get_variable (string key) {
		if (!contains (key))
			return null;

		return lookup (key);
	}

	/**
	 * Sets a set of variables to handle.
	 *
	 * @param variables the variables to handle
	 */
	public void set_variable (Variable[] variables) {
		insert_multiple (variables);
	}

	/**
	 * Gets whether a value have been updated since last call to
	 * {@link get_variable}.
	 *
	 * @return //true// if a variable have been updated, //false// otherwise
	 */
	public bool get_variable_update () {
		if (update) {
			update = false;
			return true;
		}
		return false;
	}
}

}
