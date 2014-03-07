[CCode (cheader_filename = "libretro.h")]

namespace Retro {

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

}

