// This file is part of RetroGtk. License: GPLv3

namespace RetroGtk {

public class VirtualGamepad : Object, Retro.InputDevice {
	public Gtk.Widget widget { get; construct; }
	public GamepadConfiguration configuration { get; construct set; }

	private KeyboardState keyboard;

	public VirtualGamepad (Gtk.Widget widget) {
		Object (widget: widget);
	}

	public VirtualGamepad.with_configuration (Gtk.Widget widget, GamepadConfiguration configuration) {
		Object (widget: widget, configuration: configuration);
	}

	construct {
		keyboard = new KeyboardState (widget);

		if (configuration == null) {
			configuration = new GamepadConfiguration ();
			configuration.set_to_default ();
		}
	}

	public void poll () {}

	public int16 get_input_state (Retro.DeviceType device, uint index, uint id) {
		if ((Retro.DeviceType) device != Retro.DeviceType.JOYPAD) return 0;

		return get_button_pressed ((GamepadButtonType) id) ? int16.MAX : 0;
	}

	public Retro.DeviceType get_device_type () {
		return Retro.DeviceType.JOYPAD;
	}

	public uint64 get_device_capabilities () {
		return 1 << Retro.DeviceType.JOYPAD;
	}

	public bool get_button_pressed (GamepadButtonType button) {
		return keyboard.get_key_state (configuration.get_button_key (button));
	}
}

}

