// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * The device types.
 */
public enum DeviceType {
	TYPE_MASK = 0xff,

	/**
	 * No device type.
	 */
	NONE = 0,

	/**
	 * A classic joypad.
	 *
	 * The joypad is called RetroPad.
	 *
	 * It is essentially a Super Nintendo controller,
	 * but with additional L2/R2/L3/R3 buttons, similar to a PlayStation DualShock.
	 */
	JOYPAD = 1,

	/**
	 * A simple mouse.
	 *
	 * It is similar to Super Nintendo's mouse (with two axes and two buttons).
	 *
	 * X and Y coordinates are reported relatively to last poll (poll callback).
	 * It is up to the Libretro implementation to keep track of where the mouse pointer
	 * is supposed to be on the screen.
	 *
	 * The frontend must make sure not to interfere with its own hardware mouse pointer.
	 */
	MOUSE = 2,

	/**
	 * A keyboard.
	 *
	 * Keyboard device lets one poll for hardware key pressed.
	 *
	 * It is poll based, so input callback will return with the current pressed state.
	 */
	KEYBOARD = 3,

	/**
	 * A lightgun.
	 *
	 * X/Y coordinates are reported relatively to last poll, similar to mouse.
	 */
	LIGHTGUN = 4,

	/**
	 * An analog joypad.
	 *
	 * It is an extension to the joypad (RetroPad).
	 *
	 * Similar to DualShock it adds two analog sticks.
	 *
	 * This is treated as a separate device type as it returns values in the full analog range
	 * of [-0x8000, 0x7fff]. Positive X axis is right. Positive Y axis is down.
	 * Only use the analog type when polling for analog values of the axes.
	 */
	ANALOG = 5,

	/**
	 * An abstract pointing mecanism (e.g. touch).
	 *
	 * This allows Libretro to query in absolute coordinates where on the screen a pointer
	 * (a mouse or something similar) is being placed.
	 * For a touch centric device, coordinates reported are the coordinates of the press.
	 *
	 * Coordinates in X and Y are reported as:
	 * [-0x7fff, 0x7fff]: -0x7fff corresponds to the far left/top of the screen,
	 * and 0x7fff corresponds to the far right/bottom of the screen.
	 *
	 * The "screen" is here defined as area that is passed to the frontend and later displayed on
	 * the monitor.
	 * The frontend is free to scale/resize this screen as it sees fit, however,
	 * (X, Y) = (-0x7fff, -0x7fff) will correspond to the top-left pixel of the game image, etc.
	 *
	 * To check if the pointer coordinates are valid (e.g. a touch display actually being touched),
	 * {@link PointerId.PRESSED} returns 1 or 0.
	 * If using a mouse, {@link PointerId.PRESSED} will usually correspond to the left
	 * mouse button.
	 * {@link PointerId.PRESSED} will only return 1 if the pointer is inside the game
	 * screen.
	 *
	 * For multi-touch, the index argument of {@link Input.get_state} can be used to
	 * successively query more presses.
	 * If index = 0 returns 1 for {@link PointerId.PRESSED}, coordinates can be extracted
	 * with {@link PointerId.X}, {@link PointerId.Y} for index = 0.
	 * One can then query {@link PointerId.PRESSED}, {@link PointerId.X},
	 * {@link PointerId.Y} with index = 1, and so on.
	 * Eventually {@link PointerId.PRESSED} will return 0 for an index. No further
	 * presses are registered at this point.
	 */
	POINTER = 6,

	/**
	 * A joypad multitap.
	 *
	 * It is a specialization of the {@link DeviceType.JOYPAD}.
	 *
	 * It should only be used in {@link Core.set_controller_port_device} to inform Libretro
	 * implementations about use of a joypad multitap.
	 *
	 * In input state callback, however, the device will be {@link DeviceType.JOYPAD}.
	 */
	JOYPAD_MULTITAP = (1 << 8) | DeviceType.JOYPAD,

	/**
	 * A Super Nintendo Super Scope.
	 *
	 * It is a specialization of the {@link DeviceType.LIGHTGUN}.
	 *
	 * It should only be used in {@link Core.set_controller_port_device} to inform Libretro
	 * implementations about use of a Super Nintendo Super Scope.
	 *
	 * In input state callback, however, the device will be {@link DeviceType.LIGHTGUN}.
	 */
	LIGHTGUN_SUPER_SCOPE = (1 << 8) | DeviceType.LIGHTGUN,

	/**
	 * A Konami Justifier.
	 *
	 * It is a specialization of the {@link DeviceType.LIGHTGUN}.
	 *
	 * It should only be used in {@link Core.set_controller_port_device} to inform Libretro
	 * implementations about use of a Konami Justifier.
	 *
	 * In input state callback, however, the device will be {@link DeviceType.LIGHTGUN}.
	 */
	LIGHTGUN_JUSTIFIER = (2 << 8) | DeviceType.LIGHTGUN,

	/**
	 * Konami Justifiers.
	 *
	 * It is a specialization of the {@link DeviceType.LIGHTGUN}.
	 *
	 * It should only be used in {@link Core.set_controller_port_device} to inform Libretro
	 * implementations about use of Konami Justifiers.
	 *
	 * In input state callback, however, the device will be {@link DeviceType.LIGHTGUN}.
	 */
	LIGHTGUN_JUSTIFIERS = (3 << 8) | DeviceType.LIGHTGUN;

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
	R3
}

/**
 * The analog sticks of an analog joypad.
 */
public enum AnalogIndex {
	LEFT,
	RIGHT
}

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
public enum MouseId {
	X,
	Y,
	LEFT,
	RIGHT,
	WHEELUP,
	WHEELDOWN,
	MIDDLE
}

/**
 * The input types of a lightgun.
 */
public enum LightgunId {
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
public enum PointerId {
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

