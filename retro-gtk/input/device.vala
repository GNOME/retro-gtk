// This file is part of retro-gtk. License: GPL-3.0+.

namespace Retro {

/**
 * The axes of an analog stick.
 */
public enum AnalogId {
	X,
	Y
}

/**
 * The input types of a mouse.
 */
private enum MouseId {
	X,
	Y,
	LEFT,
	RIGHT,
	WHEELUP,
	WHEELDOWN,
	MIDDLE,
	HORIZ_WHEELUP,
	HORIZ_WHEELDOWN,
}

/**
 * The input types of a lightgun.
 */
private enum LightgunId {
	X,
	Y,
	TRIGGER,
	CURSOR,
	TURBO,
	PAUSE,
	START
}

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

