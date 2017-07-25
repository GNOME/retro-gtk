// This file is part of retro-gtk. License: GPL-3.0+.

namespace Retro {

/**
 * The device types.
 */
public enum DeviceType {
	TYPE_MASK = 0xff,

	/* See RETRO_DEVICE_NONE and below in libretro.h for docs */
	NONE = 0,
	JOYPAD = 1,
	MOUSE = 2,
	KEYBOARD = 3,
	LIGHTGUN = 4,
	ANALOG = 5,
	POINTER = 6;

	/**
	 * Gets the basic type of a device type.
	 *
	 * Applies the type mask on a DeviceType to get its basic type.
	 * If the device type is already basic, it will return the same type.
	 *
	 * E.g DeviceType.JOYPAD_MULTITAP.get_basic_type () returns
	 * DeviceType.JOYPAD, and DeviceType.JOYPAD.get_basic_type () also
	 * returns Type.JOYPAD.
	 *
	 * @return the basic type of a device type
	 */
	public DeviceType get_basic_type () {
		return this & DeviceType.TYPE_MASK;
	}
}

/**
 * The input types of a joypad.
 */
public enum JoypadId {
	B,
	Y,
	SELECT,
	START,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	A,
	X,
	L,
	R,
	L2,
	R2,
	L3,
	R3,
	COUNT,
}

/**
 * The analog sticks of an analog joypad.
 */
public enum AnalogIndex {
	LEFT,
	RIGHT,
}

/**
 * The axes of an analog stick.
 */
public enum AnalogId {
	X,
	Y,
	COUNT,
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

