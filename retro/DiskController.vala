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
 * Handles a virtual disk drive
 *
 * Used by a {@link Core} which can swap out multiple disk images in runtime.
 *
 * If the {@link Core} can do this automatically, it should strive to do so.
 * However, there are cases where the user must manually do so.
 *
 * Overview: To swap a disk image, eject the disk image by setting
 * {@link eject_state} to true.
 * Set the {@link image_index} to the one you want to use.
 * Insert the disk again by setting {@link eject_state} to false.
 */
public class DiskControl: Object {
	private weak Core core;

	private DiskControlCallback callback_struct;

	internal DiskControl (Core core, DiskControlCallback callback_struct) {
		this.core = core;
		this.callback_struct = callback_struct;
	}

	/**
	 * Set the current eject state
	 *
	 * When set to true, "ejects" the virtual disk tray.
	 * When set to false, "closes" the virtual disk tray.
	 *
	 * When ejected, the disk image index can be set.
	 *
	 * The default state is "closed".
	 *
	 * @param ejected the desired eject state
	 * @return whether the change happened or not
	 */
	public bool set_eject_state (bool ejected) {
		if (callback_struct.set_eject_state == null) return false;

		core.push_cb_data ();
		var result = callback_struct.set_eject_state (ejected);
		core.pop_cb_data ();
		return result;
	}

	/**
	 * Get the current eject state
	 *
	 * See {@link set_eject_state} for more informations.
	 *
	 * @return the current eject state
	 */
	public bool get_eject_state () {
		if (callback_struct.get_eject_state == null) return false;

		core.push_cb_data ();
		var result = callback_struct.get_eject_state ();
		core.pop_cb_data ();
		return result;
	}

	/**
	 * Set the current disk index
	 *
	 * Can only be set when the disk drive is ejected.
	 *
	 * If the value is >= {@link get_num_images}, no disk is currently
	 * inserted.
	 *
	 * @param ejected the desired image index
	 * @return whether the change happened or not
	 */
	public bool set_image_index (uint image_index) {
		if (callback_struct.set_image_index == null) return false;

		core.push_cb_data ();
		var result = callback_struct.set_image_index (image_index);
		core.pop_cb_data ();
		return result;
	}

	/**
	 * The current disk index
	 *
	 * @return the current image index
	 */
	public uint get_image_index () {
		if (callback_struct.get_image_index == null) return 0;

		core.push_cb_data ();
		var result = callback_struct.get_image_index ();
		core.pop_cb_data ();
		return result;
	}

	/**
	 * Gets the total number of images which are available to use
	 *
	 * @return total number of images available to use
	 */
	public uint get_num_images () {
		if (callback_struct.get_num_images == null) return 0;

		core.push_cb_data ();
		var result = callback_struct.get_num_images ();
		core.pop_cb_data ();
		return result;
	}

	/**
	 * Replaces the disk image associated with index
	 *
	 * Virtual disk tray must be ejected when calling this.
	 *
	 * Arguments to pass in info have same requirements as
	 * {@link Core.load_game}.
	 *
	 * @param index index of the disk image to replace
	 * @param info information on the disk image to use
	 * @return whether the change happened or not
	 */
	public bool replace_image_index (uint index, GameInfo info) {
		if (callback_struct.replace_image_index == null) return false;

		core.push_cb_data ();
		var result = callback_struct.replace_image_index (index, info);
		core.pop_cb_data ();
		return result;
	}

	/**
	 * Removes the disk image associated with index
	 *
	 * Virtual disk tray must be ejected when calling this.
	 *
	 * It will remove the disk image from the internal list.
	 * As a result, {@link image_index} can change.
	 *
	 * E.g. {@link remove_image_index} (1), and previous
	 * {@link image_index} was 4 before.
	 * Index 1 will be removed, and the new index is 3.
	 *
	 * @param index index of the disk image to remove
	 * @return whether the change happened or not
	 */
	public bool remove_image_index (uint index) {
		if (callback_struct.replace_image_index == null) return false;

		var i = get_image_index ();
		core.push_cb_data ();
		var result = callback_struct.replace_image_index (index, null);
		core.pop_cb_data ();
		// Notify a change on the "image-index" property
		if (i != get_image_index ()) notify_property ("image-index");
		return result;
	}

	/**
	 * Adds a new valid index ({@link get_num_images}) to the internal disk
	 * list
	 *
	 * This will increment subsequent return values from {@link get_num_images}
	 * by 1.
	 * This image index cannot be used until a disk image has been set with
	 * {@link replace_image_index}.
	 *
	 * @return whether the change happened or not
	 */
	public bool add_image_index () {
		if (callback_struct.add_image_index == null) return false;

		core.push_cb_data ();
		var result = callback_struct.add_image_index ();
		core.pop_cb_data ();
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

