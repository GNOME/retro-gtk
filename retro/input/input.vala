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

/**
 * An interface to render a {@link Core}'s inputs.
 */
public interface Input : Object {
	/**
	 * The core to handle the inputs for.
	 */
	public abstract Core core { get; set; }

	/**
	 * Asks the frontend to poll inputs.
	 */
	public abstract void poll ();

	/**
	 * Gets the state of a specific input.
	 *
	 * @param port the port of the controller to check the input state from
	 * @param device the type of the device to check the input state from
	 * @param index the index of given input (left or right stick)
	 * @param id the specific input to get (button or axis)
	 * @return the value of the given state
	 */
	public abstract int16 get_state (uint port, DeviceType device, uint index, uint id);

	/**
	 * Sets how the {@link core} handles the inputs in a human-readable way.
	 *
	 * @param input_descriptors the descriptors of the inputs used by the
	 * {@link core}
	 */
	public abstract void set_descriptors (InputDescriptor[] input_descriptors);

	/**
	 * Gets the flags of the devices implemented by the frontend.
	 *
	 * @return flags of the devices implemented by the frontend
	 */
	public abstract uint64 get_device_capabilities ();

	/**
	 * Warns the {@link core} of keyboard events.
	 *
	 * @param down whether the key has been pressed or released
	 * @param keycode the core of the key
	 * @param character the character of the key
	 * @param key_modifiers the modifier key which are held
	 */
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
