/* SystemAvInfo.vala  Get AV informations from a Core.
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

public struct GameGeometry {
	public uint  base_width;
	public uint  base_height;
	public uint  max_width;
	public uint  max_height;
	public float aspect_ratio;
}

public struct SystemTiming {
	public double fps;
	public double sample_rate;
}

public struct SystemAvInfo {
	public GameGeometry geometry;
	public SystemTiming timing;
}

}

