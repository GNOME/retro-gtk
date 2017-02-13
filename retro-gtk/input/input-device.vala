// This file is part of Retro. License: GPLv3

namespace Retro {

public interface InputDevice : Object {
	public abstract void  poll ();
	public abstract int16 get_input_state (DeviceType device, uint index, uint id);

	public abstract DeviceType get_device_type ();
	public abstract uint64 get_device_capabilities ();
}

}

