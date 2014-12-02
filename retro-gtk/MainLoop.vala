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

public class MainLoop : Object, Loop {
	public Core core { get; construct; }
	private uint? loop;

	public double speed_rate { construct set; get; default = 1; }

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
	}

	~Runner () {
		stop ();
	}

	public void start () {
		var info = core.av_info;
		var fps = info == null ? info.fps : 60.0;
		if (loop == null && speed_rate > 0) {
			loop = Timeout.add ((uint) (1000 / (fps * speed_rate)), run);
		}
	}

	public void reset () {
		if (core != null) {
			core.reset ();
		}
	}

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
}

}

