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
	public abstract PixelFormat pixel_format { set; get; default = PixelFormat.ORGB1555; }
	public abstract Device.InputDescriptor[] input_descriptors { set; get; }
	public abstract SystemAvInfo? system_av_info { set; get; default = null; }
	
	public abstract Keyboard.Callback? keyboard_callback { set; get; default = null; }
	public abstract Disk.ControlCallback? disk_control_interface { set; get; default = null; }
	public abstract Hardware.RenderCallback? hw_render { set; get; default = null; }
	public abstract Audio? audio_callback { set; get; default = null; }
	public abstract FrameTime.Callback? frame_time_callback { set; get; default = null; }
}

}

