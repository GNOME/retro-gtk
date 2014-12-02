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

using Retro;

namespace RetroGtk {

public class ThreadedLoop : Object, Loop {
	public Core core { get; construct; }

	private Thread<bool> thread;
	private bool run;

	public ThreadedLoop (Core core) {
		Object (core: core);
	}

	construct {
		run = false;
		thread = null;
	}

	~ThreadedLoop () {
		stop ();
	}

	public void start () {
		run = true;
		thread = new Thread<bool> (null, loop);
	}

	public void reset () {
		if (core != null) {
			core.reset ();
		}
	}

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
			int64 delta = (int64) (1000000 / core.av_info.fps);

			// Sleep for the delta - the time spent running
			int64 sleep = delta - (get_monotonic_time () - start_time);
			if (sleep > 0)
				Thread.usleep ((ulong) sleep);
		}

		return true;
	}
}

}

