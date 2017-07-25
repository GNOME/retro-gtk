// This file is part of retro-gtk. License: GPL-3.0+.

public class Retro.Gamepad: Object, InputDevice {
	internal void *internal;

	public Gamepad (bool analog) {
		internal_init (analog);
	}

	~Gamepad () {
		internal_finalize ();
	}

	public void poll () {}

	public int16 get_input_state (DeviceType device, uint index, uint id) {
		return internal_get_input_state (device, index, id);
	}

	public DeviceType get_device_type () {
		return internal_get_device_type ();
	}
	public uint64 get_device_capabilities () {
		return internal_get_device_capabilities ();
	}

	public extern void button_press_event (uint16 button);
	public extern void button_release_event (uint16 button);
	public extern void axis_event (uint16 axis, double value);
	private extern int16 internal_get_input_state (DeviceType device, uint index, uint id);
	private extern DeviceType internal_get_device_type ();
	private extern uint64 internal_get_device_capabilities ();
	private extern void internal_init (bool analog);
	private extern void internal_finalize ();
}
