// This file is part of retro-gtk. License: GPL-3.0+.

public class Retro.GamepadConfiguration : Object {
	internal uint16[] gamepad_keys;

	construct {
		gamepad_keys = new uint16[JoypadId.COUNT];
	}

	public extern void set_to_default ();
	public extern void set_button_key (uint16 button, uint16 key);
	public extern uint16 get_button_key (uint16 button);
}
