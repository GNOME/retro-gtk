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

public interface Environment: Object {
	public enum Command {
		EXPERIMENTAL_MASK             = 0x10000,
		PRIVATE_MASK                  = 0x20000,
		SET_ROTATION                  = 1,
		GET_OVERSCAN                  = 2,
		GET_CAN_DUPE                  = 3,
		SET_MESSAGE                   = 6,
		SHUTDOWN                      = 7,
		SET_PERFORMANCE_LEVEL         = 8,
		GET_SYSTEM_DIRECTORY          = 9,
		SET_PIXEL_FORMAT              = 10,
		SET_INPUT_DESCRIPTORS         = 11,
		SET_KEYBOARD_CALLBACK         = 12,
		SET_DISK_CONTROL_INTERFACE    = 13,
		SET_HW_RENDER                 = 14,
		GET_VARIABLE                  = 15,
		SET_VARIABLES                 = 16,
		GET_VARIABLE_UPDATE           = 17,
		SET_SUPPORT_NO_GAME           = 18,
		GET_LIBRETRO_PATH             = 19,
		SET_AUDIO_CALLBACK            = 22,
		SET_FRAME_TIME_CALLBACK       = 21,
		GET_RUMBLE_INTERFACE          = 23,
		GET_INPUT_DEVICE_CAPABILITIES = 24,
		GET_SENSOR_INTERFACE          = (25 | Command.EXPERIMENTAL_MASK),
		GET_CAMERA_INTERFACE          = (26 | Command.EXPERIMENTAL_MASK),
		GET_LOG_INTERFACE             = 27,
		GET_PERF_INTERFACE            = 28,
		GET_LOCATION_INTERFACE        = 29,
		GET_CONTENT_DIRECTORY         = 30,
		GET_SAVE_DIRECTORY            = 31,
		SET_SYSTEM_AV_INFO            = 32
	}
	
	public signal bool shutdown ();
	public signal bool set_message (Message message);
	
	public abstract bool overscan { set; get; default = true; }
	public abstract bool can_dupe { set; get; default = false; }
	public abstract uint64 input_device_capabilities { get; }
	public abstract string system_directory { set; get; default = "."; }
	public abstract string libretro_path { set; get; default = "."; }
	public abstract string content_directory { set; get; default = "."; }
	public abstract string save_directory { set; get; default = "."; }
	
	public abstract string? get_variable (string key);
	public abstract bool variable_update { set; get; default = false; }
	public abstract bool set_variables (Variable[] variables);
	
	public abstract Rotation rotation { set; get; default = Rotation.NONE; }
	public abstract bool support_no_game { set; get; default = false; }
	public abstract Performance.Level performance_level { set; get; }
	public abstract Video.PixelFormat pixel_format { set; get; default = Video.PixelFormat.ORGB1555; }
	public abstract Device.InputDescriptor[] input_descriptors { set; get; }
	public abstract SystemAvInfo? system_av_info { set; get; default = null; }
	
	public abstract Keyboard.Callback? keyboard_callback { set; get; default = null; }
	public abstract Disk.ControlCallback? disk_control_interface { set; get; default = null; }
	public abstract Hardware.RenderCallback? hw_render { set; get; default = null; }
	public abstract Audio.Callback? audio_callback { set; get; default = null; }
	public abstract FrameTime.Callback? frame_time_callback { set; get; default = null; }
}

}

