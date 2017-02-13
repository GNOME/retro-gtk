// This file is part of retro-gtk. License: GPLv3

public enum Retro.GamepadButtonType {
	ACTION_DOWN,
	ACTION_LEFT,
	SELECT,
	START,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	ACTION_RIGHT,
	ACTION_UP,
	SHOULDER_L,
	SHOULDER_R,
	SHOULDER_L2,
	SHOULDER_R2,
	STICK_L,
	STICK_R,
	HOME;

	public static size_t size () {
		return 1 + (size_t) GamepadButtonType.HOME;
	}
}
