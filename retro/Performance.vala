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

public interface Performance: Object {
	[Flags]
	public enum SimdFlags {
		SSE    = (1 << 0),
		SSE2   = (1 << 1),
		VMX    = (1 << 2),
		VMX128 = (1 << 3),
		AVX    = (1 << 4),
		NEON   = (1 << 5),
		SSE3   = (1 << 6),
		SSSE3  = (1 << 7),
		MMX    = (1 << 8),
		MMXEXT = (1 << 9),
		SSE4   = (1 << 10),
		SSE42  = (1 << 11),
		AVX2   = (1 << 12),
		VFPU   = (1 << 13),
		PS     = (1 << 14)
	}
	
	public enum Level {
		EMBEDED,
		MOBILE,
		LOW_END,
		MID_END,
		HIGH_END
	}
	
	public struct Counter {
		string ident;
		uint64 start_tick;
		uint64 total_tick;
		uint64 call_cnt_tick;
	
		bool registered;
	}
	
	public abstract int64 get_time_usec ();
	public abstract uint64 get_cpu_features ();
	public abstract uint64 get_perf_counter ();
	
	public abstract void perf_register (Counter counter);
	public abstract void perf_start (Counter counter);
	public abstract void perf_stop (Counter counter);
	public abstract void perf_log ();
}

}

