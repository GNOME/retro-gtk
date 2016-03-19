// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * TODO Change visibility once the interface have been tested.
 */
internal enum RumbleEffect {
	STRONG,
	WEAK
}

/**
 * TODO Change visibility once the interface have been tested.
 */
internal interface Rumble: Object {
	public abstract bool set_rumble_state (uint port, RumbleEffect effect, uint16 strength);
}

}

