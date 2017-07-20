// This file is part of retro-gtk. License: GPL-3.0+.

namespace Retro {

public enum RumbleEffect {
	STRONG,
	WEAK
}

public interface Rumble: Object {
	public abstract bool set_rumble_state (uint port, RumbleEffect effect, uint16 strength);
}

}

