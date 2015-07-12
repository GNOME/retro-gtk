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

