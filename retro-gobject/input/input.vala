// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * An interface to render a {@link Core}'s inputs.
 */
public interface Input : Object {
	public signal void controller_connected (uint port, InputDevice device);
	public signal void controller_disconnected (uint port);

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
