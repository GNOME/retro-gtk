// This file is part of Retro. License: GPLv3

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

}

