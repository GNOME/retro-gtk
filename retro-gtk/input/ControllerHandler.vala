/* Engine.vala  A simple frontend for libretro.
 * Copyright (C) 2014  Adrien Plazas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

using Retro;

namespace RetroGtk {

public class ControllerHandler : Object, Retro.Input {
	public weak Core _core;
	public weak Core core {
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

	private HashTable<uint?, ControllerDevice> controller_devices;
	private Keyboard keyboard;

	construct {
		controller_devices = new HashTable<int?, ControllerDevice> (int_hash, int_equal);
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

	public void set_controller_device (uint port, ControllerDevice device) {
		if (controller_devices.contains (port)) {
			controller_devices.replace (port, device);
		}
		else {
			controller_devices.insert (port, device);
		}

		if (core != null)
			core.set_controller_port_device (port, Retro.DeviceType.JOYPAD);
	}

	public void set_keyboard (Keyboard keyboard) {
		this.keyboard = keyboard;

		keyboard.key_event.connect ((p, k, c, m) => {
			try {
				key_event (p, k, c, m);
			}
			catch (CbError e) {
				// There is no core or the core set no keyboard callback
			}
		});
	}

	public void remove_controller_device (uint port) {
		if (controller_devices.contains (port)) {
			controller_devices.remove (port);
		}

		if (core != null)
			core.set_controller_port_device (port, DeviceType.NONE);
	}

	public void init_core () {
		foreach (var port in controller_devices.get_keys ()) {
			core.set_controller_port_device (port, Retro.DeviceType.JOYPAD);
		}
	}
}

}

