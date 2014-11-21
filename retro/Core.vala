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
	 * Gets region of the loaded game.
	 */
	public Region region {
		get {
			set_cb_data ();
			return game_loaded ? module.get_region () : Region.UNKNOWN;
		}
	}
	
	// Implementation of environment properties
	
	public bool overscan { set; get; default = true; }
	public bool can_dupe { set; get; default = false; }
	public string system_directory { set; get; default = "."; }
	public string libretro_path { set; get; default = "."; }
	public string content_directory { set; get; default = "."; }
	public string save_directory { set; get; default = "."; }
	
	public bool variable_update { set; get; default = false; }
	
	public Rotation rotation { protected set; get; default = Rotation.NONE; }
	public bool support_no_game { protected set; get; default = false; }
	public PerfLevel performance_level { protected set; get; }
	public PixelFormat pixel_format { protected set; get; default = PixelFormat.ORGB1555; }
	public InputDescriptor[] input_descriptors { protected set; get; }
	public SystemAvInfo? system_av_info { protected set; get; default = null; }
	
	public Keyboard.Callback? keyboard_callback { protected set; get; default = null; }
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
	private extern void set_cb_data ();
	
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
	
	public CoreCallbackHandler cb_handler { get; construct; }
	
	
	
	
	
	/**
	 * Create a Core from the file name of a libretro implementation.
	 * 
	 * The file must be a dynamically loadable shared object implementing the
	 * same version of the libretro API as Retro.
	 * 
	 * @param file_name the file name of the libretro implementation to load
	 */
	public Core (string file_name, CoreCallbackHandler cb_handler) {
		Object (file_name: file_name, cb_handler: cb_handler);
	}
	
	construct {
		module = new Module (file_name);
		
		set_cb_data ();
		module.set_video_refresh (get_module_video_refresh_cb ());
		module.set_audio_sample (get_module_audio_sample_cb ());
		module.set_audio_sample_batch (get_module_audio_sample_batch_cb ());
		module.set_input_poll (get_module_input_poll_cb ());
		module.set_input_state (get_module_input_state_cb ());
		
		init ();
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
		set_cb_data ();
		module.set_environment (get_module_environment_interface ());
		module.init ();
	}
	
	/**
	 * Deinitialize the module.
	 */
	private void deinit () {
		set_cb_data ();
		module.deinit ();
	}
	
	/**
	 * The version of libretro used by the module.
	 * 
	 * Can be compared with {@link API_VERSION} to validate ABI
	 * compatibility.
	 * 
	 * @return the libretro version of the module
	 */
	public uint api_version () {
		set_cb_data ();
		return module.api_version ();
	}
	
	/**
	 * Gets system information.
	 * 
	 * Can be called at any time, even before {@link init}.
	 * 
	 * @return information on the system implemented in the module
	 */
	public SystemInfo get_system_info () {
		set_cb_data ();
		
		SystemInfo info;
		module.get_system_info (out info);
		return info;
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
	private void set_system_av_info (bool valid) {
		set_cb_data ();
		if (valid) {
			SystemAvInfo info;
			module.get_system_av_info (out info);
			system_av_info = info;
			
		}
		else {
			system_av_info = null;
		}
	}
	
	/**
	 * Sets device to be used for player 'port'.
	 * 
	 * @param port the port on wich to connect a device
	 * @param device the type of the device connected
	 */
	public void set_controller_port_device (uint port, DeviceType device) {
		set_cb_data ();
		module.set_controller_port_device (port, device);
	}
	
	/**
	 * Resets the current game.
	 */
	public void reset () {
		set_cb_data ();
		module.reset ();
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
		set_cb_data ();
		module.run ();
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
		set_cb_data ();
		return module.serialize_size ();
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
		set_cb_data ();
		return module.serialize (out data);
	}
	
	/**
	 * Unserializes the internal state.
	 * 
	 * @param data the buffer where the data is stored
	 * @return false if the unserialization failed, true otherwise
	 */
	public bool unserialize ([CCode (array_length_type = "gsize")] uint8[] data) {
		set_cb_data ();
		return module.unserialize (data);
	}
	
	/**
	 * Resets the cheats.
	 */
	[Deprecated (since = "1.0")]
	public void cheat_reset () {
		set_cb_data ();
		module.cheat_reset ();
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
		set_cb_data ();
		module.cheat_set (index, enabled, code);
	}
	
	/**
	 * Loads a game.
	 * 
	 * @param game information to load the game
	 * @return false if the loading failed, true otherwise
	 */
	public bool load_game (GameInfo game) {
		if (game_loaded) unload_game ();
		
		set_cb_data ();
		game_loaded = module.load_game (game);
		
		set_system_av_info (game_loaded);
		
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
		
		set_cb_data ();
		game_loaded = module.load_game_special (game_type, info);
		
		set_system_av_info (game_loaded);
		
		return game_loaded;
	}
	
	/**
	 * Unloads a currently loaded game.
	 */
	private void unload_game () {
		set_cb_data ();
		module.unload_game ();
	}
	
	/**
	 * Gets a region of memory.
	 * 
	 * @param id the region of memory
	 * @return the region of memory
	 */
	public uint8[] get_memory (MemoryType id) {
		set_cb_data ();
		var data = (uint8[]) module.get_memory_data (id);
		data.length = (int) module.get_memory_size (id);
		return data;
	}
}

}
