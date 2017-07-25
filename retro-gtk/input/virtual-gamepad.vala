// This file is part of retro-gtk. License: GPL-3.0+.

public class Retro.VirtualGamepad : Object, InputDevice {
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

	public int16 get_input_state (DeviceType device, uint index, uint id) {
		if ((DeviceType) device != DeviceType.JOYPAD) return 0;

		return get_button_pressed ((JoypadId) id) ? int16.MAX : 0;
	}

	public DeviceType get_device_type () {
		return DeviceType.JOYPAD;
	}

	public uint64 get_device_capabilities () {
		return 1 << DeviceType.JOYPAD;
	}

	public bool get_button_pressed (JoypadId button) {
		return keyboard.get_key_state (configuration.get_button_key (button));
	}
}
