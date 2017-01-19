// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * Handles a Libretro module.
 */
public class Core : Object {
	private static RecMutex r_mutex = RecMutex ();
	private static RecMutex w_mutex = RecMutex ();
	private static (unowned Core)[] objects = new (unowned Core)[32];
	private static int i = 0;

	/**
	 * Stores the current Core instance in a stack.
	 *
	 * Stores the current instance of Core in a thread local global stack.
	 * It allows to know wich Core a callback is related to.
	 *
	 * Must be called before any call to a function from the module.
	 */
	internal void push_cb_data () {
		w_mutex.lock ();
		r_mutex.lock ();

		if (i == objects.length) {
			stderr.printf ("Error: Callback data stack overflow.\n");

			r_mutex.unlock ();
			assert_not_reached ();
		}

		objects[i] = this;
		i++;

		r_mutex.unlock ();
	}

	/**
	 * Removes the Core at the head of the stack.
	 *
	 * Must be called after any call to {@link push_cb_data()}.
	 */
	internal static void pop_cb_data () {
		r_mutex.lock ();
		if (i == 0) {
			stderr.printf ("Error: Callback data stack underflow.\n");

			r_mutex.unlock ();
			w_mutex.unlock ();
			assert_not_reached ();
		}

		i--;
		objects[i] = null;

		r_mutex.unlock ();
		w_mutex.unlock ();
	}

	internal static unowned Core get_cb_data () {
		r_mutex.lock ();
		if (i == 0) {
			stderr.printf ("Error: Callback data segmentation fault.\n");

			r_mutex.unlock ();
			assert_not_reached ();
		}

		unowned Core result =  objects[i - 1];
		r_mutex.unlock ();

		return result;
	}

	/**
	 * The version of Libretro used by the module.
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
	 * The system informations.
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
	 * The region of the loaded game.
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
	 * The file name of the module.
	 */
	public string file_name { construct; get; }

	/**
	 * The directory the core will use to look for for additional data.
	 */
	public string system_directory { set; get; default = "."; }

	/**
	 * The absolute path to the source module file.
	 */
	public string libretro_path { set; get; default = "."; }

	/**
	 * The directory the core will use to look for for additional assets.
	 */
	public string content_directory { set; get; default = "."; }

	/**
	 * The directory the core will use to save user data.
	 */
	public string save_directory { set; get; default = "."; }

	/**
	 * Whether or not the a game is loaded.
	 */
	public bool is_initiated { private set; get; default = false; }

	/**
	 * Whether or not the a game is loaded.
	 */
	public bool game_loaded { private set; get; default = false; }

	/**
	 * Whether or not the core supports games.
	 */
	public bool support_no_game { internal set; get; default = false; }

	/**
	 * The level of performance requiered by the core to run correctly.
	 *
	 * Can be set by the Core when loading a game.
	 */
	public PerfLevel performance_level { internal set; get; }

	/**
	 * Information on audio and video geometry and timings.
	 *
	 * Can be set by the Core when loading a game.
	 */
	public AvInfo av_info { internal set; get; }

	/**
	 * The disk controlling interface.
	 *
	 * The Core can set it to let the frontend insert and eject disks images.
	 */
	public DiskControl disk_control_interface { internal set; get; }

	/**
	 * The video input interface.
	 *
	 * The Core can set it to let the frontend pass video to it.
	 *
	 * TODO Change visibility once the interface have been tested.
	 */
	internal HardwareRender hw_render { internal set; get; }

	/**
	 * The audio callback interface.
	 *
	 * The Core can set it to let the frontend pass audio to it.
	 */
	internal AudioCallback? audio_callback { set; get; }

	/**
	 * The time input interface.
	 *
	 * The Core can set it to let the frontend inform it of the amount
	 * of time passed since the last call to {@link run()}.
	 *
	 * TODO Change visibility once the interface have been tested.
	 */
	internal FrameTime frame_time_callback { internal set; get; }

	private weak Video _video_interface;
	/**
	 * The video interface.
	 *
	 * It must be set before {@link init} is called.
	 */
	public weak Video video_interface {
		get { return _video_interface; }
		construct set {
			if (_video_interface != null)
				_video_interface.core = null;

			_video_interface = value;

			if (_video_interface != null && _video_interface.core != this)
				_video_interface.core = this;
		}
	}

	private weak Audio _audio_interface;
	/**
	 * The audio interface.
	 *
	 * It must be set before {@link init} is called.
	 */
	public weak Audio audio_interface {
		get { return _audio_interface; }
		construct set {
			if (_audio_interface != null)
				_audio_interface.core = null;

			_audio_interface = value;

			if (_audio_interface != null && _audio_interface.core != this)
				_audio_interface.core = this;
		}
	}

	private weak Input _input_interface;
	private ulong input_controller_connected_id;
	private ulong input_controller_disconnected_id;
	private ulong input_key_event_id;
	/**
	 * The input interface.
	 *
	 * It must be set before {@link init} is called.
	 */
	public Input input_interface {
		get { return _input_interface; }
		construct set {
			if (value == input_interface)
				return;

			if (input_interface != null) {
				input_interface.disconnect (input_controller_connected_id);
				input_interface.disconnect (input_controller_disconnected_id);
				input_interface.disconnect (input_key_event_id);
			}

			_input_interface = value;

			if (input_interface == null)
				return;

			input_interface.controller_connected.connect (on_input_controller_connected);
			input_interface.controller_disconnected.connect (on_input_controller_disconnected);
			input_interface.key_event.connect (on_input_key_event);

			if (is_initiated)
				init_input ();
		}
	}

	private weak Variables _variables_interface;
	/**
	 * The variables interface.
	 *
	 * Optional.
	 * If set, it must be set before {@link init} is called.
	 */
	public weak Variables variables_interface {
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
	 * The rumble interface.
	 *
	 * Optional.
	 * If set, it must be set before {@link init} is called.
	 */
	public Rumble rumble_interface { set; get; }

	/**
	 * The sensor interface.
	 *
	 * Optional.
	 * If set, it must be set before {@link init} is called.
	 *
	 * TODO Change visibility once the interface have been tested.
	 */
	internal Sensor sensor_interface { set; get; }

	/**
	 * The camera interface.
	 *
	 * Optional.
	 * If set, it must be set before {@link init} is called.
	 *
	 * TODO Change visibility once the interface have been tested.
	 */
	internal Camera camera_interface { set; get; }

	/**
	 * The logging interface.
	 *
	 * Optional.
	 * If set, it must be set before {@link init} is called.
	 */
	public Log log_interface { set; get; }

	/**
	 * The performance interface.
	 *
	 * Optional.
	 * If set, it must be set before {@link init} is called.
	 *
	 * TODO Change visibility once the interface have been tested.
	 */
	internal Performance performance_interface { set; get; }

	/**
	 * The location interface.
	 *
	 * Optional.
	 * If set, it must be set before {@link init} is called.
	 *
	 * TODO Change visibility once the interface have been tested.
	 */
	internal Location location_interface { set; get; }

	/**
	 * Asks the frontend to shut down.
	 */
	public signal bool shutdown ();

	/**
	 * Asks the frontend to display a message for an amount of frames.
	 */
	public signal bool message (string message, uint frames);

	private extern void *get_module_environment_interface ();
	private extern void *get_module_video_refresh_cb ();
	private extern void *get_module_audio_sample_cb ();
	private extern void *get_module_audio_sample_batch_cb ();
	private extern void *get_module_input_poll_cb ();
	private extern void *get_module_input_state_cb ();

	internal Module module;

	/**
	 * Creates a Core from the file name of a Libretro implementation.
	 *
	 * The file must be a dynamically loadable shared object implementing the
	 * same version of the Libretro API as Retro.
	 *
	 * @param file_name the file name of the Libretro implementation to load
	 */
	public Core (string file_name) {
		Object (file_name: file_name);
	}

	construct {
		libretro_path = File.new_for_path (file_name).resolve_relative_path ("").get_path ();

		module = new Module (libretro_path);

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
	 * Initializes the module.
	 *
	 * Must be called before loading a game and running the core.
	 */
	public virtual signal void init () {
		push_cb_data ();
		module.set_environment (get_module_environment_interface ());
		module.init ();
		pop_cb_data ();

		init_input ();

		is_initiated = true;
	}

	/**
	 * Deinitializes the module.
	 */
	public void deinit () {
		push_cb_data ();
		module.deinit ();
		pop_cb_data ();
		is_initiated = false;
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
	 * frame if the can_dupe property of {@link video_interface} is set to true.
	 * In this case, the video callback can take a null argument for data.
	 */
	public void run () {
		push_cb_data ();
		module.run ();
		pop_cb_data ();
	}

	/**
	 * Returns the amount of data the implementation requires to serialize
	 * the internal state.
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
	public bool serialize ([CCode (array_length_type = "gsize")] uint8[] data) {
		push_cb_data ();
		var result = module.serialize (data);
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
	public void cheat_reset () {
		push_cb_data ();
		module.cheat_reset ();
		pop_cb_data ();
	}

	/**
	 * Sets a new cheat.
	 *
	 * @param index the index of the cheat
	 * @param enabled whether the cheat is enabled or not
	 * @param code the cheat code
	 */
	public void cheat_set (uint index, bool enabled, string code) {
		push_cb_data ();
		module.cheat_set (index, enabled, code);
		pop_cb_data ();
	}

	/**
	 * Load. a game.
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
	public void unload_game () {
		push_cb_data ();
		module.unload_game ();
		pop_cb_data ();
	}

	/**
	 * Gets the size of a region of memory.
	 *
	 * @param id the region of memory
	 * @return the size of the region of memory
	 */
	public size_t get_memory_size (MemoryType id) {
		push_cb_data ();
		var result = module.get_memory_size (id);
		pop_cb_data ();

		return result;
	}

	/**
	 * Gets a region of memory.
	 *
	 * @param id the region of memory
	 * @return the region of memory
	 */
	public extern uint8[] get_memory (MemoryType id);

	/**
	 * Sets a region of memory.
	 *
	 * @param id the region of memory
	 * @param data the data to write in the memory region
	 */
	public extern void set_memory (MemoryType id, uint8[] data);

	private void init_input () {
		if (input_interface == null)
			return;

		input_interface.foreach_controller (init_controller_device);
	}

	private void init_controller_device (uint port, InputDevice device) {
		var device_type = device.get_device_type ();
		set_controller_port_device (port, device_type);
	}

	private void on_input_controller_connected (uint port, InputDevice device) {
		if (!is_initiated)
			return;

		var device_type = device.get_device_type ();
		set_controller_port_device (port, device_type);
	}

	private void on_input_controller_disconnected (uint port) {
		if (!is_initiated)
			return;

		set_controller_port_device (port, DeviceType.NONE);
	}

	private void on_input_key_event (bool down, KeyboardKey keycode, uint32 character, KeyboardModifierKey key_modifiers) {
		if (!is_initiated)
			return;

		// TODO Handle the key event.
	}
}

}
