/* RetroGtk  Building blocks for a Retro frontend.
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

using Retro;

namespace RetroGtk {

public class FileStreamLogger: Object, Retro.Log {
	private static const string default_color = "\033[39m";

	public unowned FileStream stream { set; get; }

	public FileStreamLogger (FileStream stream = GLib.stderr) {
		Object (stream: stream);
	}

	public bool log (LogLevel level, string message) {
		stream.printf ("%s%s%s", get_color (level), message, default_color);
		return true;
	}

	private string get_color (LogLevel level) {
		switch (level) {
			case LogLevel.DEBUG:
				// Green
				return "\033[32m";
			case LogLevel.WARN:
				// Yellow
				return "\033[33m";
			case LogLevel.ERROR:
				// Red
				return "\033[31m";
			default:
				return default_color;
		}
	}
}

}

