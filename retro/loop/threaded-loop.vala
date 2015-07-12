/* Copyright (C) 2014  Adrien Plazas
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

