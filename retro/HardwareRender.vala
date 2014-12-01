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
internal interface HardwareRender: Object {
	public static const ulong FRAME_BUFFER_VALID = ulong.MAX;

	[CCode (has_target = false)]
	public delegate void ProcAdress ();

	public enum ContexType {
		NONE = 0,
		OPENGL,
		OPENGLES2,
		OPENGL_CORE,
		OPENGLES3,
		OPENGLES_VERSION
	}

	public abstract ContexType context_type { set; get; default = ContexType.NONE; }
	public abstract bool depth { set; get; default = false; }
	public abstract bool stencil { set; get; default = false; }
	public abstract bool bottom_left_origin { set; get; default = false; }
	public abstract uint version_major { set; get; default = 0; }
	public abstract uint version_minor { set; get; default = 0; }
	public abstract bool cache_context { set; get; default = false; }
	public abstract bool debug_context { set; get; default = false; }

	public abstract void context_reset ();
	public abstract ulong get_current_framebuffer ();
	public abstract ProcAdress get_proc_address (string sym);
	public abstract void context_destroy ();
}

private class CoreHardwareRender: Object, HardwareRender {
	[CCode (has_target = false)]
	internal delegate void ContextReset ();
	[CCode (has_target = false)]
	internal delegate ulong GetCurrentFramebuffer ();
	[CCode (has_target = false)]
	internal delegate HardwareRender.ProcAdress GetProcAdress (string sym);

	internal struct Callback {
		public HardwareRender.ContexType context_type;
		public ContextReset              context_reset;
		public GetCurrentFramebuffer     get_current_framebuffer;
		public GetProcAdress             get_proc_address;
		public bool                      depth;
		public bool                      stencil;
		public bool                      bottom_left_origin;
		public uint                      version_major;
		public uint                      version_minor;
		public bool                      cache_context;
		public ContextReset              context_destroy;
		public bool                      debug_context;
	}

	internal CoreHardwareRender (Callback callback_struct) {
		Object (callback_struct: callback_struct);
	}

	public Callback callback_struct { construct; private get; }
	public HardwareRender.ContexType context_type {
		set {
			callback_struct.context_type = value;
		}
		get {
			return callback_struct.context_type;
		}
	}

	public bool depth {
		set {
			callback_struct.depth = value;
		}
		get {
			return callback_struct.depth;
		}
	}

	public bool stencil {
		set {
			callback_struct.stencil = value;
		}
		get {
			return callback_struct.stencil;
		}
	}

	public bool bottom_left_origin {
		set {
			callback_struct.bottom_left_origin = value;
		}
		get {
			return callback_struct.bottom_left_origin;
		}
	}

	public uint version_major {
		set {
			callback_struct.version_major = value;
		}
		get {
			return callback_struct.version_major;
		}
	}

	public uint version_minor {
		set {
			callback_struct.version_minor = value;
		}
		get {
			return callback_struct.version_minor;
		}
	}

	public bool cache_context {
		set {
			callback_struct.cache_context = value;
		}
		get {
			return callback_struct.cache_context;
		}
	}

	public bool debug_context {
		set {
			callback_struct.debug_context = value;
		}
		get {
			return callback_struct.debug_context;
		}
	}

	public void context_reset () {
		if (callback_struct.context_reset != null) {
			callback_struct.context_reset ();
		}
	}

	public ulong get_current_framebuffer () {
		if (callback_struct.get_current_framebuffer != null) {
			return callback_struct.get_current_framebuffer ();
		}
		return (ulong) 0;
	}

	public HardwareRender.ProcAdress get_proc_address (string sym) {
		if (callback_struct.get_proc_address != null) {
			return callback_struct.get_proc_address (sym);
		}
		return (HardwareRender.ProcAdress) null;
	}

	public void context_destroy () {
		if (callback_struct.context_destroy != null) {
			callback_struct.context_destroy ();
		}
	}

}

}
