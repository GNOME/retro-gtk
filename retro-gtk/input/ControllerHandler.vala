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

			if (_core != null && _core.input_interface != this) {
				_core.input_interface = this;
				init_core ();
			}
		}
	}

	public KeyboardCallback? keyboard_callback { set; get; }

	private HashTable<uint?, ControllerDevice> controller_devices;

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
				bool is_capable = (capabilities & (device)) != 0;
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

	public void set_keyboard (KeyboardBox keyboard) {
		keyboard.key_press_event.connect ((w, e) => on_key_event (e, true));
		keyboard.key_release_event.connect ((w, e) => on_key_event (e, false));
	}

	private KeyboardModifierKey modifier_key_converter (int keyval, Gdk.ModifierType modifiers) {
		var retro_modifiers = KeyboardModifierKey.NONE;
		if ((bool) (modifiers & Gdk.ModifierType.SHIFT_MASK))
			retro_modifiers |= KeyboardModifierKey.SHIFT;
		if ((bool) (modifiers & Gdk.ModifierType.CONTROL_MASK))
			retro_modifiers |= KeyboardModifierKey.CTRL;
		if ((bool) (modifiers & Gdk.ModifierType.MOD1_MASK))
			retro_modifiers |= KeyboardModifierKey.ALT;
		if ((bool) (modifiers & Gdk.ModifierType.META_MASK))
			retro_modifiers |= KeyboardModifierKey.META;
		if (keyval == Gdk.Key.Num_Lock)
			retro_modifiers |= KeyboardModifierKey.NUMLOCK;
		if ((bool) (modifiers & Gdk.ModifierType.LOCK_MASK))
			retro_modifiers |= KeyboardModifierKey.CAPSLOCK;
		if (keyval == Gdk.Key.Scroll_Lock)
			retro_modifiers |= KeyboardModifierKey.SCROLLOCK;
		return retro_modifiers;
	}

	private KeyboardKey key_converter (uint keyval) {
		// Common keys (0x0020 to 0x00fe)
		if (keyval < 0x80) {
			var key = (0x7f & keyval);

			// If the key is uppercase, turn it lower case
			if (key >= 'A' && key <= 'Z')
				return (KeyboardKey) (key + 0x20);

			return (KeyboardKey) key;
		}

		// Function keys
		var fx = keyval - Gdk.Key.F1;
		if (fx < 15) return (KeyboardKey) ((uint) KeyboardKey.F1 + fx);

		// Keypad digits
		var kp = keyval - Gdk.Key.KP_0;
		if (kp < 10) return (KeyboardKey) ((uint) KeyboardKey.KP0 + kp);

		// Various keys
		// Missing keys: MODE, COMPOSE, POWER
		switch (keyval) {
			case Gdk.Key.BackSpace:
				return KeyboardKey.BACKSPACE;
			case Gdk.Key.Tab:
				return KeyboardKey.TAB;
			case Gdk.Key.Clear:
				return KeyboardKey.CLEAR;
			case Gdk.Key.Return:
				return KeyboardKey.RETURN;
			case Gdk.Key.Pause:
				return KeyboardKey.PAUSE;
			case Gdk.Key.Escape:
				return KeyboardKey.ESCAPE;
			case Gdk.Key.Delete:
				return KeyboardKey.DELETE;

			case Gdk.Key.Up:
				return KeyboardKey.UP;
			case Gdk.Key.Down:
				return KeyboardKey.DOWN;
			case Gdk.Key.Left:
				return KeyboardKey.LEFT;
			case Gdk.Key.Right:
				return KeyboardKey.RIGHT;
			case Gdk.Key.Insert:
				return KeyboardKey.INSERT;
			case Gdk.Key.Home:
				return KeyboardKey.HOME;
			case Gdk.Key.End:
				return KeyboardKey.END;
			case Gdk.Key.Page_Up:
				return KeyboardKey.PAGEUP;
			case Gdk.Key.Page_Down:
				return KeyboardKey.PAGEDOWN;

			case Gdk.Key.KP_Decimal:
				return KeyboardKey.KP_PERIOD;
			case Gdk.Key.KP_Divide:
				return KeyboardKey.KP_DIVIDE;
			case Gdk.Key.KP_Multiply:
				return KeyboardKey.KP_MULTIPLY;
			case Gdk.Key.KP_Subtract:
				return KeyboardKey.KP_MINUS;
			case Gdk.Key.KP_Add:
				return KeyboardKey.KP_PLUS;
			case Gdk.Key.KP_Enter:
				return KeyboardKey.KP_ENTER;
			case Gdk.Key.KP_Equal:
				return KeyboardKey.KP_EQUALS;

			case Gdk.Key.Num_Lock:
				return KeyboardKey.NUMLOCK;
			case Gdk.Key.Caps_Lock:
				return KeyboardKey.CAPSLOCK;
			case Gdk.Key.Scroll_Lock:
				return KeyboardKey.SCROLLOCK;
			case Gdk.Key.Shift_R:
				return KeyboardKey.RSHIFT;
			case Gdk.Key.Shift_L:
				return KeyboardKey.LSHIFT;
			case Gdk.Key.Control_R:
				return KeyboardKey.RCTRL;
			case Gdk.Key.Control_L:
				return KeyboardKey.LCTRL;
			case Gdk.Key.Alt_R:
				return KeyboardKey.RALT;
			case Gdk.Key.Alt_L:
				return KeyboardKey.LALT;
			case Gdk.Key.Meta_R:
				return KeyboardKey.RMETA;
			case Gdk.Key.Meta_L:
				return KeyboardKey.LMETA;
			case Gdk.Key.Super_R:
				return KeyboardKey.RSUPER;
			case Gdk.Key.Super_L:
				return KeyboardKey.LSUPER;

			case Gdk.Key.Help:
				return KeyboardKey.HELP;
			case Gdk.Key.Print:
				return KeyboardKey.PRINT;
			case Gdk.Key.Sys_Req:
				return KeyboardKey.SYSREQ;
			case Gdk.Key.Break:
				return KeyboardKey.BREAK;
			case Gdk.Key.Menu:
				return KeyboardKey.MENU;
			case Gdk.Key.EuroSign:
				return KeyboardKey.EURO;
			case Gdk.Key.Undo:
				return KeyboardKey.UNDO;

			default:
				return KeyboardKey.UNKNOWN;
		}
	}

	private bool on_key_event (Gdk.EventKey event, bool pressed) {
		if (keyboard_callback == null) return false;

		keyboard_callback.callback (
			pressed,
			key_converter (event.keyval),
			event.str.to_utf8 ()[0],
			modifier_key_converter (event.keyval, event.state)
		);

		return false;
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

