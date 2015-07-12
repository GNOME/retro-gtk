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

/**
 * TODO Change visibility once the interface have been tested.
 */
internal interface FrameTime: Object {
	public abstract int64 usec_reference { set; get; default = 0; }
	public abstract void callback (int64 usec);
}

private class CoreFrameTime: Object, FrameTime {
	[CCode (has_target = false)]
	internal delegate void FrameTimeCallback (int64 usec);

	internal struct Callback {
		FrameTimeCallback callback;
		int64             usec_reference;
	}

	public Callback callback_struct { construct; private get; }
	public int64 usec_reference { set; get; }

	internal CoreFrameTime (Callback callback_struct) {
		Object (callback_struct: callback_struct);
	}

	construct {
		notify["usec_reference"].connect (() => {
			callback_struct.usec_reference = usec_reference;
		});
	}

	public void callback (int64 usec) {
		if (callback_struct.callback != null) {
			callback_struct.callback (usec);
		}
	}
}

}

