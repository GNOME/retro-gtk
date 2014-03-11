/* Retro  GObject libretro wrapper.
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

/**
 * An interface for logging.
 */
public interface Log: Object {
	/**
	 * The level of importance of a message.
	 */
	public enum Level {
		DEBUG = 0,
		INFO,
		WARN,
		ERROR
	}
	
	/**
	 * Logs a message.
	 * 
	 * @param level the importance of the message
	 * @param message the message to log
	 * @return true if the message have been logged, false otherwise
	 */
	public abstract bool log (Level level, string message);
}

}

