// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * An interface to run a {@link Core} in a loop.
 */
public interface Loop : Object {
	/**
	 * The core to run.
	 */
	public abstract Core core { get; construct set; }

	/**
	 * The speed factor at which the {@link core} should run.
	 */
	public abstract double speed_rate { get; construct set; }


	/**
	 * Starts running the {@link core}.
	 */
	public abstract void start ();

	/**
	 * Resets the {@link core}.
	 */
	public abstract void reset ();

	/**
	 * Stops running the {@link core}.
	 */
	public abstract void stop ();
}

}

