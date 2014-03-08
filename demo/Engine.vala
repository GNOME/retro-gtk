/* Engine.vala  A simple frontend for libretro.
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

using Flicky;

namespace Retro {

class Engine : Object, Runnable {
	private Core core;
	private HashTable<uint?, ControllerDevice> controller_devices;
	
	public Gdk.PixbufRotation rotation { private set; get; default = Gdk.PixbufRotation.NONE; }
	public bool overscan { set; get; default = true; }
	public bool can_dupe { set; get; default = false; }
	public Video.PixelFormat pixel_format { private set; get; default = Video.PixelFormat.ORGB1555; }
	
	private bool option_changed;
	private OptionsHandler options;
	
	public SystemInfo info { private set; get; }
	public SystemAvInfo? av_info { private set; get; default = null; }
	
	public signal void video_refresh (Gdk.Pixbuf pixbuf);
	public signal void audio_refresh (AudioSamples samples);
	
	public Engine (string file_name) {
		core = new Core (file_name);
		pixel_format = Video.PixelFormat.UNKNOWN;
		
		option_changed = false;
		options = new OptionsHandler ();
		options.value_changed.connect (() => {
			option_changed = true;
		});
		
		core.environment_cb        = on_environment_cb;
		core.video_refresh_cb      = on_video_refresh_cb;
		core.audio_sample_cb       = on_audio_sample_cb;
		core.audio_sample_batch_cb = on_audio_sample_batch_cb;
		core.input_poll_cb         = on_input_poll_cb;
		core.input_state_cb        = on_input_state_cb;
		
		controller_devices = new HashTable<int?, ControllerDevice> (int_hash, int_equal);
		
		core.init ();
		
		info = core.get_system_info ();
	}
	
	private bool on_environment_cb (Retro.Environment.Command cmd, void *data) {
		switch (cmd) {
			case Retro.Environment.Command.SET_ROTATION:
				rotation = (Gdk.PixbufRotation) Retro.Environment.get_uint (data);
				break;
			case Retro.Environment.Command.GET_OVERSCAN:
				Retro.Environment.set_bool (data, overscan);
				break;
			case Retro.Environment.Command.GET_CAN_DUPE:
				Retro.Environment.set_bool (data, can_dupe);
				break;
			case Retro.Environment.Command.SET_MESSAGE:
				var message = Retro.Environment.get_message (data);
				stdout.printf ("on_environment_cb: SET_MESSAGE\n");
				// TODO
				break;
			case Retro.Environment.Command.SHUTDOWN:
				stdout.printf ("on_environment_cb: SHUTDOWN\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_PERFORMANCE_LEVEL:
				var performance_level = Retro.Environment.get_uint (data);
				stdout.printf ("on_environment_cb: SET_PERFORMANCE_LEVEL\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_SYSTEM_DIRECTORY:
				Retro.Environment.set_string (data, "/home/kekun/nestopia");
				stdout.printf ("on_environment_cb: GET_SYSTEM_DIRECTORY\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_PIXEL_FORMAT:
				pixel_format = (Video.PixelFormat) Retro.Environment.get_uint (data);
				break;
			case Retro.Environment.Command.SET_INPUT_DESCRIPTORS:
				var input_descriptors = Retro.Environment.get_input_descriptors (data);
				stdout.printf ("on_environment_cb: SET_INPUT_DESCRIPTORS\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_KEYBOARD_CALLBACK:
				var keyboard_callback = Retro.Environment.get_keyboard_callback (data);
				stdout.printf ("on_environment_cb: SET_KEYBOARD_CALLBACK\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_DISK_CONTROL_INTERFACE:
				var disk_control_callback = Retro.Environment.get_disk_control_callback (data);
				stdout.printf ("on_environment_cb: SET_DISK_CONTROL_INTERFACE\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_HW_RENDER:
				var disk_control_callback = Retro.Environment.get_hardware_render_callback (data);
				stdout.printf ("on_environment_cb: SET_HW_RENDER\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_VARIABLE:
				var key = Retro.Environment.get_variable_key (data);
				Retro.Environment.set_variable_value (data, options[key]);
				break;
			case Retro.Environment.Command.SET_VARIABLES:
				var variables = Retro.Environment.get_variables (data);
				options.insert_multiple (variables);
				break;
			case Retro.Environment.Command.GET_VARIABLE_UPDATE:
				Retro.Environment.set_bool (data, option_changed);
				option_changed = false;
				break;
			case Retro.Environment.Command.SET_SUPPORT_NO_GAME:
				var support_no_game = Retro.Environment.get_bool (data);
				stdout.printf ("on_environment_cb: SET_SUPPORT_NO_GAME\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_LIBRETRO_PATH:
				Retro.Environment.set_string (data, "");
				stdout.printf ("on_environment_cb: GET_LIBRETRO_PATH\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_AUDIO_CALLBACK:
				var audio_callback = Retro.Environment.get_audio_callback (data);
				stdout.printf ("on_environment_cb: SET_AUDIO_CALLBACK\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_FRAME_TIME_CALLBACK:
				var frame_time_callback = Retro.Environment.get_frame_time_callback (data);
				stdout.printf ("on_environment_cb: SET_FRAME_TIME_CALLBACK\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_RUMBLE_INTERFACE:
				//Retro.Environment.set_rumble_interface (data, value);
				stdout.printf ("on_environment_cb: GET_RUMBLE_INTERFACE\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_INPUT_DEVICE_CAPABILITIES:
				uint64 capabilities = 0;
				foreach (var device in controller_devices.get_values ()) {
					capabilities |= device.get_device_capabilities ();
				}
				Retro.Environment.set_uint64 (data, capabilities);
				break;
			case Retro.Environment.Command.GET_SENSOR_INTERFACE:
				//Retro.Environment.set_sensor_interface (data, value);
				stdout.printf ("on_environment_cb: GET_SENSOR_INTERFACE\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_CAMERA_INTERFACE:
				//Retro.Environment.set_camera_interface (data, value);
				stdout.printf ("on_environment_cb: GET_CAMERA_INTERFACE\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_LOG_INTERFACE:
				//Retro.Environment.set_log_interface (data, value);
				stdout.printf ("on_environment_cb: GET_LOG_INTERFACE\n");
				//((Log.Callback *) data)->log = (Log.Printf) Log.printf;
				*((void **) data) = null;
				// TODO
				break;
			case Retro.Environment.Command.GET_PERF_INTERFACE:
				//Retro.Environment.set_performance_interface (data, value);
				stdout.printf ("on_environment_cb: GET_PERF_INTERFACE\n");
				// TODO
				break;
			default:
				stdout.printf ("on_environment_cb: unknown command: %d\n", cmd);
				break;
		}
		return true;
	}
	
	[CCode (cname = "video_to_pixbuf", cheader_filename="video_converter.h")]
	static extern Gdk.Pixbuf video_to_pixbuf ([CCode (array_length = false)] uint8[] data, uint width, uint height, size_t pitch, Video.PixelFormat pixel_format);
	
	private void on_video_refresh_cb (uint8[] data, uint width, uint height, size_t pitch) {
		var pixbuf = video_to_pixbuf (data, width, height,  pitch, pixel_format);
		video_refresh (pixbuf);
	}
	
	private void on_audio_sample_cb (int16 left, int16 right) {
		var audio_samples = new AudioSamples.from_sample (left, right, av_info.timing.sample_rate);
		audio_refresh (audio_samples);
	}
	
	private size_t on_audio_sample_batch_cb (int16[] data, size_t frames) {
		var audio_samples = new AudioSamples (data, av_info.timing.sample_rate);
		audio_refresh (audio_samples);
		return 0;
	}
	
	private void on_input_poll_cb () {
		foreach (var device in controller_devices.get_values ()) {
			if (device != null) device.poll ();
		}
	}
	
	private int16 on_input_state_cb (uint port, Device.Type device, uint index, uint id) {
		if (controller_devices.contains (port)) {
			var controller_device = controller_devices.lookup (port);
			if (controller_device != null) {
				var capabilities = controller_device.get_device_capabilities ();
				bool is_capable = (capabilities & (1 << device)) != 0;
				if (is_capable) return controller_device.get_input_state (device, index, id);
			}
		}
		
		return 0;
	}
	
	public OptionsHandler get_options () {
		return options;
	}
	
	public void set_controller_device (uint port, ControllerDevice device) {
		if (controller_devices.contains (port)) {
			controller_devices.replace (port, device);
		}
		else {
			controller_devices.insert (port, device);
		}
		
		core.set_controller_port_device (port, Retro.Device.Type.JOYPAD);
	}
	
	public void remove_controller_device (uint port) {
		if (controller_devices.contains (port)) {
			controller_devices.remove (port);
		}
		
		core.set_controller_port_device (port, Device.Type.NONE);
	}
	
	public double get_iterations_per_second () {
		if (av_info == null) av_info = core.get_system_av_info ();
		return av_info.timing.fps;
	}
	
	public void run () {
		av_info = core.get_system_av_info ();
		core.run ();
	}
	
	public void load_game (GameInfo game) {
		core.load_game (game);
	}
}

}
