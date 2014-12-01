/* Copyright (C) 2014  Adrien Plazas
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

namespace Retro {

public interface Input : Object {
	public abstract weak Core core { get; set; }

	public abstract void poll ();
	public abstract int16 get_state (uint port, DeviceType device, uint index, uint id);

	public abstract void set_descriptors (InputDescriptor[] input_descriptors);
	public abstract uint64 get_device_capabilities ();

	public void key_event (bool down, KeyboardKey keycode, uint32 character, KeyboardModifierKey key_modifiers) throws CbError {
		if (core == null)
			throw new CbError.NO_CORE ("No core");

		if (core.keyboard_callback == null)
			throw new CbError.NO_CALLBACK ("No keyboard callback");

		core.keyboard_callback.callback (down, keycode, character, key_modifiers);
	}
}

[CCode (has_target = false)]
private delegate void KeyboardCallbackCallback (bool down, KeyboardKey keycode, uint32 character, KeyboardModifierKey key_modifiers);

private struct KeyboardCallback {
	KeyboardCallbackCallback callback;
}

}
