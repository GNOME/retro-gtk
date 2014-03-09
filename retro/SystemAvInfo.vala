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
 * Stores video geometry informations.
 */
public struct GameGeometry {
	/**
	 * Nominal video width of game.
	 */
	public uint base_width;
	
	/**
	 * Nominal video height of game.
	 */
	public uint base_height;
	
	/**
	 * Maximum possible width of game.
	 */
	public uint max_width;
	
	/**
	 * Maximum possible height of game.
	 */
	public uint max_height;
	
	/**
	 * Nominal aspect ratio of game.
	 * 
	 * If aspect_ratio is <= 0.0, an aspect ratio of base_width / base_height is assumed.
	 * 
	 * A frontend could override this setting if desired.
	 */
	public float aspect_ratio;
}

/**
 * Stores video and audio timing informations.
 */
public struct SystemTiming {
	/**
	 * FPS of video content.
	 */
	public double fps;
	
	/**
	 * Sampling rate of audio.
	 */
	public double sample_rate;
}

/**
 * Stores video geometry and AV timing informations.
 */
public struct SystemAvInfo {
	/**
	 * Video geometry informations.
	 */
	public GameGeometry geometry;
	
	/**
	 * Video and audio timing informations.
	 */
	public SystemTiming timing;
}

}

