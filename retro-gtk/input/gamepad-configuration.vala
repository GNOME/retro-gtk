// This file is part of retro-gtk. License: GPL-3.0+.

public class Retro.GamepadConfiguration : Object {
	private uint16[] gamepad_keys;

	construct {
		gamepad_keys = new uint16[JoypadId.COUNT];
	}

	public void set_to_default () {
		set_button_key (JoypadId.B,      39); // QWERTY S
		set_button_key (JoypadId.Y,      38); // QWERTY A
		set_button_key (JoypadId.SELECT,           22); // Backspace
		set_button_key (JoypadId.START,            36); // Enter
		set_button_key (JoypadId.UP,    111); // Up arrow
		set_button_key (JoypadId.DOWN,  116); // Down arrow
		set_button_key (JoypadId.LEFT,  113); // Left arrow
		set_button_key (JoypadId.RIGHT, 114); // Right arrow
		set_button_key (JoypadId.A,     40); // QWERTY D
		set_button_key (JoypadId.X,        25); // QWERTY W
		set_button_key (JoypadId.L,       24); // QWERTY Q
		set_button_key (JoypadId.R,       26); // QWERTY E
		set_button_key (JoypadId.L2,      52); // QWERTY Z
		set_button_key (JoypadId.R2,      54); // QWERTY C
		set_button_key (JoypadId.L3,          10); // QWERTY 1
		set_button_key (JoypadId.R3,          12); // QWERTY 3
	}

	public void set_button_key (JoypadId button, uint16 key) {
		gamepad_keys[button] = key;
	}

	public uint16 get_button_key (JoypadId button) {
		return gamepad_keys[button];
	}
}
