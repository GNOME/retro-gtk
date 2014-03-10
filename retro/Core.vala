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
public class Core : Object {
	// Callbacks for the libretro module
	
	/**
	 * Environment callback. 
	 * 
	 * Gives implementations a way of performing uncommon tasks. Extensible.
	 * 
	 * @return true if the command have been executed, false otherwise
	 * @param cmd the command to execute
	 * @param data an obscure data pointer, its definition changes with the command
	 */
	public delegate bool Environment (Retro.Environment.Command cmd, void *data);
	
	/**
	 * Render a frame.
	 * 
	 * Pixel format is 15-bit 0RGB1555 native endian unless changed
	 * (see {@link Retro.Environment.Command.SET_PIXEL_FORMAT}).
	 * 
	 * For performance reasons, it is highly recommended to have a frame that is
	 * packed in memory, i.e. pitch == width * byte_per_pixel.
	 * Certain graphic APIs, such as OpenGL ES, do not like textures that are
	 * not packed in memory.
	 * 
	 * @param data the frame data
	 * @param width width of the frame
	 * @param height height of the frame
	 * @param pitch length in bytes between two lines in buffer
	 */
	public delegate void VideoRefresh ([CCode (array_length_type = "gsize")] uint8[] data, uint width, uint height, size_t pitch);
	
	/**
	 * Renders a single audio frame.
	 * 
	 * Should only be used if implementation generates a single sample at a
	 * time. Format is signed 16-bit native endian.
	 * 
	 * @param left the left channel of the audio frame
	 * @param right the right channel of the audio frame
	 */
	public delegate void AudioSample (int16 left, int16 right);
	
	/**
	 * Renders multiple audio frames in one go.
	 * 
	 * One frame is defined as a sample of left and right channels, interleaved.
	 * I.e. int16[] buf = { l, r, l, r }; would be 2 frames.
	 * 
	 * Only one of the audio callbacks must ever be used.
	 * 
	 * @return the number of audio frames read
	 * @param data the audio sample batch
	 * @param frames the number of frames in the batch
	 */
	public delegate size_t AudioSampleBatch ([CCode (array_length_type = "gsize")] int16[] data, size_t frames);
	
	/**
	 * Polls input.
	 */
	public delegate void InputPoll ();
	
	/**
	 * Queries for input for player 'port'.
	 * 
	 * Device will be masked with {@link Retro.Device.TYPE_MASK}.
	 * Specialization of devices such as
	 * {@link Retro.Device.Type.JOYPAD_MULTITAP} that have been set with
	 * {@link Retro.Core.set_controller_port_device} will still use the
	 * higher level {@link Retro.Device.Type.JOYPAD} to request input.
	 * 
	 * @return the state of the input
	 * @param port the port number
	 * @param device the type of device
	 * @param index the index, its definition changes with the device
	 * @param id the id, its definition changes with the device
	 */
	public delegate int16 InputState (uint port, Device.Type device, uint index, uint id);
	
	
	
	
	
	// Types of the module's functions
	
	[CCode (has_target = false)]
	private delegate void SetCallback (void *cb);
	
	[CCode (has_target = false)]
	private delegate void Init ();
	[CCode (has_target = false)]
	private delegate void Deinit ();
	
	[CCode (has_target = false)]
	private delegate uint ApiVersion ();
	
	[CCode (has_target = false)]
	private delegate void GetSystemInfo (out unowned SystemInfo info);
	[CCode (has_target = false)]
	private delegate void GetSystemAvInfo (out unowned SystemAvInfo info);
	
	[CCode (has_target = false)]
	private delegate void SetControllerPortDevice (uint port, Device.Type device);
	
	[CCode (has_target = false)]
	private delegate void Reset ();
	[CCode (has_target = false)]
	private delegate void Run ();
	
	[CCode (has_target = false)]
	private delegate size_t SerializeSize ();
	[CCode (has_target = false)]
	private delegate bool Serialize ([CCode (array_length_type = "gsize")] uint8[] data);
	[CCode (has_target = false)]
	private delegate bool Unserialize ([CCode (array_length_type = "gsize")] uint8[] data);
	
	[CCode (has_target = false)]
	private delegate void CheatReset ();
	[CCode (has_target = false)]
	private delegate void CheatSet (uint index, bool enabled, string code);
	
	[CCode (has_target = false)]
	private delegate bool LoadGame (GameInfo game);
	[CCode (has_target = false)]
	private delegate bool LoadGameSpecial (GameType game_type, [CCode (array_length_type = "gsize")] GameInfo[] info);
	[CCode (has_target = false)]
	private delegate void UnloadGame ();
	
	[CCode (has_target = false)]
	private delegate Region GetRegion ();
	
	[CCode (has_target = false)]
	private delegate void *GetMemoryData (Memory.Type id);
	[CCode (has_target = false)]
	private delegate size_t GetMemorySize (Memory.Type id);
	
	
	
	
	
	// Helper C methods
	
	/**
	 * Store the current instance.
	 * 
	 * Store the current instance of Core in a thread local global variable.
	 * It allows to know wich module call back and associate it with its Core.
	 * 
	 * Must be called before any call to a function from the module.
	 */
	private extern void set_global_self ();
	
	/*
	 * Get a callback that can be passed to the module.
	 * 
	 * These callbacks act like wrappers around the real callbacks.
	 */
	private extern void *get_module_environment_cb ();
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
	 * Whether or not the core have been initialized.
	 */
	public bool is_init { private set; get; default = false; }
	
	/**
	 * Whether or not the a game is loaded.
	 */
	public bool game_loaded { private set; get; default = false; }
	
	
	
	
	
	// Module's functions
	
	/*
	 * Store the functions taken from the module.
	 */
	private SetCallback _set_environment;
	private SetCallback _set_video_refresh;
	private SetCallback _set_audio_sample;
	private SetCallback _set_audio_sample_batch;
	private SetCallback _set_input_poll;
	private SetCallback _set_input_state;
	
	private Init _init;
	private Deinit _deinit;
	private ApiVersion _api_version;
	
	private GetSystemInfo _get_system_info;
	private GetSystemAvInfo _get_system_av_info;
	
	private SetControllerPortDevice _set_controller_port_device;
	
	private Reset _reset;
	private Run _run;
	
	private SerializeSize _serialize_size;
	private Serialize _serialize;
	private Unserialize _unserialize;
	
	private CheatReset _cheat_reset;
	private CheatSet _cheat_set;
	
	private LoadGame _load_game;
	private LoadGameSpecial _load_game_special;
	private UnloadGame _unload_game;
	
	private GetRegion _get_region;
	
	private GetMemoryData _get_memory_data;
	private GetMemorySize _get_memory_size;
	
	
	
	
	
	// Callback setters and getters
	
	private Environment _environment_cb;
	/**
	 * The environment callback.
	 * 
	 * Must be set before {@link Retro.Core.init} is called.
	 */
	public Environment environment_cb {
		set {
			_environment_cb = value;
			
			set_global_self ();
			_set_environment (get_module_environment_cb ());
		}
		get {
			return _environment_cb;
		}
		default = null;
	}
	
	private VideoRefresh _video_refresh_cb;
	/**
	 * The video refresh callback.
	 * 
	 * Must be set before the first call to
	 * {@link Retro.Core.run} is made.
	 */
	public VideoRefresh video_refresh_cb {
		set {
			_video_refresh_cb = value;
			
			set_global_self ();
			_set_video_refresh (get_module_video_refresh_cb ());
		}
		get {
			return _video_refresh_cb;
		}
		default = null;
	}
	
	private AudioSample _audio_sample_cb;
	/**
	 * The audio sample callback.
	 * 
	 * Must be set before the first call to
	 * {@link Retro.Core.run} is made.
	 */
	public AudioSample audio_sample_cb {
		set {
			_audio_sample_cb = value;
			
			set_global_self ();
			_set_audio_sample (get_module_audio_sample_cb ());
		}
		get {
			return _audio_sample_cb;
		}
		default = null;
	}
	
	private AudioSampleBatch _audio_sample_batch_cb;
	/**
	 * The audio sample batch callback.
	 * 
	 * Must be set before the first call to
	 * {@link Retro.Core.run} is made.
	 */
	public AudioSampleBatch audio_sample_batch_cb {
		set {
			_audio_sample_batch_cb = value;
			
			set_global_self ();
			_set_audio_sample_batch (get_module_audio_sample_batch_cb ());
		}
		get {
			return _audio_sample_batch_cb;
		}
		default = null;
	}
	
	private InputPoll _input_poll_cb;
	/**
	 * The input poll callback.
	 * 
	 * Must be set before the first call to
	 * {@link Retro.Core.run} is made.
	 */
	public InputPoll input_poll_cb {
		set {
			_input_poll_cb = value;
			
			set_global_self ();
			_set_input_poll (get_module_input_poll_cb ());
		}
		get {
			return _input_poll_cb;
		}
		default = null;
	}
	
	private InputState _input_state_cb;
	/**
	 * The input state callback.
	 * 
	 * Must be set before the first call to
	 * {@link Retro.Core.run} is made.
	 */
	public InputState input_state_cb {
		set {
			_input_state_cb = value;
			
			set_global_self ();
			_set_input_state (get_module_input_state_cb ());
		}
		get {
			return _input_state_cb;
		}
		default = null;
	}
	
	/**
	 * The rumble interface.
	 * 
	 * Must be set before the first call to
	 * {@link Retro.Core.run} is made.
	 */
	public Rumble rumble_interface { set; get; default = null; }
	
	/**
	 * The sensor interface.
	 * 
	 * Must be set before the first call to
	 * {@link Retro.Core.run} is made.
	 */
	public Sensor sensor_interface { set; get; default = null; }
	
	/**
	 * The camera interface.
	 * 
	 * Must be set before the first call to
	 * {@link Retro.Core.run} is made.
	 */
	public Camera camera_interface { set; get; default = null; }
	
	/**
	 * The logging interface.
	 * 
	 * Must be set before the first call to
	 * {@link Retro.Core.run} is made.
	 */
	public Log log_interface { set; get; default = null; }
	
	
	
	
	
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
		module = Module.open (file_name, ModuleFlags.BIND_LAZY | ModuleFlags.BIND_LOCAL);
		
		void *function;
		
		// Get the callback setters from the module
		
		module.symbol ("retro_set_environment", out function);
		_set_environment = (SetCallback) function;
		module.symbol ("retro_set_video_refresh", out function);
		_set_video_refresh = (SetCallback) function;
		module.symbol ("retro_set_audio_sample", out function);
		_set_audio_sample = (SetCallback) function;
		module.symbol ("retro_set_audio_sample_batch", out function);
		_set_audio_sample_batch = (SetCallback) function;
		module.symbol ("retro_set_input_poll", out function);
		_set_input_poll = (SetCallback) function;
		module.symbol ("retro_set_input_state", out function);
		_set_input_state = (SetCallback) function;
		
		// Get the other functions from the module
		
		module.symbol ("retro_init", out function);
		_init = (Init) function;
		module.symbol ("retro_deinit", out function);
		_deinit = (Deinit) function;
		module.symbol ("retro_api_version", out function);
		_api_version = (ApiVersion) function;
		module.symbol ("retro_get_system_info", out function);
		_get_system_info = (GetSystemInfo) function;
		module.symbol ("retro_get_system_av_info", out function);
		_get_system_av_info = (GetSystemAvInfo) function;
		module.symbol ("retro_set_controller_port_device", out function);
		_set_controller_port_device = (SetControllerPortDevice) function;
		module.symbol ("retro_reset", out function);
		_reset = (Reset) function;
		module.symbol ("retro_run", out function);
		_run = (Run) function;
		module.symbol ("retro_serialize_size", out function);
		_serialize_size = (SerializeSize) function;
		module.symbol ("retro_serialize", out function);
		_serialize = (Serialize) function;
		module.symbol ("retro_unserialize", out function);
		_unserialize = (Unserialize) function;
		module.symbol ("retro_cheat_reset", out function);
		_cheat_reset = (CheatReset) function;
		module.symbol ("retro_cheat_set", out function);
		_cheat_set = (CheatSet) function;
		module.symbol ("retro_load_game", out function);
		_load_game = (LoadGame) function;
		module.symbol ("retro_load_game_special", out function);
		_load_game_special = (LoadGameSpecial) function;
		module.symbol ("retro_unload_game", out function);
		_unload_game = (UnloadGame) function;
		module.symbol ("retro_get_region", out function);
		_get_region = (GetRegion) function;
		module.symbol ("retro_get_memory_data", out function);
		_get_memory_data = (GetMemoryData) function;
		module.symbol ("retro_get_memory_size", out function);
		_get_memory_size = (GetMemorySize) function;
	}
	
	~Core () {
		if (game_loaded) unload_game ();
		if (is_init) deinit ();
	}
	
	/**
	 * Initialize the module.
	 * 
	 * {@link Retro.Core.environment_cb} must be set before the module is
	 * initialized.
	 */
	public void init () {
		set_global_self ();
		_init ();
		is_init = true;
	}
	
	/**
	 * Deinitialize the module.
	 */
	private void deinit () {
		set_global_self ();
		_deinit ();
		is_init = false;
	}
	
	/**
	 * The version of libretro used by the module.
	 * 
	 * Can be compared with {@link Retro.API_VERSION} to validate ABI
	 * compatibility.
	 * 
	 * @return the libretro version of the module
	 */
	public uint api_version () {
		set_global_self ();
		return _api_version ();
	}
	
	/**
	 * Gets system information.
	 * 
	 * Can be called at any time, even before {@link Retro.Core.init}.
	 * 
	 * @return information on the system implemented in the module
	 */
	public SystemInfo get_system_info () {
		set_global_self ();
		
		SystemInfo info;
		_get_system_info (out info);
		return info;
	}
	
	/**
	 * Gets information about system audio/video timings and geometry.
	 * 
	 * Can be called only after {@link Retro.Core.load_game} has successfully
	 * completed.
	 * 
	 * NOTE: The implementation of this function might not initialize every
	 * variable if needed.
	 * E.g. geometry.aspect_ratio might not be initialized if the core doesn't
	 * desire a particular aspect ratio.
	 * 
	 * @return information on the system audio/video timings and geometry
	 */
	public SystemAvInfo get_system_av_info () {
		set_global_self ();
		
		SystemAvInfo info;
		_get_system_av_info (out info);
		return info;
	}
	
	/**
	 * Sets device to be used for player 'port'.
	 * 
	 * @param port the port on wich to connect a device
	 * @param device the type of the device connected
	 */
	public void set_controller_port_device (uint port, Device.Type device) {
		set_global_self ();
		_set_controller_port_device (port, device);
	}
	
	/**
	 * Resets the current game.
	 */
	public void reset () {
		set_global_self ();
		_reset ();
	}
	
	/**
	 * Runs the game for one video frame.
	 * 
	 * The callbacks must be set and the core must be initialized before
	 * running the core.
	 * 
	 * During {@link Retro.Core.run}, the input_poll callback will be called
	 * at least once.
	 * 
	 * If a frame is not rendered for reasons where a game "dropped" a frame,
	 * this still counts as a frame, and {@link Retro.Core.run} will
	 * explicitly dupe a frame if
	 * {@link Retro.Environment.Command.GET_CAN_DUPE} returns true.
	 * In this case, the video callback can take a null argument for data.
	 */
	public void run () {
		set_global_self ();
		_run ();
	}
	
	/**
	 * Returns the amount of data the implementation requires to serialize the
	 * internal state (save states).
	 * 
	 * Beetween calls to {@link Retro.Core.load_game} and
	 * {@link Retro.Core.unload_game}, the returned size is never allowed to
	 * be larger than a previous returned value, to ensure that the frontend can
	 * allocate a save state buffer once.
	 * 
	 * @return the size needed to serialize the internal state
	 */
	public size_t serialize_size () {
		set_global_self ();
		return _serialize_size ();
	}
	
	/**
	 * Serializes the internal state.
	 * 
	 * If failed, or size is lower than {@link Retro.Core.serialize_size}, it
	 * should return false, true otherwise.
	 * 
	 * @param data the buffer where the data will be stored
	 * @return false if the serialization failed, true otherwise
	 */
	public bool serialize ([CCode (array_length_type = "gsize")] out uint8[] data) {
		set_global_self ();
		return _serialize (data);
	}
	
	/**
	 * Unserializes the internal state.
	 * 
	 * @param data the buffer where the data is stored
	 * @return false if the unserialization failed, true otherwise
	 */
	public bool unserialize ([CCode (array_length_type = "gsize")] uint8[] data) {
		set_global_self ();
		return _unserialize (data);
	}
	
	/**
	 * Resets the cheats.
	 */
	[Deprecated (since = "1.0")]
	public void cheat_reset () {
		set_global_self ();
		_cheat_reset ();
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
		set_global_self ();
		_cheat_set (index, enabled, code);
	}
	
	/**
	 * Loads a game.
	 * 
	 * @param game information to load the game
	 * @return false if the loading failed, true otherwise
	 */
	public bool load_game (GameInfo game) {
		if (game_loaded) unload_game ();
		
		set_global_self ();
		game_loaded = _load_game (game);
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
		
		set_global_self ();
		game_loaded = _load_game_special (game_type, info);
		return game_loaded;
	}
	
	/**
	 * Unloads a currently loaded game.
	 */
	private void unload_game () {
		set_global_self ();
		_unload_game ();
	}
	
	/**
	 * Gets region of the loaded game.
	 * 
	 * @return the region of the game
	 */
	public Region get_region () {
		set_global_self ();
		return _get_region ();
	}
	
	/**
	 * Gets a region of memory.
	 * 
	 * @param id the region of memory
	 * @return the region of memory
	 */
	public void *get_memory_data (Memory.Type id) {
		set_global_self ();
		return _get_memory_data (id);
	}
	
	/**
	 * Gets the size of a region of memory.
	 * 
	 * @param id the region of memory
	 * @return the size of the region of memory
	 */
	public size_t get_memory_size (Memory.Type id) {
		set_global_self ();
		return _get_memory_size (id);
	}
}

}
