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
using Retro;

public class Engine : Object, CoreCallbackHandler, Runnable {
	private Core core;
	private HashTable<uint?, ControllerDevice> controller_devices;
	
	public OptionsHandler options { private set; get; default = null; }
	
	public PixelFormat pixel_format {
		get {
			return core.pixel_format;
		}
	}
	
	public SystemAvInfo? system_av_info {
		get {
			return core.system_av_info;
		}
	}
	
	public SystemInfo info { private set; get; }
	
	public signal void video_refresh (Gdk.Pixbuf pixbuf);
	public signal void audio_refresh (AudioSamples samples);
	
	public Engine (string file_name) {
		core = new Core (file_name, this);
		core.get_variable.connect ((key) => { return options[key]; });
		core.set_variables.connect ((variables) => {
			options.insert_multiple (variables);
			return true;
		});
		
		options = new OptionsHandler ();
		options.value_changed.connect (() => {
			core.variable_update = true;
		});
		
		controller_devices = new HashTable<int?, ControllerDevice> (int_hash, int_equal);
		
		info = core.system_info;
	}
	
	[CCode (cname = "video_to_pixbuf", cheader_filename="video_converter.h")]
	static extern Gdk.Pixbuf video_to_pixbuf ([CCode (array_length = false)] uint8[] data, uint width, uint height, size_t pitch, PixelFormat pixel_format);
	
	private void video_refresh_cb (uint8[] data, uint width, uint height, size_t pitch) {
		var pixbuf = video_to_pixbuf (data, width, height,  pitch, pixel_format);
		video_refresh (pixbuf);
	}
	
	private void audio_sample_cb (int16 left, int16 right) {
		var audio_samples = new AudioSamples.from_sample (left, right, system_av_info.timing.sample_rate);
		audio_refresh (audio_samples);
	}
	
	private size_t audio_sample_batch_cb (int16[] data, size_t frames) {
		var audio_samples = new AudioSamples (data, system_av_info.timing.sample_rate);
		audio_refresh (audio_samples);
		return 0;
	}
	
	private void input_poll_cb () {
		foreach (var device in controller_devices.get_values ()) {
			if (device != null) device.poll ();
		}
	}
	
	private int16 input_state_cb (uint port, DeviceType device, uint index, uint id) {
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
	
	public void set_controller_device (uint port, ControllerDevice device) {
		if (controller_devices.contains (port)) {
			controller_devices.replace (port, device);
		}
		else {
			controller_devices.insert (port, device);
		}
		
		core.set_controller_port_device (port, Retro.DeviceType.JOYPAD);
	}
	
	public void remove_controller_device (uint port) {
		if (controller_devices.contains (port)) {
			controller_devices.remove (port);
		}
		
		core.set_controller_port_device (port, DeviceType.NONE);
	}
	
	/**
	 * Resets the current game.
	 */
	public void reset () {
		core.reset ();
	}
	
	/**
	 * Runs the game for one video frame.
	 */
	public void run () {
		core.run ();
	}
	
	/**
	 * Runs the game for one video frame.
	 */
	public double get_frames_per_second () {
		return system_av_info.timing.fps;
	}
	
	public void load_game (GameInfo game) {
		core.load_game (game);
	}
}

