// This file is part of Retro. License: GPLv3

namespace Retro {

public class InputDeviceManager : Object, Input {
	public Core _core;
	public Core core {
		get { return _core; }
		set {
			if (_core == value) return;

			_core = value;

			if (_core != null) {
				if (_core.input_interface != this)
					_core.input_interface = this;

				init_core ();
			}
		}
	}

	private HashTable<uint?, InputDevice> controller_devices;

	construct {
		controller_devices = new HashTable<int?, InputDevice> (int_hash, int_equal);
	}

	private void poll () {
		foreach (var device in controller_devices.get_values ())
			if (device != null) device.poll ();
	}

	private int16 get_state (uint port, DeviceType device, uint index, uint id) {
		if (controller_devices.contains (port)) {
			var controller_device = controller_devices.lookup (port);
			if (controller_device != null) {
				var capabilities = controller_device.get_device_capabilities ();
				bool is_capable = (capabilities & (1 << device)) != 0;
				if (is_capable) return controller_device.get_input_state (device, index, id);
			}
		}

		return 0;
	}

	public void set_descriptors (InputDescriptor[] input_descriptors) {
		// TODO
	}

	public uint64 get_device_capabilities () {
		// TODO
		return 0;
	}

	public void set_controller_device (uint port, InputDevice device) {
		if (controller_devices.contains (port))
			controller_devices.replace (port, device);
		else
			controller_devices.insert (port, device);

		controller_connected (port, device);
	}

	public void remove_controller_device (uint port) {
		if (!controller_devices.contains (port))
			return;

		controller_devices.remove (port);

		controller_disconnected (port);
	}

	public void init_core () {
		foreach (var port in controller_devices.get_keys ()) {
			core.set_controller_port_device (port, controller_devices[port].get_device_type ());
		}
	}
}

}

