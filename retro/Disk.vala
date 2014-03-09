/* Retro  GObject libretro wrapper.
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

namespace Retro {

namespace Disk {
	/**
	 * If ejected is true, "ejects" the virtual disk tray.
	 * 
	 * When ejected, the disk image index can be set.
	 * 
	 * @param ejected FIXME
	 * @return FIXME
	 */
	[CCode (has_target = false)]
	public delegate bool SetEjectState (bool ejected);
	
	/**
	 * Gets current eject state.
	 * 
	 * The initial state is 'not ejected'.
	 * 
	 * @return FIXME
	 */
	[CCode (has_target = false)]
	public delegate bool GetEjectState ();
	
	/**
	 * Gets current disk index.
	 * 
	 * First disk is index 0.
	 * If return value is >= get_num_images(), no disk is currently inserted.
	 * 
	 * @return FIXME
	 */
	[CCode (has_target = false)]
	public delegate uint GetImageIndex ();
	
	/**
	 * Sets image index.
	 * 
	 * Can only be called when disk is ejected.
	 * The implementation supports setting "no disk" by using an index >=
	 * get_num_images ().
	 * 
	 * @param index FIXME
	 * @return FIXME
	 */
	[CCode (has_target = false)]
	public delegate bool SetImageIndex (uint index);
	
	/**
	 * Gets total number of images which are available to use.
	 * 
	 * @return FIXME
	 */
	[CCode (has_target = false)]
	public delegate uint GetNumImages ();
	
	/**
	 * Replaces the disk image associated with index.
	 * 
	 * Arguments to pass in info have same requirements as
	 * {@link Retro.Core.load_game}.
	 * Virtual disk tray must be ejected when calling this.
	 * Replacing a disk image with info = null will remove the disk image from
	 * the internal list.
	 * As a result, calls to {@link Retro.Disk.GetImageIndex} can change.
	 *
	 * E.g. {@link Retro.Disk.ReplaceImageIndex} (1, null), and previous
	 * {@link Retro.Disk.GetImageIndex} returned 4 before.
	 * Index 1 will be removed, and the new index is 3.
	 * 
	 * @param index FIXME
	 * @param info FIXME
	 * @return FIXME
	 */
	[CCode (has_target = false)]
	public delegate bool ReplaceImageIndex (uint index, GameInfo info);
	
	/**
	 * Adds a new valid index ({@link Retro.Disk.GetNumImages}) to the
	 * internal disk list.
	 * 
	 * This will increment subsequent return values from
	 * {@link Retro.Disk.GetNumImages} by 1.
	 * This image index cannot be used until a disk image has been set with
	 * {@link Retro.Disk.ReplaceImageIndex}.
	 * 
	 * @return FIXME
	 */
	[CCode (has_target = false)]
	public delegate bool AddImageIndex ();
	
	
	/**
	 * Callbacks for {@link Retro.Environment.Command.SET_DISK_CONTROL_INTERFACE}.
	 * 
	 * Should be set for implementations which can swap out multiple disk images
	 * in runtime.
	 * If the implementation can do this automatically, it should strive to do
	 * so.
	 * However, there are cases where the user must manually do so.
	 * 
	 * Overview: To swap a disk image, eject the disk image with
	 * set_eject_state (true).
	 * Set the disk index with set_image_index (index).
	 * Insert the disk again with set_eject_state (false).
	 */
	public struct ControlCallback {
		public SetEjectState set_eject_state;
		public GetEjectState get_eject_state;
		public GetImageIndex get_image_index;
		public SetImageIndex set_image_index;
		public GetNumImages get_num_images;
		public ReplaceImageIndex replace_image_index;
		public AddImageIndex add_image_index;
	}
}

}

