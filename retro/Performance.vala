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

namespace Performance {
	namespace SimdFlags {
		public static const uint64 SSE    = (1 << 0);
		public static const uint64 SSE2   = (1 << 1);
		public static const uint64 VMX    = (1 << 2);
		public static const uint64 VMX128 = (1 << 3);
		public static const uint64 AVX    = (1 << 4);
		public static const uint64 NEON   = (1 << 5);
		public static const uint64 SSE3   = (1 << 6);
		public static const uint64 SSSE3  = (1 << 7);
		public static const uint64 MMX    = (1 << 8);
		public static const uint64 MMXEXT = (1 << 9);
		public static const uint64 SSE4   = (1 << 10);
		public static const uint64 SSE42  = (1 << 11);
		public static const uint64 AVX2   = (1 << 12);
		public static const uint64 VFPU   = (1 << 13);
		public static const uint64 PS     = (1 << 14);
	}
	
	public struct Counter {
		string ident;
		uint64 start_tick;
		uint64 total_tick;
		uint64 call_cnt_tick;
	
		bool registered;
	}
	
	[CCode (has_target = false)]
	public delegate int64 GetTimeUsec ();
	
	[CCode (has_target = false)]
	public delegate uint64 GetCounter ();
	
	[CCode (has_target = false)]
	public delegate uint64 GetCpuFeatures ();
	
	[CCode (has_target = false)]
	public delegate void Log ();
	
	[CCode (has_target = false)]
	public delegate void Register (Counter counter);
	
	[CCode (has_target = false)]
	public delegate void Start (Counter counter);
	
	[CCode (has_target = false)]
	public delegate void Stop (Counter counter);
	
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

}

