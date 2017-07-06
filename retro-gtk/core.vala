// This file is part of retro-gtk. License: GPLv3

namespace Retro {

/**
 * Handles a Libretro module.
 */
public class Core : Object {
	private static RecMutex r_mutex = RecMutex ();
	private static RecMutex w_mutex = RecMutex ();
	private static (unowned Core)[] objects = new (unowned Core)[32];
	private static int i = 0;

	public signal void video_output (uint8[] data, uint width, uint height, size_t pitch, PixelFormat pixel_format, float aspect_ratio);
	public signal void audio_output (int16[] frames, double sample_rate);
	public signal void log (string log_domain, LogLevelFlags log_level, string message);

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

	internal double _frames_per_second;
	public double frames_per_second {
		get { return _frames_per_second; }
	}

	/**
	 * The disk controlling interface.
	 *
	 * The Core can set it to let the frontend insert and eject disks images.
	 */
	public DiskControl disk_control_interface { internal set; get; }

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

	/**
	 * The rumble interface.
	 *
	 * Optional.
	 * If set, it must be set before {@link init} is called.
	 */
	public Rumble rumble_interface { set; get; }

	/**
	 * Asks the frontend to shut down.
	 */
	public signal bool shutdown ();

	/**
	 * Asks the frontend to display a message for an amount of frames.
	 */
	public signal bool message (string message, uint frames);

	private extern void set_environment_interface ();
	private extern void set_callbacks ();

	internal Module module;
	internal Variables variables_interface;

	internal void *environment_internal;

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
		environment_internal_setup ();

		libretro_path = File.new_for_path (file_name).resolve_relative_path ("").get_path ();

		module = new Module (libretro_path);
		set_callbacks ();
		variables_interface = new Options ();
	}

	~Core () {
		push_cb_data ();
		if (game_loaded)
			module.unload_game ();
		module.deinit ();
		pop_cb_data ();

		environment_internal_release ();
	}

	/**
	 * Initializes the module.
	 *
	 * Must be called before loading a game and running the core.
	 */
	public virtual signal void init () {
		set_environment_interface ();
		push_cb_data ();
		module.init ();
		pop_cb_data ();

		init_input ();

		is_initiated = true;
	}

	public extern void set_controller_port_device (uint port, DeviceType device);

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

	public extern bool supports_serialization ();

	[CCode (array_length_type = "gsize")]
	public extern uint8[] serialize_state () throws Error;

	public extern void deserialize_state ([CCode (array_length_type = "gsize")] uint8[] data) throws Error;

	/**
	 * Load. a game.
	 *
	 * @param game information to load the game
	 * @return false if the loading failed, true otherwise
	 */
	public bool load_game (GameInfo game) {
		if (game_loaded) {
			push_cb_data ();
			module.unload_game ();
			pop_cb_data ();
		}

		push_cb_data ();
		game_loaded = module.load_game (game);
		SystemAvInfo info;
		module.get_system_av_info (out info);
		set_system_av_info (info);
		pop_cb_data ();

		return game_loaded;
	}

	/**
	 * Prepare the standalone core.
	 *
	 * This should be used instead of load_game() for standalone cores.
	 *
	 * @return false if the preparation failed, true otherwise
	 */
	public bool prepare () {
		push_cb_data ();
		game_loaded = module.load_game (null);
		SystemAvInfo info;
		module.get_system_av_info (out info);
		set_system_av_info (info);
		pop_cb_data ();

		return game_loaded;
	}

	/**
	 * Gets the size of a region of memory.
	 *
	 * @param id the region of memory
	 * @return the size of the region of memory
	 */
	public extern size_t get_memory_size (MemoryType id);

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

	private extern void init_input ();
	private extern void on_input_controller_connected (uint port, InputDevice device);
	private extern void on_input_controller_disconnected (uint port);
	private extern void on_input_key_event (bool down, KeyboardKey keycode, uint32 character, KeyboardModifierKey key_modifiers);

	private extern void set_system_av_info (SystemAvInfo system_av_info);

	private extern void environment_internal_setup ();
	private extern void environment_internal_release ();
}

}
