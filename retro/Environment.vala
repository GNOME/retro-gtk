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
	
	public static void set_bool (void *data, bool value) {
		*((bool *) data) = value;
	}
	
	public static void set_uint64 (void *data, uint64 value) {
		*((uint64 *) data) = value;
	}
	
	public static extern void set_string (void *data, string value);
	public static extern void set_variable_value (void *data, string value);
	//public static extern void set_rumble_interface (void *data, Rumble.Interface value);
	//public static extern void set_sensor_interface (void *data, Sensor.Interface value);
	//public static extern void set_camera_interface (void *data, Camera.Callback value);
	//public static extern void set_log_interface (void *data, Log.Callback value);
	//public static extern void set_performanc_interface (void *data, Performance.Callback value);
	
	public static bool get_bool (void *data) {
		return *((bool *) data);
	}
	
	public static uint get_uint (void *data) {
		return *((uint *) data);
	}
	
	public static unowned Message get_message (void *data) {
		return *((Message *) data);
	}
	
	public static unowned Device.InputDescriptor[] get_input_descriptors (void *data) {
		unowned Device.InputDescriptor[] array = (Device.InputDescriptor[]) data;
		
		int i;
		for (i = 0 ; array[i].description != null ; i++);
		
		return array[0:i];
	}
	
	public static unowned Keyboard.Callback get_keyboard_callback (void *data) {
		return *((Keyboard.Callback *) data);
	}
	
	public static unowned Disk.ControlCallback get_disk_control_callback (void *data) {
		return *((Disk.ControlCallback *) data);
	}
	
	public static unowned Hardware.RenderCallback get_hardware_render_callback (void *data) {
		return *((Hardware.RenderCallback *) data);
	}
	
	public static extern unowned string get_variable_key (void *data);
	
	public static unowned Variable[] get_variables (void *data) {
		unowned Variable[] array = (Variable[]) data;
		
		int i;
		for (i = 0 ; array[i].key != null && array[i].value != null ; i++);
		
		return array[0:i];
	}
	
	public static unowned Audio.Callback get_audio_callback (void *data) {
		return *((Audio.Callback *) data);
	}
	
	public static unowned FrameTime.Callback get_frame_time_callback (void *data) {
		return *((FrameTime.Callback *) data);
	}
	
	public abstract bool shutdown ();
	
	public abstract bool get_overscan (out bool overscan);
	public abstract bool get_can_dupe (out bool can_dupe);
	public abstract bool get_input_device_capabilities (out uint64 input_device_capabilities);
	public abstract bool get_system_directory (out string system_directory);
	public abstract bool get_libretro_path (out string libretro_path);
	public abstract bool get_content_directory (out string content_directory);
	public abstract bool get_save_directory (out string save_directory);
	
	public abstract bool get_variable (ref Variable variable);
	public abstract bool get_variable_update (out bool variable_update);
	public abstract bool set_variables (Variable[] variables);
	
	public abstract bool set_rotation (Rotation rotation);
	public abstract bool set_message (Message message);
	public abstract bool set_support_no_game (bool support_no_game);
	public abstract bool set_performance_level (Performance.Level performance_level);
	public abstract bool set_pixel_format (Video.PixelFormat pixel_format);
	public abstract bool set_input_descriptors (Device.InputDescriptor[] input_descriptors);
	public abstract bool set_system_av_info (SystemAvInfo av_info);
	
	public abstract bool set_keyboard_callback (Keyboard.Callback keyboard_callback);
	public abstract bool set_disk_control_interface (Disk.ControlCallback disk_control_interface);
	public abstract bool set_hw_render (Hardware.RenderCallback hw_render);
	public abstract bool set_audio_callback (Audio.Callback audio_callback);
	public abstract bool set_frame_time_callback (FrameTime.Callback frame_time_callback);
}

}

