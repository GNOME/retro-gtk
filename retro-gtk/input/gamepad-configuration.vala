// This file is part of retro-gtk. License: GPL-3.0+.

public class Retro.GamepadConfiguration : Object {
	private uint16[] gamepad_keys;

	construct {
		gamepad_keys = new uint16[17];
	}

	public void set_to_default () {
		set_button_key (GamepadButtonType.ACTION_DOWN,      39); // QWERTY S
		set_button_key (GamepadButtonType.ACTION_LEFT,      38); // QWERTY A
		set_button_key (GamepadButtonType.SELECT,           22); // Backspace
		set_button_key (GamepadButtonType.START,            36); // Enter
		set_button_key (GamepadButtonType.DIRECTION_UP,    111); // Up arrow
		set_button_key (GamepadButtonType.DIRECTION_DOWN,  116); // Down arrow
		set_button_key (GamepadButtonType.DIRECTION_LEFT,  113); // Left arrow
		set_button_key (GamepadButtonType.DIRECTION_RIGHT, 114); // Right arrow
		set_button_key (GamepadButtonType.ACTION_RIGHT,     40); // QWERTY D
		set_button_key (GamepadButtonType.ACTION_UP,        25); // QWERTY W
		set_button_key (GamepadButtonType.SHOULDER_L,       24); // QWERTY Q
		set_button_key (GamepadButtonType.SHOULDER_R,       26); // QWERTY E
		set_button_key (GamepadButtonType.SHOULDER_L2,      52); // QWERTY Z
		set_button_key (GamepadButtonType.SHOULDER_R2,      54); // QWERTY C
		set_button_key (GamepadButtonType.STICK_L,          10); // QWERTY 1
		set_button_key (GamepadButtonType.STICK_R,          12); // QWERTY 3
		set_button_key (GamepadButtonType.HOME,            110); // Home
	}

	public void set_button_key (GamepadButtonType button, uint16 key) {
		gamepad_keys[button] = key;
	}

	public uint16 get_button_key (GamepadButtonType button) {
		return gamepad_keys[button];
	}
}
