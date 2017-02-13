// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * Runs a {@link Core} in a GLib or Gtk main loop.
 *
 * It may cause problems if the core takes more time than expected to run a
 * frame.
 */
public class MainLoop : Object {
	/**
	 * The core to run.
	 */
	public Core core { get; construct set; }

	/**
	 * The speed factor at which the {@link core} should run.
	 */
	public double speed_rate { get; construct set; default = 1; }

	private uint? loop;

	/**
	 * Creates a MainLoop to run a {@link Core}.
	 *
	 * @param core the {@link Core} tu run
	 */
	public MainLoop (Core core) {
		Object (core: core);
	}

	construct {
		loop = null;

		notify.connect ((src, param) => {
			switch (param.get_name ()) {
				case "speed_rate":
					if (loop != null) {
						stop ();
						start ();
					}
					break;
			}
		});

		core.notify["frames-per-second"].connect (on_frames_per_second_changed);
	}

	~MainLoop () {
		stop ();
	}

	/**
	 * Starts running the {@link core}.
	 */
	public void start () {
		// TODO What if FPSs <= 0?

		if (loop == null && speed_rate > 0) {
			loop = Timeout.add ((uint) (1000 / (core.frames_per_second * speed_rate)), run);
		}
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
		if (loop != null) {
			Source.remove (loop);
			loop = null;
		}
	}

	private bool run () {
		if (core != null && loop != null) {
			core.run ();

			return true;
		}

		return false;
	}

	private void on_frames_per_second_changed () {
		if (loop == null)
			return;

		stop ();
		start ();
	}
}

}

