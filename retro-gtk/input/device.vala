// This file is part of retro-gtk. License: GPL-3.0+.

namespace Retro {

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

