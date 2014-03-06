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

namespace Retro {

class Engine : Core, Runnable {
	public Gdk.PixbufRotation rotation { private set; get; default = Gdk.PixbufRotation.NONE; }
	public bool overscan { set; get; default = true; }
	public bool can_dupe { set; get; default = false; }
	
	public Video.PixelFormat pixel_format;
	
	private bool option_changed;
	private OptionsHandler options;
	
	private SystemAvInfo av_info;
	private HashTable<uint?, ControllerDevice> controller_devices;
	
	public signal void video_refresh (Gdk.Pixbuf pixbuf);
	public signal void audio_refresh (AudioSamples samples, double sample_rate);
	
	public Engine (string file_name) {
		base (file_name);
		pixel_format = Video.PixelFormat.UNKNOWN;
		
		option_changed = false;
		options = new OptionsHandler ();
		options.value_changed.connect (() => {
			option_changed = true;
		});
		
		environment_cb        = on_environment_cb;
		video_refresh_cb      = on_video_refresh_cb;
		audio_sample_cb       = on_audio_sample_cb;
		audio_sample_batch_cb = on_audio_sample_batch_cb;
		input_poll_cb         = on_input_poll_cb;
		input_state_cb        = on_input_state_cb;
		
		av_info = get_system_av_info ();
		
		stdout.printf ("sample_rate: %lf\n", av_info.timing.sample_rate);
		stdout.printf ("fps: %lf\n", av_info.timing.fps);
		
		controller_devices = new HashTable<int?, ControllerDevice> (int_hash, int_equal);
	}
	
	private bool on_environment_cb (Retro.Environment.Command cmd, void *data) {
		switch (cmd) {
			case Retro.Environment.Command.SET_ROTATION:
				rotation = *((Gdk.PixbufRotation *) data);
				// TODO annoncer le changement d'une valeur
				break;
			case Retro.Environment.Command.GET_OVERSCAN:
				*((bool *) data) = overscan;
				break;
			
			case Retro.Environment.Command.GET_CAN_DUPE:
				*((bool *) data) = can_dupe;
				break;
			
			case Retro.Environment.Command.SET_MESSAGE:
				stdout.printf ("on_environment_cb: SET_MESSAGE\n");
				// TODO
				break;
			case Retro.Environment.Command.SHUTDOWN:
				stdout.printf ("on_environment_cb: SHUTDOWN\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_PERFORMANCE_LEVEL:
				stdout.printf ("on_environment_cb: SET_PERFORMANCE_LEVEL\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_SYSTEM_DIRECTORY:
				*((char **) data) = "/home/kekun/nestopia";
				// TODO
				break;
			case Retro.Environment.Command.SET_PIXEL_FORMAT:
				pixel_format = *((Video.PixelFormat *) data);
				// TODO annoncer le changement d'une valeur
				break;
			case Retro.Environment.Command.SET_INPUT_DESCRIPTORS:
				stdout.printf ("on_environment_cb: SET_INPUT_DESCRIPTORS\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_KEYBOARD_CALLBACK:
				stdout.printf ("on_environment_cb: SET_KEYBOARD_CALLBACK\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_DISK_CONTROL_INTERFACE:
				stdout.printf ("on_environment_cb: SET_DISK_CONTROL_INTERFACE\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_HW_RENDER:
				stdout.printf ("on_environment_cb: SET_HW_RENDER\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_VARIABLE:
				/* data is an unowned Variable *
				 * Its key is the key that the core want to get back.
				 * Its value must be set to the one stored in the variable handler.
				 */
				unowned Variable *variable = (Variable *) data;
				Retro.Environment.get_variable (data, options[variable->key]);
				
				break;
			
			case Retro.Environment.Command.SET_VARIABLES:
				var variables = Retro.Environment.data_to_variable_array (data);
				options.insert_multiple (variables);
				break;
			
			case Retro.Environment.Command.GET_VARIABLE_UPDATE:
				*((bool *) data) = option_changed;
				option_changed = false;
				break;
			
			case Retro.Environment.Command.SET_SUPPORT_NO_GAME:
				stdout.printf ("on_environment_cb: SET_SUPPORT_NO_GAME\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_LIBRETRO_PATH:
				stdout.printf ("on_environment_cb: GET_LIBRETRO_PATH\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_AUDIO_CALLBACK:
				stdout.printf ("on_environment_cb: SET_AUDIO_CALLBACK\n");
				// TODO
				break;
			case Retro.Environment.Command.SET_FRAME_TIME_CALLBACK:
				stdout.printf ("on_environment_cb: SET_FRAME_TIME_CALLBACK\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_RUMBLE_INTERFACE:
				stdout.printf ("on_environment_cb: GET_RUMBLE_INTERFACE\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_INPUT_DEVICE_CAPABILITIES:
				stdout.printf ("on_environment_cb: GET_INPUT_DEVICE_CAPABILITIES\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_SENSOR_INTERFACE:
				stdout.printf ("on_environment_cb: GET_SENSOR_INTERFACE\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_CAMERA_INTERFACE:
				stdout.printf ("on_environment_cb: GET_CAMERA_INTERFACE\n");
				// TODO
				break;
			case Retro.Environment.Command.GET_LOG_INTERFACE:
				stdout.printf ("on_environment_cb: GET_LOG_INTERFACE\n");
				//((Log.Callback *) data)->log = (Log.Printf) Log.printf;
				*((void **) data) = null;
				// TODO
				break;
			case Retro.Environment.Command.GET_PERF_INTERFACE:
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
		var audio_samples = new AudioSamples.from_sample (left, right);
		audio_refresh (audio_samples, av_info.timing.sample_rate);
	}
	
	private size_t on_audio_sample_batch_cb (int16[] data, size_t frames) {
		var audio_samples = new AudioSamples (data);
		audio_refresh (audio_samples, av_info.timing.sample_rate);
		return 0;
	}
	
	private void on_input_poll_cb () {
		foreach (var device in controller_devices.get_values ()) {
			if (device != null) device.poll ();
		}
	}
	
	private int16 on_input_state_cb (uint port, uint device, uint index, uint id) {
		if (controller_devices.contains (port)) {
			var controller_device = controller_devices.lookup (port);
			if (controller_device != null) {
				return controller_device.get_state (device, index, id);
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
		
		set_controller_port_device (port, device.get_device_type ());
	}
	
	public void remove_controller_device (uint port) {
		if (controller_devices.contains (port)) {
			controller_devices.remove (port);
		}
		
		set_controller_port_device (port, Device.Type.NONE);
	}
	
	public double get_iterations_per_second () {
		var info = get_system_av_info ();
		
		return info.timing.fps;
	}
}

}
