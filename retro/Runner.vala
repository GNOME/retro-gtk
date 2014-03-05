/* Runner.vala  Run a Runnable object.
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

public class Runner : Object {
	private Runnable runnable;
	private uint? loop;
	
	public double speed_rate { construct set; get; default = 1; }
	
	public Runner (Runnable runnable) {
		this.runnable = runnable;
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
		if (speed_rate > 0) {
			var ips = runnable.get_iterations_per_second ();
			loop = Timeout.add ((uint) (1000 / (ips * speed_rate)), run);
		}
	}
	
	public void stop () {
		if (loop != null) {
			Source.remove (loop);
		}
		
		loop = null;
	}
	
	private bool run () {
		if (runnable != null && loop != null) {
			runnable.run ();
			
			return true;
		}
		
		return false;
	}
}

}

