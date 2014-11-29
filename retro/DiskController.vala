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
 * Handles a virtual disk drive.
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
public interface DiskController: Object {
	/**
	 * The current eject state.
	 *
	 * When set to true, "ejects" the virtual disk tray.
	 * When set to false, "closes" the virtual disk tray.
	 *
	 * When ejected, the disk image index can be set.
	 *
	 * The default state is "closed".
	 */
	public abstract bool eject_state { set; get; default = false; }

	/**
	 * The current disk index.
	 *
	 * Can only be set when disk is ejected.
	 *
	 * If the value is >= {@link get_num_images}, no disk is currently inserted.
	 */
	public abstract uint image_index { set; get; default = 0; }

	/**
	 * Gets the total number of images which are available to use.
	 *
	 * @return total number of images available to use
	 */
	public abstract uint get_num_images ();

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
	 * @return FIXME
	 */
	public abstract bool replace_image_index (uint index, GameInfo info);

	/**
	 * Removes the disk image associated with index.
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
	 * @return FIXME
	 */
	public abstract bool remove_image_index (uint index);

	/**
	 * Adds a new valid index ({@link get_num_images}) to the internal disk
	 * list.
	 *
	 * This will increment subsequent return values from {@link get_num_images}
	 * by 1.
	 * This image index cannot be used until a disk image has been set with
	 * {@link replace_image_index}.
	 *
	 * @return FIXME
	 */
	public abstract bool add_image_index ();
}

private class CoreDiskController: Object, DiskController {
	[CCode (has_target = false)]
	internal delegate bool SetEjectState (bool ejected);

	[CCode (has_target = false)]
	internal delegate bool GetEjectState ();

	[CCode (has_target = false)]
	internal delegate uint GetImageIndex ();

	[CCode (has_target = false)]
	internal delegate bool SetImageIndex (uint index);

	[CCode (has_target = false)]
	internal delegate uint GetNumImages ();

	[CCode (has_target = false)]
	internal delegate bool ReplaceImageIndex (uint index, GameInfo? info);

	[CCode (has_target = false)]
	internal delegate bool AddImageIndex ();

	internal struct Callback {
		public SetEjectState set_eject_state;
		public GetEjectState get_eject_state;
		public GetImageIndex get_image_index;
		public SetImageIndex set_image_index;
		public GetNumImages get_num_images;
		public ReplaceImageIndex replace_image_index;
		public AddImageIndex add_image_index;
	}

	public Callback callback_struct { construct; private get; }

	public bool eject_state {
		set {
			if (callback_struct.set_eject_state != null) {
				callback_struct.set_eject_state (value);
			}
		}
		get {
			if (callback_struct.get_eject_state != null) {
				return callback_struct.get_eject_state ();
			}
			return false;
		}
	}

	public uint image_index {
		set {
			if (callback_struct.set_image_index != null) {
				callback_struct.set_image_index (value);
			}
		}
		get {
			if (callback_struct.get_image_index != null) {
				return callback_struct.get_image_index ();
			}
			return 0;
		}
	}

	internal CoreDiskController (Callback callback_struct) {
		Object (callback_struct: callback_struct);
	}

	public uint get_num_images () {
		if (callback_struct.get_num_images != null) {
			return callback_struct.get_num_images ();
		}
		return 0;
	}

	public bool replace_image_index (uint index, GameInfo info) {
		if (callback_struct.replace_image_index != null) {
			return callback_struct.replace_image_index (index, info);
		}
		return false;
	}


	public bool remove_image_index (uint index) {
		if (callback_struct.replace_image_index != null) {
			var i = image_index;
			var result = callback_struct.replace_image_index (index, null);
			// Notify a change on the "image-index" property
			if (i != image_index) notify_property ("image-index");
			return result;
		}
		return false;
	}

	public bool add_image_index () {
		if (callback_struct.add_image_index != null) {
			return callback_struct.add_image_index ();
		}
		return false;
	}
}

}

