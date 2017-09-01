// This file is part of retro-gtk. License: GPL-3.0+.

namespace Retro {

/**
 * Handles a Libretro module.
 */
public class Core : Object {
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
	internal extern void push_cb_data ();

	/**
	 * Removes the Core at the head of the stack.
	 *
	 * Must be called after any call to {@link push_cb_data()}.
	 */
	internal extern static void pop_cb_data ();

	internal extern static unowned Core get_cb_data ();

	private extern uint get_api_version_real ();
	/**
	 * The version of Libretro used by the module.
	 *
	 * Can be compared with {@link API_VERSION} to validate ABI
	 * compatibility.
	 */
	public uint api_version {
		get { return get_api_version_real (); }
	}

	/**
	 * The file name of the module.
	 */
	public string file_name { internal set; get; }

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
	public bool is_initiated { internal set; get; default = false; }

	/**
	 * Whether or not the a game is loaded.
	 */
	public bool game_loaded { internal set; get; default = false; }

	/**
	 * Whether or not the core supports games.
	 */
	public bool support_no_game { internal set; get; default = false; }

	internal double _frames_per_second;
	public double frames_per_second {
		get { return _frames_per_second; }
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
			input_interface.key_event.connect (on_key_event);

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
		constructor (file_name);
	}

	~Core () {
		destructor ();
	}

	private extern void constructor (string file_name);
	private extern void destructor ();

	/**
	 * Initializes the module.
	 *
	 * Must be called before loading a game and running the core.
	 */
	public extern void init () throws Error;

	public extern void set_medias ([CCode (array_null_terminated = true, array_length = false)] string[] uris);

	public extern void set_current_media (uint media_index) throws Error;

	public extern void set_controller_port_device (uint port, DeviceType device);

	/**
	 * Resets the current game.
	 */
	public extern void reset ();

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
	public extern void run ();

	public extern bool supports_serialization ();

	[CCode (array_length_type = "gsize")]
	public extern uint8[] serialize_state () throws Error;

	public extern void deserialize_state ([CCode (array_length_type = "gsize")] uint8[] data) throws Error;

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
	private extern bool on_key_event (Gdk.EventKey event);
}

}
