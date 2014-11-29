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

public interface VideoInterface : Object {
	public abstract weak Core core { get; set; }

	public abstract void render (uint8[] data, uint width, uint height, size_t pitch);

	public abstract Rotation rotation { get; set; }
	public abstract bool overscan { get; set; }
	public abstract bool can_dupe { get; set; }
	public abstract PixelFormat pixel_format { get; set; }
}

}
