// This file is part of retro-gtk. License: GPL-3.0+.

namespace Retro {

/**
 * Handles a virtual disk drive.
 *
 * It si used by a {@link Core} which can swap out multiple disk images at
 * runtime.
 *
 * If the {@link Core} can do this automatically, it should strive to do so.
 * However, there are cases where the user must manually do so.
 *
 * To swap a disk image:
 *
 *  * eject the disk image by setting the eject state to true;
 *  * set the image index to the one you want to use.
 *  * insert the disk again by setting the eject state to false.
 */
public class DiskControl: Object {
	private weak Core core;

	private DiskControlCallback callback_struct;

	internal DiskControl (Core core, DiskControlCallback callback_struct) {
		this.core = core;
		this.callback_struct = callback_struct;
	}

	/**
	 * Sets the current eject state.
	 *
	 * When set to true, ejects the virtual disk tray.
	 * When set to false, closes the virtual disk tray.
	 *
	 * When ejected, the disk image index can be set.
	 *
	 * The default state is "closed".
	 *
	 * @param ejected the desired eject state
	 * @return //true// on successfully changed eject state, //false// otherwise
	 * @throws Error the core or its callback couldn't be found
	 */
	public bool set_eject_state (bool ejected) throws Error {
		if (core == null)
			throw new CbError.NO_CORE ("DiskControl has no core");

		if (callback_struct.set_eject_state == null)
			throw new CbError.NO_CALLBACK ("DiskControl has no callback for this operation");

		core.push_cb_data ();
		var result = callback_struct.set_eject_state (ejected);
		Core.pop_cb_data ();
		return result;
	}

	/**
	 * Gets the current eject state.
	 *
	 * See {@link set_eject_state} for more informations.
	 *
	 * @return the current eject state
	 * @throws Error the core or its callback couldn't be found
	 */
	public bool get_eject_state () throws Error {
		if (core == null)
			throw new CbError.NO_CORE ("DiskControl has no core");

		if (callback_struct.get_eject_state == null)
			throw new CbError.NO_CALLBACK ("DiskControl has no callback for this operation");

		core.push_cb_data ();
		var result = callback_struct.get_eject_state ();
		Core.pop_cb_data ();
		return result;
	}

	/**
	 * Sets the current disk index.
	 *
	 * Can only be set when the disk drive is ejected.
	 *
	 * If the value is >= to the total number of images,
	 * no disk is currently inserted.
	 *
	 * @param image_index the desired image index
	 * @return //true// on successfully changed image index, //false// otherwise
	 * @throws Error the core or its callback couldn't be found
	 */
	public bool set_image_index (uint image_index) throws Error {
		if (core == null)
			throw new CbError.NO_CORE ("DiskControl has no core");

		if (callback_struct.set_image_index == null)
			throw new CbError.NO_CALLBACK ("DiskControl has no callback for this operation");

		core.push_cb_data ();
		var result = callback_struct.set_image_index (image_index);
		Core.pop_cb_data ();
		return result;
	}

	/**
	 * Gets the current disk index.
	 *
	 * @return the current image index
	 * @throws Error the core or its callback couldn't be found
	 */
	public uint get_image_index () throws Error {
		if (core == null)
			throw new CbError.NO_CORE ("DiskControl has no core");

		if (callback_struct.get_image_index == null)
			throw new CbError.NO_CALLBACK ("DiskControl has no callback for this operation");

		core.push_cb_data ();
		var result = callback_struct.get_image_index ();
		Core.pop_cb_data ();
		return result;
	}

	/**
	 * Gets the total number of images which are available to use.
	 *
	 * @return total number of images available to use
	 * @throws Error the core or its callback couldn't be found
	 */
	public uint get_num_images () throws Error {
		if (core == null)
			throw new CbError.NO_CORE ("DiskControl has no core");

		if (callback_struct.get_num_images == null)
			throw new CbError.NO_CALLBACK ("DiskControl has no callback for this operation");

		core.push_cb_data ();
		var result = callback_struct.get_num_images ();
		Core.pop_cb_data ();
		return result;
	}

	/**
	 * Replaces the disk image associated with index.
	 *
	 * Virtual disk tray must be ejected when calling this.
	 *
	 * Arguments to pass in info have same requirements as
	 * {@link Core.load_game}.
	 *
	 * @param index index of the disk image to replace
	 * @param info information on the disk image to use
	 * @return //true// on successfully replaced image, //false// otherwise
	 * @throws Error the core or its callback couldn't be found
	 */
	public bool replace_image_index (uint index, GameInfo info) throws Error {
		if (core == null)
			throw new CbError.NO_CORE ("DiskControl has no core");

		if (callback_struct.replace_image_index == null)
			throw new CbError.NO_CALLBACK ("DiskControl has no callback for this operation");

		core.push_cb_data ();
		var result = callback_struct.replace_image_index (index, info);
		Core.pop_cb_data ();
		return result;
	}

	/**
	 * Removes the disk image associated with index.
	 *
	 * Virtual disk tray must be ejected when calling this.
	 *
	 * It will remove the disk image from the internal list.
	 * As a result, the current image index can change.
	 *
	 * E.g. remove_image_index (1), and previous
	 * image index was 4 before.
	 * Index 1 will be removed, and the new index is 3.
	 *
	 * @param index index of the disk image to remove
	 * @return //true// on successfully removed index, //false// otherwise
	 * @throws Error the core or its callback couldn't be found
	 */
	public bool remove_image_index (uint index) throws Error {
		if (core == null)
			throw new CbError.NO_CORE ("DiskControl has no core");

		if (callback_struct.replace_image_index == null)
			throw new CbError.NO_CALLBACK ("DiskControl has no callback for this operation");

		var i = get_image_index ();
		core.push_cb_data ();
		var result = callback_struct.replace_image_index (index, null);
		Core.pop_cb_data ();
		// Notify a change on the "image-index" property
		if (i != get_image_index ()) notify_property ("image-index");
		return result;
	}

	/**
	 * Adds a new valid index to the internal disk lit.
	 *
	 * This will increment subsequent return values from {@link get_num_images}
	 * by 1.
	 * This image index cannot be used until a disk image has been set with
	 * {@link replace_image_index}.
	 *
	 * @return //true// on successfully added index, //false// otherwise
	 * @throws Error the core or its callback couldn't be found
	 */
	public bool add_image_index () throws Error {
		if (core == null)
			throw new CbError.NO_CORE ("DiskControl has no core");

		if (callback_struct.add_image_index == null)
			throw new CbError.NO_CALLBACK ("DiskControl has no callback for this operation");

		core.push_cb_data ();
		var result = callback_struct.add_image_index ();
		Core.pop_cb_data ();
		return result;
	}
}

internal struct DiskControlCallback {
	public DiskControlCallbackSetEjectState set_eject_state;
	public DiskControlCallbackGetEjectState get_eject_state;
	public DiskControlCallbackGetImageIndex get_image_index;
	public DiskControlCallbackSetImageIndex set_image_index;
	public DiskControlCallbackGetNumImages get_num_images;
	public DiskControlCallbackReplaceImageIndex replace_image_index;
	public DiskControlCallbackAddImageIndex add_image_index;
}

[CCode (has_target = false)]
private delegate bool DiskControlCallbackSetEjectState (bool ejected);

[CCode (has_target = false)]
private delegate bool DiskControlCallbackGetEjectState ();

[CCode (has_target = false)]
private delegate uint DiskControlCallbackGetImageIndex ();

[CCode (has_target = false)]
private delegate bool DiskControlCallbackSetImageIndex (uint index);

[CCode (has_target = false)]
private delegate uint DiskControlCallbackGetNumImages ();

[CCode (has_target = false)]
private delegate bool DiskControlCallbackReplaceImageIndex (uint index, GameInfo? info);

[CCode (has_target = false)]
private delegate bool DiskControlCallbackAddImageIndex ();

}

