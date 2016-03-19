// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * A string key-value pair.
 *
 * Used by a {@link Core} in its environment callback.
 */
public struct Variable {
	/**
	 * The key of the pair.
	 */
	public string? key;

	/**
	 * The value of the pair.
	 *
	 * If the key is null while retrieving a variable,
	 * value should be the complete environment variables formatted as
	 * key-value pairs separated by semicolons, like so:
	 * "key1=value1;key2=value2;...".
	 *
	 * If the key exists while retrieving a variable, value should be the
	 * value associated to the key.
	 *
	 * If the key doesn't exist while retrieving a variable, the value should
	 * be null.
	 */
	public string? value;
}

/**
 * An interface to handle a {@link Core}'s variables.
 */
public interface Variables : Object {
	/**
	 * The core whose variables will be handled.
	 */
	public abstract Core core { get; set; }

	/**
	 * Gets the value for a variable.
	 *
	 * @param key the key of the variable whose value should be returned
	 * @return the value of the variable or //null// if the key doesn't exist
	 */
	public abstract string? get_variable (string key);

	/**
	 * Sets a set of variables to handle.
	 *
	 * @param variables the variables to handle
	 */
	public abstract void set_variable (Variable[] variables);

	/**
	 * Gets whether a value have been updated since last call to
	 * {@link get_variable}.
	 *
	 * @return //true// if a variable have been updated, //false// otherwise
	 */
	public abstract bool get_variable_update ();
}

}

