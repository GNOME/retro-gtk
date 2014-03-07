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

namespace Hardware {
	public const uintptr_t FRAME_BUFFER_VALID = uintptr_t.MAX;
	
	[CCode (has_target = false)]
	public delegate void ContextReset ();
	[CCode (has_target = false)]
	public delegate uintptr_t GetCurrentFramebuffer ();
	[CCode (has_target = false)]
	public delegate void ProcAdress ();
	[CCode (has_target = false)]
	public delegate ProcAdress GetProcAdress (string sym);
	
	public enum ContexType {
		NONE = 0,
		OPENGL,
		OPENGLES2,
		OPENGL_CORE,
		OPENGLES3
	}
	
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

}
