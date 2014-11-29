/* Copyright (C) 2014  Adrien Plazas
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

public class Core : Object {
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
	 * The system informations
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
	 * The region of the loaded game
	 */
	public Region region {
		get {
			push_cb_data ();
			var result = game_loaded ? module.get_region () : Region.UNKNOWN;
			pop_cb_data ();
			return result;
		}
	}

	/**
	 * Whether the module have been copied or not
	 */
	public bool copy { construct; private get; }

	/**
	 * The file name of the loaded module
	 */
	public string file_name { construct; get; }

	/**
	 * The directory the core will use to look for for additional data
	 */
	public string system_directory { set; get; default = "."; }

	/**
	 * The absolute path to the source module file
	 */
	public string libretro_path { set; get; default = "."; }

	/**
	 * The directory the core will use to look for for additional assets
	 */
	public string content_directory { set; get; default = "."; }

	/**
	 * The directory the core will use to save user data
	 */
	public string save_directory { set; get; default = "."; }

	/**
	 * Whether or not the a game is loaded
	 */
	public bool game_loaded { private set; get; default = false; }

	/**
	 * Whether or not the core supports games
	 */
	public bool support_no_game { internal set; get; default = false; }

	/**
	 * The level of performance requiered by the core to run correctly
	 *
	 * Can be set by the Core when loading a game.
	 */
	public PerfLevel performance_level { internal set; get; }

	/**
	 * Information on audio and video geometry and timings
	 *
	 * Can be set by the Core when loading a game.
	 */
	public AvInfo av_info { internal set; get; }

	/**
	 * The disk controlling interface
	 *
	 * The Core can set it to let the frontend insert and eject disks.
	 */
	public DiskController disk_control_interface { internal set; get; }

	/**
	 * The video input interface
	 *
	 * The Core can set it to let the frontend pass video to it.
	 */
	public HardwareRender hw_render { internal set; get; }

	/**
	 * The audio input interface
	 *
	 * The Core can set it to let the frontend pass audio to it.
	 */
	public AudioInput audio_input_callback { internal set; get; }

	/**
	 * The time input interface
	 *
	 * The Core can set it to let the frontend inform it of the amount
	 * of time passed since the last call to {@link run()}.
	 */
	public FrameTime frame_time_callback { internal set; get; }

	/**
	 * The video interface
	 *
	 * It must be set before {@link init()} is called.
	 */
	private Video _video_interface;
	public Video video_interface {
		get { return _video_interface; }
		construct set {
			if (_video_interface != null)
				_video_interface.core = null;

			_video_interface = value;

			if (_video_interface != null && _video_interface.core != this)
				_video_interface.core = this;
		}
	}

	/**
	 * The audio interface
	 *
	 * It must be set before {@link init()} is called.
	 */
	private AudioInterface _audio_interface;
	public AudioInterface audio_interface {
		get { return _audio_interface; }
		construct set {
			if (_audio_interface != null)
				_audio_interface.core = null;

			_audio_interface = value;

			if (_audio_interface != null && _audio_interface.core != this)
				_audio_interface.core = this;
		}
	}

	/**
	 * The input interface
	 *
	 * It must be set before {@link init()} is called.
	 */
	private InputInterface _input_interface;
	public InputInterface input_interface {
		get { return _input_interface; }
		construct set {
			if (_input_interface != null)
				_input_interface.core = null;

			_input_interface = value;

			if (_input_interface != null && _input_interface.core != this)
				_input_interface.core = this;
		}
	}

	/**
	 * The variables interface
	 *
	 * Optional.
	 * If set, it must be set before {@link init()} is called.
	 */
	private VariablesInterface _variables_interface;
	public VariablesInterface variables_interface {
		get { return _variables_interface; }
		construct set {
			if (_variables_interface != null)
				_variables_interface.core = null;

			_variables_interface = value;

			if (_variables_interface != null && _variables_interface.core != this)
				_variables_interface.core = this;
		}
	}

	/**
	 * The rumble interface
	 *
	 * Optional.
	 * If set, it must be set before {@link init()} is called.
	 */
	public Rumble rumble_interface { set; get; }

	/**
	 * The sensor interface
	 *
	 * Optional.
	 * If set, it must be set before {@link init()} is called.
	 */
	public Sensor sensor_interface { set; get; }

	/**
	 * The camera interface
	 *
	 * Optional.
	 * If set, it must be set before {@link init()} is called.
	 */
	public Camera camera_interface { set; get; }

	/**
	 * The logging interface
	 *
	 * Optional.
	 * If set, it must be set before {@link init()} is called.
	 */
	public Log log_interface { set; get; }

	/**
	 * The performance interface
	 *
	 * Optional.
	 * If set, it must be set before {@link init()} is called.
	 */
	public Performance performance_interface { set; get; }

	/**
	 * The location interface
	 *
	 * Optional.
	 * If set, it must be set before {@link init()} is called.
	 */
	public Location location_interface { set; get; }

	/**
	 * Ask the frontend to shut down
	 */
	public signal bool shutdown ();

	/**
	 * Ask the frontend to display a message for an amount of frames
	 */
	public signal bool message (string message, uint frames);

	/**
	 * Store the current Core instance in a stack
	 *
	 * Stores the current instance of Core in a thread local global stack.
	 * It allows to know wich Core a callback is related to.
	 *
	 * Must be called before any call to a function from the module.
	 */
	private extern void push_cb_data ();

	/**
	 * Remove the Core at the head of the stack
	 *
	 * Must be called after any call to {@link push_cb_data()}.
	 */
	private extern void pop_cb_data ();

	private extern void *get_module_environment_interface ();
	private extern void *get_module_video_refresh_cb ();
	private extern void *get_module_audio_sample_cb ();
	private extern void *get_module_audio_sample_batch_cb ();
	private extern void *get_module_input_poll_cb ();
	private extern void *get_module_input_state_cb ();

	private Module module;

	/**
	 * Create a Core from the file name of a libretro implementation
	 *
	 * The file must be a dynamically loadable shared object implementing the
	 * same version of the libretro API as Retro.
	 *
	 * The module can be copied before being loaded to avoid clash on the
	 * module's static variables.
	 *
	 * @param file_name the file name of the libretro implementation to load
	 * @param copy_module wheter the module should be copied or not
	 */
	public Core (string file_name, bool copy_module = false) {
		Object (file_name: file_name, copy: copy_module);
	}

	construct {
		libretro_path = File.new_for_path (file_name).resolve_relative_path ("").get_path ();

		module = new Module (file_name, copy);

		file_name = module.file_name;

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
	 * Initialize the module
	 *
	 * Must be called before loading a game and running the core.
	 */
	public void init () {
		push_cb_data ();
		module.set_environment (get_module_environment_interface ());
		module.init ();
		pop_cb_data ();
	}

	/**
	 * Deinitialize the module
	 */
	private void deinit () {
		push_cb_data ();
		module.deinit ();
		pop_cb_data ();
	}

	/**
	 * Get information about system audio/video timings and geometry
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
	 * Set device to be used for player 'port'
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
	 * Reset the current game
	 */
	public void reset () {
		push_cb_data ();
		module.reset ();
		pop_cb_data ();
	}

	/**
	 * Run the game for one video frame
	 *
	 * The callbacks must be set and the core must be initialized before
	 * running the core.
	 *
	 * During {@link run}, the input_poll callback will be called
	 * at least once.
	 *
	 * If a frame is not rendered for reasons where a game "dropped" a frame,
	 * this still counts as a frame, and {@link run} will explicitly dupe a
	 * frame if the can_dupe property of {@link video_interface} is set to true.
	 * In this case, the video callback can take a null argument for data.
	 */
	public void run () {
		push_cb_data ();
		module.run ();
		pop_cb_data ();
	}

	/**
	 * Return the amount of data the implementation requires to serialize the
	 * internal state (save states)
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
	 * Serialize the internal state
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
	 * Unserialize the internal state
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
	 * Reset the cheats
	 */
	public void cheat_reset () {
		push_cb_data ();
		module.cheat_reset ();
		pop_cb_data ();
	}

	/**
	 * Set a new cheat
	 *
	 * @param index the index of the cheat
	 * @param enabled whereas the cheat is enabled or not
	 * @param code the cheat code
	 */
	public void cheat_set (uint index, bool enabled, string code) {
		push_cb_data ();
		module.cheat_set (index, enabled, code);
		pop_cb_data ();
	}

	/**
	 * Load a game
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
	 * Load a "special" kind of game. Should not be used except in extreme
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
	 * Unload a currently loaded game
	 */
	private void unload_game () {
		push_cb_data ();
		module.unload_game ();
		pop_cb_data ();
	}

	/**
	 * Get a region of memory
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
