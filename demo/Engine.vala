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

public class Engine : Object, VideoHandler, AudioHandler, Runnable {
	private Core core;
	private HashTable<uint?, ControllerDevice> controller_devices;
	
	public OptionsHandler options { private set; get; default = null; }
	public ControllerHandler controller_handler { private set; get; }
	
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
		controller_handler = new ControllerHandler ();

		core = new Core (file_name, this, this, controller_handler);
		core.get_variable.connect ((key) => { return options[key]; });
		core.set_variables.connect ((variables) => {
			options.insert_multiple (variables);
			return true;
		});

		controller_handler.core = core;

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
		var info = system_av_info;
		
		if (info == null) {
			stderr.printf ("Error: system_av_info null");
			return 60;
		}
		
		return info.timing.fps;
	}
	
	public void load_game (GameInfo game) {
		core.load_game (game);
	}
}

