// This file is part of RetroGtk. License: GPLv3

namespace RetroGtk {

public class InputDeviceManager : Retro.InputDeviceManager {
	private Keyboard keyboard;

	public void set_keyboard (Keyboard keyboard) {
		this.keyboard = keyboard;

		keyboard.key_event.connect ((p, k, c, m) => {
			key_event (p, k, c, m);
		});
	}
}

}

