// This file is part of retro-gtk. License: GPL-3.0+.

namespace Retro {

/**
 * The input types of a pointer.
 */
private enum PointerId {
	X,
	Y,
	PRESSED
}

/**
 * Describes an input source.
 */
public struct InputDescriptor {
	uint port;
	DeviceType device;
	uint index;
	uint id;

	string description;
}

}

