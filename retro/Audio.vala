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

public interface Audio: Object {
	public abstract bool state { set; get; default = false; }
	public abstract void callback ();
}

private class CoreAudio: Object, Audio {
	[CCode (has_target = false)]
	internal delegate void AudioCallback ();
	
	[CCode (has_target = false)]
	internal delegate void SetStateCallback (bool enabled);
	
	internal struct Callback {
		AudioCallback    callback;
		SetStateCallback set_state;
	}
	
	public Callback callback_struct { construct; private get; }
	public bool state { set; get; }
	
	internal CoreAudio (Callback callback_struct) {
		Object (callback_struct: callback_struct);
	}
	
	construct {
		notify["state"].connect (() => {
			if (callback_struct.set_state != null) {
				callback_struct.set_state (state);
			}
		});
	}
	
	public void callback () {
		if (callback_struct.callback != null) {
			callback_struct.callback ();
		}
	}
}

}

