[CCode (cheader_filename = "retro.h")]

namespace Retro {

[CCode (cname = "RETRO_API_VERSION")]
const uint api_version;

[CCode (cname = "unsigned")]
enum Device {
	MASK,
	NONE,
	JOYPAD,
	MOUSE,
	KEYBOARD,
	LIGHTGUN,
	ANALOG,
	POINTER,
	SENSOR_ACCELEROMETER,
	JOYPAD_MULTITAP,
	LIGHTGUN_SUPER_SCOPE,
	LIGHTGUN_JUSTIFIER,
	LIGHTGUN_JUSTIFIERS
}

[CCode (cname = "unsigned")]
enum DeviceIndex {
	ANALOG_LEFT,
	ANALOG_RIGHT
}

[CCode (cname = "unsigned", cprefix = "RETRO_DEVICE_ID_")]
enum DeviceID {
	JOYPAD_B,
	JOYPAD_Y,
	JOYPAD_SELECT,
	JOYPAD_START,
	JOYPAD_UP,
	JOYPAD_DOWN,
	JOYPAD_LEFT,
	JOYPAD_RIGHT,
	JOYPAD_A,
	JOYPAD_X,
	JOYPAD_L,
	JOYPAD_R,
	JOYPAD_L2,
	JOYPAD_R2,
	JOYPAD_L3,
	JOYPAD_R3,
	
	ANALOG_X,
	ANALOG_Y,
	
	MOUSE_X,
	MOUSE_Y,
	MOUSE_LEFT,
	MOUSE_RIGHT,
	
	LIGHTGUN_X,
	LIGHTGUN_Y,
	LIGHTGUN_TRIGGER,
	LIGHTGUN_CURSOR,
	LIGHTGUN_TURBO,
	LIGHTGUN_PAUSE,
	LIGHTGUN_START,
	
	POINTER_X,
	POINTER_Y,
	POINTER_PRESSED,
	
	SENSOR_ACCELEROMETER_X,
	SENSOR_ACCELEROMETER_Y,
	SENSOR_ACCELEROMETER_Z
}

[CCode (cname = "unsigned")]
enum Region {
	NTSC,
	PAL
}

[CCode (cname = "unsigned")]
enum Memory {
	MASK,
	SAVE_RAM,
	RTC,
	SYSTEM_RAM,
	VIDEO_RAM,
	SNES_BSX_RAM,
	SNES_BSX_PRAM,
	SNES_SUFAMI_TURBO_A_RAM,
	SNES_SUFAMI_TURBO_B_RAM,
	SNES_GAME_BOY_RAM,
	SNES_GAME_BOY_RTC
}

[CCode (cname = "unsigned")]
enum GameType {
	BSX,
	BSX_SLOTTED,
	SUFAMI_TURBO,
	SUPER_GAME_BOY
}

[CCode (cname = "enum retro_key", cprefix = "RETROK_")]
enum Key {
	UNKNOWN,
	FIRST,
	BACKSPACE,
	TAB,
	CLEAR,
	RETURN,
	PAUSE,
	ESCAPE,
	SPACE,
	EXCLAIM,
	QUOTEDBL,
	HASH,
	DOLLAR,
	AMPERSAND,
	QUOTE,
	LEFTPAREN,
	RIGHTPAREN,
	ASTERISK,
	PLUS,
	COMMA,
	MINUS,
	PERIOD,
	SLASH,
	[CCode (cname = "RETROK_0")]
	ZERO,
	[CCode (cname = "RETROK_1")]
	ONE,
	[CCode (cname = "RETROK_2")]
	TWO,
	[CCode (cname = "RETROK_3")]
	THREE,
	[CCode (cname = "RETROK_4")]
	FOUR,
	[CCode (cname = "RETROK_5")]
	FIVE,
	[CCode (cname = "RETROK_6")]
	SIX,
	[CCode (cname = "RETROK_7")]
	SEVEN,
	[CCode (cname = "RETROK_8")]
	EIGHT,
	[CCode (cname = "RETROK_9")]
	NINE,
	COLON,
	SEMICOLON,
	LESS,
	EQUALS,
	GREATER,
	QUESTION,
	AT,
	LEFTBRACKET,
	BACKSLASH,
	RIGHTBRACKET,
	CARET,
	UNDERSCORE,
	BACKQUOTE,
	a,
	b,
	c,
	d,
	e,
	f,
	g,
	h,
	i,
	j,
	k,
	l,
	m,
	n,
	o,
	p,
	q,
	r,
	s,
	t,
	u,
	v,
	w,
	x,
	y,
	z,
	DELETE,
	
	KP0,
	KP1,
	KP2,
	KP3,
	KP4,
	KP5,
	KP6,
	KP7,
	KP8,
	KP9,
	KP_PERIOD,
	KP_DIVIDE,
	KP_MULTIPLY,
	KP_MINUS,
	KP_PLUS,
	KP_ENTER,
	KP_EQUALS,
	
	UP,
	DOWN,
	RIGHT,
	LEFT,
	INSERT,
	HOME,
	END,
	PAGEUP,
	PAGEDOWN,
	
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	
	NUMLOCK,
	CAPSLOCK,
	SCROLLOCK,
	RSHIFT,
	LSHIFT,
	RCTRL,
	LCTRL,
	RALT,
	LALT,
	RMETA,
	LMETA,
	LSUPER,
	RSUPER,
	MODE,
	COMPOSE,
	
	HELP,
	PRINT,
	SYSREQ,
	BREAK,
	MENU,
	POWER,
	EURO,
	UNDO,
	LAST
}

[CCode (cname = "enum retro_mod", cprefix = "RETROKMOD_")]
enum ModifierKey {
	NONE,
	
	SHIFT,
	CTRL,
	ALT,
	META,
	
	NUMLOCK,
	CAPSLOCK,
	SCROLLOCK
}

// Environment

namespace Environment {
	[CCode (cname = "unsigned", cprefix = "RETRO_ENVIRONMENT_")]
	enum Command {
		SET_ROTATION,
		GET_OVERSCAN,
		GET_CAN_DUPE,
		SET_MESSAGE,
		SHUTDOWN,
		SET_PERFORMANCE_LEVEL,
		GET_SYSTEM_DIRECTORY,
		SET_PIXEL_FORMAT,
		SET_INPUT_DESCRIPTORS,
		SET_KEYBOARD_CALLBACK,
		SET_DISK_CONTROL_INTERFACE,
		SET_HW_RENDER,
		GET_VARIABLE,
		SET_VARIABLES,
		GET_VARIABLE_UPDATE,
		SET_SUPPORT_NO_GAME,
		GET_LIBRETRO_PATH,
		SET_AUDIO_CALLBACK,
		SET_FRAME_TIME_CALLBACK,
		GET_RUMBLE_INTERFACE,
		GET_INPUT_DEVICE_CAPABILITIES,
		GET_SENSOR_INTERFACE,
		GET_CAMERA_INTERFACE,
		GET_LOG_INTERFACE,
		GET_PERF_INTERFACE,
		EXPERIMENTAL,
		PRIVATE
	}
	
	[CCode (cname = "retro_core_environment_cb_t")]
	delegate bool Callback (Command cmd, void *data);
}

// Video

namespace Video {
	[CCode (cname = "enum retro_pixel_format", cprefix = "RETRO_PIXEL_FORMAT_")]
	enum PixelFormat {
		[CCode (cname = "RETRO_PIXEL_FORMAT_0RGB1555")]
		ORGB1555,
		XRGB8888,
		RGB565,
		UNKNOWN
	}
	
	[CCode (cname = "retro_core_video_refresh_cb_t")]
	delegate void Refresh (void *data, uint width, uint height, size_t pitch);
}

// Log

namespace Log {
	[CCode (cname = "enum retro_log_level", cprefix = "RETRO_LOG_")]
	enum Level {
		DEBUG,
		INFO,
		WARN,
		ERROR
	}
	
	[CCode (cname = "retro_core_environment_cb_t")]
	delegate bool Printf (Level level, string fmt, ...);
	
	[CCode (cname = "struct retro_log_callback")]
	struct Callback {
		Printf log;
	}
}

// Performance

namespace Perf {
	[CCode (cname = "uint64_t", cprefix = "RETRO_SIMD_")]
	enum SIMD {
		SSE,
		SSE2,
		VMX,
		VMX128,
		AVX,
		NEON,
		SSE3,
		SSSE3
	}
	
	[CCode (cname = "retro_perf_tick_t", simple_type = 1)]
	struct tick_t {}
	
	[CCode (cname = "retro_time_t", simple_type = 1)]
	struct time_t {}
	
	[CCode (cname = "struct retro_perf_counter")]
	struct Counter {
		const string ident;
		tick_t start;
		tick_t total;
		tick_t call_cnt;
	
		bool registered;
	}
	
	[CCode (cname = "retro_perf_get_time_usec_t")]
	delegate Retro.Perf.time_t GetTimeUsec ();
	
	[CCode (cname = "retro_perf_get_counter_t")]
	delegate tick_t GetCounter ();
	
	[CCode (cname = "retro_get_cpu_features_t")]
	delegate uint64 GetCpuFeatures ();
	
	[CCode (cname = "retro_perf_log_t")]
	delegate void Log ();
	
	[CCode (cname = "retro_perf_register_t")]
	delegate void Register (Counter counter);
	
	[CCode (cname = "retro_perf_start_t")]
	delegate void Start (Counter counter);
	
	[CCode (cname = "retro_perf_stop_t")]
	delegate void Stop (Counter counter);
	
	[CCode (cname = "struct retro_perf_callback")]
	struct Callback {
		GetTimeUsec    get_time_usec;
		GetCpuFeatures get_cpu_features;
		GetCounter     get_perf_counter;
		Register       perf_register;
		Start          perf_start;
		Stop           perf_stop;
		Log            perf_log;
	}
	
}

namespace Sensor {
	[CCode (cname = "unsigned", cprefix = "RETRO_SENSOR_")]
	enum Action {
		ACCELEROMETER_ENABLE,
		ACCELEROMETER_DISABLE
	}
	
	[CCode (cname = "retro_set_sensor_state_t")]
	delegate bool SetState (uint port, Action action, uint rate);
	
	[CCode (cname = "struct retro_sensor_interface")]
	struct Interface {
		SetState set_sensor_state;
	}
}

namespace Camera {
	[CCode (cname = "unsigned")]
	enum Buffer {
		OPENGL_TEXTURE,
		RAW_FRAMEBUFFER
	}
	
	[CCode (cname = "retro_camera_start_t")]
	delegate bool Start ();
	
	[CCode (cname = "retro_camera_stop_t")]
	delegate void Stop ();
	
	[CCode (cname = "retro_camera_lifetime_status_t")]
	delegate void LifetimeStatus ();
	
	[CCode (cname = "retro_camera_frame_raw_framebuffer_t")]
	delegate void FrameRawFramebuffer (uint32[] buffer, uint width, uint height, size_t pitch);
	
	[CCode (cname = "retro_camera_frame_opengl_texture_t")]
	delegate void FrameOpenglTexture (uint texture_id, uint texture_target, float[] affine);
	
	[CCode (cname = "struct retro_camera_callback")]
	struct Callback {
		uint64              caps;
		uint                width;
		uint                height;
		Start               start;
		Stop                stop;
		FrameRawFramebuffer frame_raw_framebuffer;
		FrameOpenglTexture  frame_opengl_texture;
		LifetimeStatus      initialized;
		LifetimeStatus      deinitialized;
	}
}

namespace Rumble {
	[CCode (cname = "unsigned")]
	enum Effect {
		STRONG,
		WEAK
	}
	
	[CCode (cname = "retro_set_rumble_state_t")]
	delegate bool SetState (uint port, Effect effect, uint16 strength);
	
	[CCode (cname = "struct retro_rumble_interface")]
	struct Interface {
		SetState set_rumble_state;
	}
}

namespace Audio {
	[CCode (cname = "retro_audio_callback_t")]
	delegate void AudioCallback ();
	
	[CCode (cname = "retro_audio_set_state_callback_t")]
	delegate void SetStateCallback (bool enabled);
	
	[CCode (cname = "struct retro_rumble_interface")]
	struct Callback {
		AudioCallback    callback;
		SetStateCallback set_state;
	} 
}

namespace FrameTime {
	[CCode (cname = "retro_usec_t", simple_type = 1)]
	struct Usec {}
	
	[CCode (cname = "retro_frame_time_callback_t")]
	delegate void FrameTimeCallback (Usec usec);
	
	[CCode (cname = "struct retro_frame_time_callback")]
	struct Callback {
		FrameTimeCallback callback;
		Usec              reference;
	} 
}

[CCode (cname = "uintptr_t", simple_type = 1)]
struct uintptr_t {}

namespace Hardware {
	[CCode (cname = "RETRO_HW_FRAME_BUFFER_VALID")]
	const uintptr_t frame_buffer_valid;
	
	[CCode (cname = "retro_hw_context_reset_t")]
	delegate void ContextReset ();
	
	[CCode (cname = "retro_hw_get_current_framebuffer_t")]
	delegate uintptr_t GetCurrentFramebuffer ();
	
	[CCode (cname = "retro_proc_address_t")]
	delegate void ProcAdress ();
	
	[CCode (cname = "retro_hw_get_proc_address_t")]
	delegate ProcAdress GetProcAdress (string sym);
	
	[CCode (cname = "unsigned", cprefix = "RETRO_HW_CONTEXT_")]
	enum ContexType {
		NONE,
		OPENGL,
		OPENGLES2,
		OPENGL_CORE,
		OPENGLES3
	}
	
	[CCode (cname = "struct retro_hw_render_callback")]
	struct RenderCallback {
		ContexType            context_type;
		ContextReset          context_reset;
		GetCurrentFramebuffer get_current_framebuffer;
		GetProcAdress         get_proc_address;
		bool                  depth;
		bool                  stencil;
		bool                  bottom_left_origin;
		uint                  version_major;
		uint                  version_minor;
		bool                  cache_context;
		ContextReset          context_destroy;
		bool                  debug_context;
	}
}

namespace Keyboard {
	[CCode (cname = "retro_keyboard_event_t")]
	delegate void Event (bool down, uint keycode, uint32 character, uint16 key_modifiers);
	
	[CCode (cname = "struct retro_keyboard_callback")]
	struct Callback {
		Event callback;
	}
}

namespace Disk {
	[CCode (cname = "retro_set_eject_state_t")]
	delegate bool SetEjectState (bool ejected);
	
	[CCode (cname = "retro_get_eject_state_t")]
	delegate bool GetEjectState ();
	
	[CCode (cname = "retro_get_image_index_t")]
	delegate uint GetImageIndex ();
	
	[CCode (cname = "retro_set_image_index_t")]
	delegate bool SetImageIndex (uint index);
	
	[CCode (cname = "retro_get_num_images_t")]
	delegate uint GetNumImages ();
	
	[CCode (cname = "retro_replace_image_index_t")]
	delegate bool ReplaceImageIndex (uint index, GameInfo info);
	
	[CCode (cname = "retro_add_image_index_t")]
	delegate bool AddImageIndex ();
	
	[CCode (cname = "struct retro_disk_control_callback")]
	struct Callback {
		SetEjectState     set_eject_state;
		GetEjectState     get_eject_state;
		GetImageIndex     get_image_index;
		SetImageIndex     set_image_index;
		GetNumImages      get_num_images;
		ReplaceImageIndex replace_image_index;
		AddImageIndex     add_image_index;
	}
}

[CCode (cname = "struct retro_message")]
struct Message {
   const string msg;
   uint         frames;
}

[CCode (cname = "struct retro_input_descriptor")]
struct InputDescriptor {
   uint port;
   Device      device;
   DeviceIndex index;
   DeviceID    id;
	
   const string description;
}

[CCode (cname = "struct retro_system_info", has_destroy_function = 0)]
struct SystemInfo {
	[CCode (weak = 1)]
	public const string library_name;
	[CCode (weak = 1)]
	public const string library_version;
	
	[CCode (weak = 1)]
	public const string valid_extensions;

	public bool   need_fullpath;
	public bool   block_extract;
}

[CCode (cname = "struct retro_game_geometry", has_destroy_function = 0)]
struct GameGeometry {
	public uint  base_width;
	public uint  base_height;
	public uint  max_width;
	public uint  max_height;
	public float aspect_ratio;
}

[CCode (cname = "struct retro_system_timing", has_destroy_function = 0)]
struct SystemTiming {
	public double fps;
	public double sample_rate;
}

[CCode (cname = "struct retro_system_av_info", has_destroy_function = 0)]
struct SystemAvInfo {
	public GameGeometry geometry;
	public SystemTiming timing;
}

[CCode (cname = "struct retro_variable", has_destroy_function = 0)]
struct Variable {
	public string key;
	public string value;
}

[CCode (cname = "struct retro_game_info", has_destroy_function = 0)]
struct GameInfo {
	[CCode (weak = 1)]
	public const string path;
	void  *data;
	size_t size;
	string meta;
}

delegate void   AudioSample      (int16 left, int16 right);
delegate size_t AudioSampleBatch ([CCode (array_length_cname = "frames", array_length_type = "size_t")] int16[] data);
delegate void   InputPoll        ();
delegate int16  InputState       (uint port, uint device, uint index, uint id);

[CCode (cname = "retro_core_t", cheader_filename = "retro_core.h")]
struct Core {
	[CCode (cname = "retro_core_construct")]
	public Core (string library_path);
	
	public void finalize ();
	
	public void set_environment (Environment.Callback cb);
	public void set_video_refresh (Video.Refresh cb);
	public void set_audio_sample (AudioSample cb);
	public void set_audio_sample_batch (AudioSampleBatch cb);
	public void set_input_poll (InputPoll cb);
	public void set_input_state (InputState cb);
	
	public void init ();
	public void deinit ();
	
	public uint api_version ();
	
	public void get_system_info (out SystemInfo info);
	public void get_system_av_info (out SystemAvInfo *info);
	
	public void set_controller_port_device (uint port, uint device);
	
	public void reset ();
	public void run ();
	
	public size_t serialize_size ();
	public bool serialize (void *data, size_t size);
	public bool unserialize (void *data, size_t size);
	
	public void cheat_reset ();
	public void cheat_set (uint index, bool enabled, string code);
	
	public bool load_game (GameInfo *game);
	public bool load_game_special (uint game_type, GameInfo *info, size_t num_info);
	public void unload_game ();
	
	public Region get_region ();
	
	public void *get_memory_data (uint id);
	public size_t get_memory_size (uint id);
}

}

