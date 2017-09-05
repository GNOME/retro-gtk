// This file is part of retro-gtk. License: GPL-3.0+.

[CCode (cheader_filename = "retro-device-type.h")]
public enum Retro.DeviceType {
	TYPE_MASK = 0xff,
	NONE = 0,
	JOYPAD = 1,
	MOUSE = 2,
	KEYBOARD = 3,
	LIGHTGUN = 4,
	ANALOG = 5,
	POINTER = 6,
}
