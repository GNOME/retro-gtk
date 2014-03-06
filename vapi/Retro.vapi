[CCode (cheader_filename = "libretro.h")]

namespace Retro {

[CCode (cname = "RETRO_API_VERSION")]
public const uint api_version;

[CCode (cname = "unsigned")]
public enum Region {
	NTSC,
	PAL
}

[CCode (cname = "unsigned")]
public enum Memory {
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
public enum GameType {
	BSX,
	BSX_SLOTTED,
	SUFAMI_TURBO,
	SUPER_GAME_BOY
}

[CCode (cname = "unsigned", cprefix = "RETROK_")]
public enum Key {
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

[CCode (cname = "uint16_t", cprefix = "RETROKMOD_")]
public enum ModifierKey {
	NONE,
	
	SHIFT,
	CTRL,
	ALT,
	META,
	
	NUMLOCK,
	CAPSLOCK,
	SCROLLOCK
}

// Video

namespace Video {
	[CCode (cname = "enum retro_pixel_format", cprefix = "RETRO_PIXEL_FORMAT_")]
	public enum PixelFormat {
		[CCode (cname = "RETRO_PIXEL_FORMAT_0RGB1555")]
		ORGB1555,
		XRGB8888,
		RGB565,
		UNKNOWN
	}
}

// Performance

namespace Perf {
	[CCode (cname = "uint64_t", cprefix = "RETRO_SIMD_")]
	public enum SIMD {
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
	public struct tick_t {}
	
	[CCode (cname = "retro_time_t", simple_type = 1)]
	public struct time_t {}
	
	[CCode (cname = "struct retro_perf_counter")]
	public struct Counter {
		const string ident;
		tick_t start;
		tick_t total;
		tick_t call_cnt;
	
		bool registered;
	}
	
	[CCode (cname = "retro_perf_get_time_usec_t")]
	public delegate Retro.Perf.time_t GetTimeUsec ();
	
	[CCode (cname = "retro_perf_get_counter_t")]
	public delegate tick_t GetCounter ();
	
	[CCode (cname = "retro_get_cpu_features_t")]
	public delegate uint64 GetCpuFeatures ();
	
	[CCode (cname = "retro_perf_log_t")]
	public delegate void Log ();
	
	[CCode (cname = "retro_perf_register_t")]
	public delegate void Register (Counter counter);
	
	[CCode (cname = "retro_perf_start_t")]
	public delegate void Start (Counter counter);
	
	[CCode (cname = "retro_perf_stop_t")]
	public delegate void Stop (Counter counter);
	
	[CCode (cname = "struct retro_perf_callback")]
	public struct Callback {
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
	public enum Action {
		ACCELEROMETER_ENABLE,
		ACCELEROMETER_DISABLE
	}
	
	[CCode (cname = "retro_set_sensor_state_t")]
	public delegate bool SetState (uint port, Action action, uint rate);
	
	[CCode (cname = "struct retro_sensor_interface")]
	public struct Interface {
		SetState set_sensor_state;
	}
}

namespace Camera {
	[CCode (cname = "unsigned")]
	public enum Buffer {
		OPENGL_TEXTURE,
		RAW_FRAMEBUFFER
	}
	
	[CCode (cname = "retro_camera_start_t")]
	public delegate bool Start ();
	
	[CCode (cname = "retro_camera_stop_t")]
	public delegate void Stop ();
	
	[CCode (cname = "retro_camera_lifetime_status_t")]
	public delegate void LifetimeStatus ();
	
	[CCode (cname = "retro_camera_frame_raw_framebuffer_t")]
	public delegate void FrameRawFramebuffer (uint32[] buffer, uint width, uint height, size_t pitch);
	
	[CCode (cname = "retro_camera_frame_opengl_texture_t")]
	public delegate void FrameOpenglTexture (uint texture_id, uint texture_target, float[] affine);
	
	[CCode (cname = "struct retro_camera_callback")]
	public struct Callback {
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
	public enum Effect {
		STRONG,
		WEAK
	}
	
	[CCode (cname = "retro_set_rumble_state_t")]
	public delegate bool SetState (uint port, Effect effect, uint16 strength);
	
	[CCode (cname = "struct retro_rumble_interface")]
	public struct Interface {
		SetState set_rumble_state;
	}
}

namespace Audio {
	[CCode (cname = "retro_audio_callback_t")]
	public delegate void AudioCallback ();
	
	[CCode (cname = "retro_audio_set_state_callback_t")]
	public delegate void SetStateCallback (bool enabled);
	
	[CCode (cname = "struct retro_rumble_interface")]
	public struct Callback {
		AudioCallback    callback;
		SetStateCallback set_state;
	} 
}

namespace FrameTime {
	[CCode (cname = "retro_usec_t", simple_type = 1)]
	public struct Usec {}
	
	[CCode (cname = "retro_frame_time_callback_t")]
	public delegate void FrameTimeCallback (Usec usec);
	
	[CCode (cname = "struct retro_frame_time_callback")]
	public struct Callback {
		FrameTimeCallback callback;
		Usec              reference;
	} 
}

[CCode (cname = "uintptr_t", simple_type = 1)]
public struct uintptr_t {}

namespace Hardware {
	[CCode (cname = "RETRO_HW_FRAME_BUFFER_VALID")]
	public const uintptr_t frame_buffer_valid;
	
	[CCode (cname = "retro_hw_context_reset_t")]
	public delegate void ContextReset ();
	
	[CCode (cname = "retro_hw_get_current_framebuffer_t")]
	public delegate uintptr_t GetCurrentFramebuffer ();
	
	[CCode (cname = "retro_proc_address_t")]
	public delegate void ProcAdress ();
	
	[CCode (cname = "retro_hw_get_proc_address_t")]
	public delegate ProcAdress GetProcAdress (string sym);
	
	[CCode (cname = "unsigned", cprefix = "RETRO_HW_CONTEXT_")]
	public enum ContexType {
		NONE,
		OPENGL,
		OPENGLES2,
		OPENGL_CORE,
		OPENGLES3
	}
	
	[CCode (cname = "struct retro_hw_render_callback")]
	public struct RenderCallback {
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
	public delegate void Event (bool down, Key keycode, uint32 character, ModifierKey key_modifiers);
	
	[CCode (cname = "struct retro_keyboard_callback")]
	public struct Callback {
		Event callback;
	}
}

namespace Disk {
	[CCode (cname = "retro_set_eject_state_t")]
	public delegate bool SetEjectState (bool ejected);
	
	[CCode (cname = "retro_get_eject_state_t")]
	public delegate bool GetEjectState ();
	
	[CCode (cname = "retro_get_image_index_t")]
	public delegate uint GetImageIndex ();
	
	[CCode (cname = "retro_set_image_index_t")]
	public delegate bool SetImageIndex (uint index);
	
	[CCode (cname = "retro_get_num_images_t")]
	public delegate uint GetNumImages ();
	
	[CCode (cname = "retro_replace_image_index_t")]
	public delegate bool ReplaceImageIndex (uint index, GameInfo info);
	
	[CCode (cname = "retro_add_image_index_t")]
	public delegate bool AddImageIndex ();
	
	[CCode (cname = "struct retro_disk_control_callback")]
	public struct Callback {
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
public struct Message {
   const string msg;
   uint         frames;
}

}

