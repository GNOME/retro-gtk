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

/**
 * The version of libretro implemented by Retro.
 */
public static const uint API_VERSION = 1;

/**
 * Handle a libretro module.
 *
 * Core can load a libretro module and handle is isolated from other cores.
 * In contrary to what the libretro API allows, multiple Cores can live in
 * the same process.
 */
public class Core : Object, Environment {
	/**
	 * The version of libretro used by the module
	 *
	 * Can be compared with {@link API_VERSION} to validate ABI
	 * compatibility.
	 */
	public uint api_version {
		get {
			push_cb_data ();
			var result = module.api_version ();
			pop_cb_data ();
			return result;
		}
	}

	/**
	 * Gets system information.
	 */
	public SystemInfo system_info {
		get {
			push_cb_data ();
			unowned SystemInfo info;
			module.get_system_info (out info);
			pop_cb_data ();
			return info;
		}
	}

	/**
	 * Gets region of the loaded game.
	 */
	public Region region {
		get {
			push_cb_data ();
			var result = game_loaded ? module.get_region () : Region.UNKNOWN;
			pop_cb_data ();
			return result;
		}
	}

	// Implementation of environment properties

	public string system_directory { set; get; default = "."; }
	public string libretro_path { set; get; default = "."; }
	public string content_directory { set; get; default = "."; }
	public string save_directory { set; get; default = "."; }

	public bool support_no_game { protected set; get; default = false; }
	public PerfLevel performance_level { protected set; get; }
	public AvInfo av_info { private set; get; }

	public DiskController? disk_control_interface { protected set; get; default = null; }
	public HardwareRender? hw_render { protected set; get; default = null; }
	public AudioInput? audio_input_callback { protected set; get; default = null; }
	public FrameTime? frame_time_callback { protected set; get; default = null; }

	public Rumble rumble_interface { set; get; default = null; }
	public Sensor sensor_interface { set; get; default = null; }
	public Camera camera_interface { set; get; default = null; }
	public Log log_interface { set; get; default = null; }
	public Performance performance_interface { set; get; default = null; }
	public Location location_interface { set; get; default = null; }





	// Helper C methods

	/**
	 * Store the current instance.
	 *
	 * Store the current instance of Core in a thread local global variable.
	 * It allows to know wich module call back and associate it with its Core.
	 *
	 * Must be called before any call to a function from the module.
	 */
	private extern void push_cb_data ();
	private extern void pop_cb_data ();

	/*
	 * Get a callback that can be passed to the module.
	 *
	 * These callbacks act like wrappers around the real callbacks.
	 */
	private extern void *get_module_environment_interface ();
	private extern void *get_module_video_refresh_cb ();
	private extern void *get_module_audio_sample_cb ();
	private extern void *get_module_audio_sample_batch_cb ();
	private extern void *get_module_input_poll_cb ();
	private extern void *get_module_input_state_cb ();





	// Various members

	/**
	 * The file name of the module.
	 */
	public string file_name { construct; get; }

	/**
	 * The dynamically loaded libretro module.
	 */
	private Module module;

	/**
	 * Whether or not the a game is loaded.
	 */
	public bool game_loaded { private set; get; default = false; }

	private VideoHandler _video_handler;
	public VideoHandler video_handler {
		get { return _video_handler; }
		construct set {
			if (_video_handler != null)
				_video_handler.core = null;

			_video_handler = value;

			if (_video_handler != null && _video_handler.core != this)
				_video_handler.core = this;
		}
	}

	private AudioHandler _audio_handler;
	public AudioHandler audio_handler {
		get { return _audio_handler; }
		construct set {
			if (_audio_handler != null)
				_audio_handler.core = null;

			_audio_handler = value;

			if (_audio_handler != null && _audio_handler.core != this)
				_audio_handler.core = this;
		}
	}

	private InputHandler _input_handler;
	public InputHandler input_handler {
		get { return _input_handler; }
		construct set {
			if (_input_handler != null)
				_input_handler.core = null;

			_input_handler = value;

			if (_input_handler != null && _input_handler.core != this)
				_input_handler.core = this;
		}
	}

	private VariablesHandler _variables_handler;
	public VariablesHandler variables_handler {
		get { return _variables_handler; }
		construct set {
			if (_variables_handler != null)
				_variables_handler.core = null;

			_variables_handler = value;

			if (_variables_handler != null && _variables_handler.core != this)
				_variables_handler.core = this;
		}
	}





	/**
	 * Create a Core from the file name of a libretro implementation.
	 *
	 * The file must be a dynamically loadable shared object implementing the
	 * same version of the libretro API as Retro.
	 *
	 * @param file_name the file name of the libretro implementation to load
	 */
	public Core (string file_name) {
		Object (file_name: file_name);
	}

	construct {
		module = new Module (file_name);

		push_cb_data ();
		module.set_video_refresh (get_module_video_refresh_cb ());
		module.set_audio_sample (get_module_audio_sample_cb ());
		module.set_audio_sample_batch (get_module_audio_sample_batch_cb ());
		module.set_input_poll (get_module_input_poll_cb ());
		module.set_input_state (get_module_input_state_cb ());
		pop_cb_data ();
	}

	~Core () {
		if (game_loaded) unload_game ();
		deinit ();
	}

	/**
	 * Initialize the module.
	 *
	 * {@link environment_interface} must be set before the module is
	 * initialized.
	 */
	public void init () {
		push_cb_data ();
		module.set_environment (get_module_environment_interface ());
		module.init ();
		pop_cb_data ();
	}

	/**
	 * Deinitialize the module.
	 */
	private void deinit () {
		push_cb_data ();
		module.deinit ();
		pop_cb_data ();
	}

	/**
	 * Gets information about system audio/video timings and geometry.
	 *
	 * Can be called only after {@link load_game} has successfully
	 * completed.
	 *
	 * NOTE: The implementation of this function might not initialize every
	 * variable if needed.
	 * E.g. geometry.aspect_ratio might not be initialized if the core doesn't
	 * desire a particular aspect ratio.
	 *
	 * @param valid whether the av_info is valid or not
	 * @return information on the system audio/video timings and geometry
	 */
	private void update_av_info (bool valid) {
		push_cb_data ();
		if (valid) {
			SystemAvInfo info;
			module.get_system_av_info (out info);
			av_info = new AvInfo (info);
		}
		else {
			av_info = null;
		}
		pop_cb_data ();
	}

	/**
	 * Sets device to be used for player 'port'.
	 *
	 * @param port the port on wich to connect a device
	 * @param device the type of the device connected
	 */
	public void set_controller_port_device (uint port, DeviceType device) {
		push_cb_data ();
		module.set_controller_port_device (port, device);
		pop_cb_data ();
	}

	/**
	 * Resets the current game.
	 */
	public void reset () {
		push_cb_data ();
		module.reset ();
		pop_cb_data ();
	}

	/**
	 * Runs the game for one video frame.
	 *
	 * The callbacks must be set and the core must be initialized before
	 * running the core.
	 *
	 * During {@link run}, the input_poll callback will be called
	 * at least once.
	 *
	 * If a frame is not rendered for reasons where a game "dropped" a frame,
	 * this still counts as a frame, and {@link run} will explicitly dupe a
	 * frame if the can_dupe property of {@link environment_interface} is set to true.
	 * In this case, the video callback can take a null argument for data.
	 */
	public void run () {
		push_cb_data ();
		module.run ();
		pop_cb_data ();
	}

	/**
	 * Returns the amount of data the implementation requires to serialize the
	 * internal state (save states).
	 *
	 * Beetween calls to {@link load_game} and
	 * {@link unload_game}, the returned size is never allowed to
	 * be larger than a previous returned value, to ensure that the frontend can
	 * allocate a save state buffer once.
	 *
	 * @return the size needed to serialize the internal state
	 */
	public size_t serialize_size () {
		push_cb_data ();
		var result = module.serialize_size ();
		pop_cb_data ();

		return result;
	}

	/**
	 * Serializes the internal state.
	 *
	 * If failed, or size is lower than {@link serialize_size}, it
	 * should return false, true otherwise.
	 *
	 * @param data the buffer where the data will be stored
	 * @return false if the serialization failed, true otherwise
	 */
	public bool serialize ([CCode (array_length_type = "gsize")] out uint8[] data) {
		push_cb_data ();
		var result = module.serialize (out data);
		pop_cb_data ();

		return result;
	}

	/**
	 * Unserializes the internal state.
	 *
	 * @param data the buffer where the data is stored
	 * @return false if the unserialization failed, true otherwise
	 */
	public bool unserialize ([CCode (array_length_type = "gsize")] uint8[] data) {
		push_cb_data ();
		var result = module.unserialize (data);
		pop_cb_data ();

		return result;
	}

	/**
	 * Resets the cheats.
	 */
	[Deprecated (since = "1.0")]
	public void cheat_reset () {
		push_cb_data ();
		module.cheat_reset ();
		pop_cb_data ();
	}

	/**
	 * Sets a new cheat.
	 *
	 * @param index the index of the cheat
	 * @param enabled whereas the cheat is enabled or not
	 * @param code the cheat code
	 */
	[Deprecated (since = "1.0")]
	public void cheat_set (uint index, bool enabled, string code) {
		push_cb_data ();
		module.cheat_set (index, enabled, code);
		pop_cb_data ();
	}

	/**
	 * Loads a game.
	 *
	 * @param game information to load the game
	 * @return false if the loading failed, true otherwise
	 */
	public bool load_game (GameInfo game) {
		if (game_loaded) unload_game ();

		push_cb_data ();
		game_loaded = module.load_game (game);
		update_av_info (game_loaded);
		pop_cb_data ();

		return game_loaded;
	}

	/**
	 * Loads a "special" kind of game. Should not be used except in extreme
	 * cases.
	 *
	 * @param game_type the type of game to load
	 * @param info the informations to load the game
	 * @return false if the loading failed, true otherwise
	 */
	public bool load_game_special (GameType game_type, [CCode (array_length_type = "gsize")] GameInfo[] info) {
		if (game_loaded) unload_game ();

		push_cb_data ();
		game_loaded = module.load_game_special (game_type, info);
		update_av_info (game_loaded);
		pop_cb_data ();

		return game_loaded;
	}

	/**
	 * Unloads a currently loaded game.
	 */
	private void unload_game () {
		push_cb_data ();
		module.unload_game ();
		pop_cb_data ();
	}

	/**
	 * Gets a region of memory.
	 *
	 * @param id the region of memory
	 * @return the region of memory
	 */
	public uint8[] get_memory (MemoryType id) {
		push_cb_data ();
		var data = (uint8[]) module.get_memory_data (id);
		data.length = (int) module.get_memory_size (id);
		pop_cb_data ();

		return data;
	}
}

}
