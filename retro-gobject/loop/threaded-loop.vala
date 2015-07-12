// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * Runs a {@link Core} in a dedicated threaded loop.
 */
public class ThreadedLoop : Object, Loop {
	/**
	 * The core to run.
	 */
	public Core core { get; construct set; }

	/**
	 * The speed factor at which the {@link core} should run.
	 */
	public double speed_rate { get; construct set; default = 1; }

	private Thread<bool> thread;
	private bool run;

	/**
	 * Creates a ThreadedLoop to run a {@link Core}.
	 *
	 * @param core the {@link Core} to run
	 */
	public ThreadedLoop (Core core) {
		Object (core: core);
	}

	construct {
		notify["core"].connect (stop);

		run = false;
		thread = null;
	}

	~ThreadedLoop () {
		stop ();
	}

	/**
	 * Starts running the {@link core}.
	 */
	public void start () {
		run = true;
		thread = new Thread<bool> (null, loop);
	}

	/**
	 * Resets the {@link core}.
	 */
	public void reset () {
		if (core != null) {
			core.reset ();
		}
	}

	/**
	 * Stops running the {@link core}.
	 */
	public void stop () {
		run = false;
		if (thread != null) {
			thread.join ();
			thread = null;
		}
	}

	private bool loop () {
		int64 start_time;

		while (run) {
			// Get the start time
			start_time = get_monotonic_time ();

			// Run the core
			core.run ();

			// Get the core's frame delta
			int64 delta = (int64) (1000000 / (core.av_info.fps * speed_rate));

			// Sleep for the delta - the time spent running
			int64 sleep = delta - (get_monotonic_time () - start_time);
			if (sleep > 0)
				Thread.usleep ((ulong) sleep);
		}

		return true;
	}
}

}

