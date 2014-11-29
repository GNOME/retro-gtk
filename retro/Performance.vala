/* Copyright (C) 2014  Adrien Plazas
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
	public abstract int64 get_time_usec ();
	public abstract uint64 get_cpu_features ();
	public abstract uint64 get_perf_counter ();

	public abstract void perf_register (PerfCounter counter);
	public abstract void perf_start (PerfCounter counter);
	public abstract void perf_stop (PerfCounter counter);
	public abstract void perf_log ();
}

}

