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

[CCode (cname = "gint", cprefix = "RETROK_")]
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

[CCode (cname = "gint", cprefix = "RETROKMOD_")]
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
	[Flags]
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

namespace Keyboard {
	[CCode (cname = "retro_keyboard_event_t")]
	public delegate void Event (bool down, Key keycode, uint32 character, ModifierKey key_modifiers);
	
	[CCode (cname = "struct retro_keyboard_callback")]
	public struct Callback {
		Event callback;
	}
}

}

