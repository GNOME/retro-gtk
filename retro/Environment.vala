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
	public signal bool shutdown ();
	public signal bool message (Message message);
	
	public abstract bool overscan { set; get; default = true; }
	public abstract bool can_dupe { set; get; default = false; }
	public signal uint64 get_input_device_capabilities ();
	public abstract string system_directory { set; get; default = "."; }
	public abstract string libretro_path { set; get; default = "."; }
	public abstract string content_directory { set; get; default = "."; }
	public abstract string save_directory { set; get; default = "."; }
	
	public signal string? get_variable (string key);
	public abstract bool variable_update { set; get; default = false; }
	public signal bool set_variables (Variable[] variables);
	
	public abstract Rotation rotation { protected set; get; default = Rotation.NONE; }
	public abstract bool support_no_game { protected set; get; default = false; }
	public abstract PerfLevel performance_level { protected set; get; }
	public abstract PixelFormat pixel_format { protected set; get; default = PixelFormat.ORGB1555; }
	public abstract Device.InputDescriptor[] input_descriptors { protected set; get; }
	public abstract SystemAvInfo? system_av_info { protected set; get; default = null; }
	
	public abstract Keyboard.Callback? keyboard_callback { protected set; get; default = null; }
	
	/**
	 * If set, the {@link Core} can swap out multiple disk images in runtime.
	 */
	public abstract DiskController? disk_control_interface { protected set; get; default = null; }
	public abstract HardwareRender? hw_render { protected set; get; default = null; }
	public abstract Audio? audio_callback { protected set; get; default = null; }
	public abstract FrameTime? frame_time_callback { protected set; get; default = null; }
	
	/**
	 * The rumble interface.
	 * 
	 * Optional.
	 * If set, it must be set before {@link environment_interface}.
	 */
	public abstract Rumble rumble_interface { set; get; default = null; }
	
	/**
	 * The sensor interface.
	 * 
	 * Optional.
	 * If set, it must be set before {@link environment_interface}.
	 */
	public abstract Sensor sensor_interface { set; get; default = null; }
	
	/**
	 * The camera interface.
	 * 
	 * Optional.
	 * If set, it must be set before {@link environment_interface}.
	 */
	public abstract Camera camera_interface { set; get; default = null; }
	
	/**
	 * The logging interface.
	 * 
	 * Optional.
	 * If set, it must be set before {@link environment_interface}.
	 */
	public abstract Log log_interface { set; get; default = null; }
	
	/**
	 * The performance interface.
	 * 
	 * Optional.
	 * If set, it must be set before {@link environment_interface}.
	 */
	public abstract Performance performance_interface { set; get; default = null; }
	
	/**
	 * The location interface.
	 * 
	 * Optional.
	 * If set, it must be set before {@link environment_interface}.
	 */
	public abstract Location location_interface { set; get; default = null; }
}

}

