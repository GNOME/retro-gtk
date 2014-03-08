/* Flicky  Building blocks for a Retro frontend.
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

namespace Flicky {

/**
 * Represent an object that can be run.
 * 
 * Such an object could be run with a {@link Flicky.Runner}.
 */
public interface Runnable : Object {
	/**
	 * Resets.
	 */
	public abstract void reset ();
	
	/**
	 * Run one frame.
	 */
	public abstract void run ();
	
	/**
	 * Gets the fps.
	 * 
	 * Gets the number of frames that should be run in a second.
	 * 
	 * @return [the number of frames per second]
	 */
	public abstract double get_frames_per_second ();
}

}

