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
 * Stores video geometry informations.
 */
internal struct GameGeometry {
	public uint base_width;
	public uint base_height;
	public uint max_width;
	public uint max_height;
	public float aspect_ratio;
}

/**
 * Stores video and audio timing informations.
 */
internal struct SystemTiming {
	public double fps;
	public double sample_rate;
}

/**
 * Stores video geometry and AV timing informations.
 */
internal struct SystemAvInfo {
	public GameGeometry geometry;
	public SystemTiming timing;
}

/**
 * Stores video geometry and AV timing informations.
 */
public class AvInfo : Object {
	/**
	 * The nominal video width of the game.
	 */
	public uint base_width { construct; get; }

	/**
	 * The nominal video height of the game.
	 */
	public uint base_height { construct; get; }

	/**
	 * The maximum possible width of the game.
	 */
	public uint max_width { construct; get; }

	/**
	 * The maximum possible height of the game.
	 */
	public uint max_height { construct; get; }

	/**
	 * The nominal aspect ratio of the game.
	 *
	 * If aspect_ratio is <= 0.0, an aspect ratio of
	 * base_width / base_height is assumed.
	 *
	 * A frontend could override this setting if desired.
	 */
	public float aspect_ratio { construct; get; }

	/**
	 * The frames per seconds of the video content.
	 */
	public double fps { construct; get; }

	/**
	 * The sampling rate of the audio.
	 */
	public double sample_rate { construct; get; }

	internal AvInfo (SystemAvInfo system_av_info) {
		Object (
			base_width: system_av_info.geometry.base_width,
			base_height: system_av_info.geometry.base_height,
			max_width: system_av_info.geometry.max_width,
			max_height: system_av_info.geometry.max_height,
			aspect_ratio: system_av_info.geometry.aspect_ratio,
			fps: system_av_info.timing.fps,
			sample_rate: system_av_info.timing.sample_rate
		);
	}
}

}

