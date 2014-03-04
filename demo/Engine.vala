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

class Engine : Object {
	public Core core;
	
	public Gdk.PixbufRotation rotation { private set; get; default = Gdk.PixbufRotation.NONE; }
	public bool overscan { set; get; default = true; }
	public bool can_dupe { set; get; default = false; }
	public Video.PixelFormat pixel_format;
	
	private AudioDevice audio_dev;
	
	private SystemAvInfo av_info;
	
	public signal void   video_refresh         (Gdk.Pixbuf pixbuf);
	//public signal void   audio_sample       (int16 left, int16 right);
	//public signal size_t audio_sample_batch (int16 *data, size_t frames);
	public signal void   input_poll         ();
	//public signal int16  input_state        (uint port, uint device, uint index, uint id);
	
	public signal Gdk.PixbufRotation rotation_changed ();
	
	public Engine (string library_path) {
		pixel_format = Video.PixelFormat.UNKNOWN;
		
		core = new Core (library_path);
		
		core.environment_cb        = on_environment_cb;
		core.video_refresh_cb      = on_video_refresh_cb;
		core.audio_sample_cb       = on_audio_sample_cb;
		core.audio_sample_batch_cb = on_audio_sample_batch_cb;
		core.input_poll_cb         = on_input_poll_cb;
		core.input_state_cb        = on_input_state_cb;
		
		core.get_system_av_info (out av_info);
		
		stdout.printf ("sample_rate: %lf\n", av_info.timing.sample_rate);
		
		audio_dev = new AudioDevice ((uint32) av_info.timing.sample_rate);
	}
	
	private bool on_environment_cb (Environment.Command cmd, void *data) {
		stdout.printf ("on_environment_cb: %d\n", cmd);
		switch (cmd) {
			case Environment.Command.SET_ROTATION:
				rotation = *((Gdk.PixbufRotation *) data);
				// TODO annoncer le changement d'une valeur
				break;
			case Environment.Command.GET_OVERSCAN:
				*((bool *) data) = overscan;
				break;
			case Environment.Command.GET_CAN_DUPE:
				*((bool *) data) = can_dupe;
				break;
			case Environment.Command.SET_MESSAGE:
				// TODO
				break;
			case Environment.Command.SHUTDOWN:
				// TODO
				break;
			case Environment.Command.SET_PERFORMANCE_LEVEL:
				// TODO
				break;
			case Environment.Command.GET_SYSTEM_DIRECTORY:
				*((char **) data) = "/home/kekun/nestopia";
				// TODO
				break;
			case Environment.Command.SET_PIXEL_FORMAT:
				pixel_format = *((Video.PixelFormat *) data);
				// TODO annoncer le changement d'une valeur
				break;
			case Environment.Command.SET_INPUT_DESCRIPTORS:
				// TODO
				break;
			case Environment.Command.SET_KEYBOARD_CALLBACK:
				// TODO
				break;
			case Environment.Command.SET_DISK_CONTROL_INTERFACE:
				// TODO
				break;
			case Environment.Command.SET_HW_RENDER:
				// TODO
				break;
			case Environment.Command.GET_VARIABLE:
				*(((char **) data)+1) = "";
				// TODO
				break;
			case Environment.Command.SET_VARIABLES:
				// TODO
				break;
			case Environment.Command.GET_VARIABLE_UPDATE:
				// TODO
				break;
			case Environment.Command.SET_SUPPORT_NO_GAME:
				// TODO
				break;
			case Environment.Command.GET_LIBRETRO_PATH:
				// TODO
				break;
			case Environment.Command.SET_AUDIO_CALLBACK:
				// TODO
				break;
			case Environment.Command.SET_FRAME_TIME_CALLBACK:
				// TODO
				break;
			case Environment.Command.GET_RUMBLE_INTERFACE:
				// TODO
				break;
			case Environment.Command.GET_INPUT_DEVICE_CAPABILITIES:
				// TODO
				break;
			case Environment.Command.GET_SENSOR_INTERFACE:
				// TODO
				break;
			case Environment.Command.GET_CAMERA_INTERFACE:
				// TODO
				break;
			case Environment.Command.GET_LOG_INTERFACE:
				((Log.Callback *) data)->log = (Log.Printf) Log.printf;
				//*((void **) data) = null;
				// TODO
				break;
			case Environment.Command.GET_PERF_INTERFACE:
				// TODO
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
		stdout.printf ("TODO on_audio_sample_cb: %d, %d\n", left, right);
		// TODO
	}
	
	private size_t on_audio_sample_batch_cb (int16[] data) {
		audio_dev.play (data);
		return 0;
	}
	
	private void on_input_poll_cb () {
		input_poll ();
	}
	
	private int16 on_input_state_cb (uint port, uint device, uint index, uint id) {
		//input_state (port, device, index, id);
		return 0; // TODO
	}
	
	public void init () {
		core.init ();
	}
	
	public void run () {
		core.run ();
	}
	
	public bool load_game (GameInfo game) {
		return core.load_game (game);
	}
}

}
