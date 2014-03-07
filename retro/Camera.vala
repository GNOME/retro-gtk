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

namespace Camera {
	public enum Buffer {
		OPENGL_TEXTURE,
		RAW_FRAMEBUFFER
	}
	
	[CCode (has_target = false)]
	public delegate bool Start ();
	
	[CCode (has_target = false)]
	public delegate void Stop ();
	
	[CCode (has_target = false)]
	public delegate void LifetimeStatus ();
	
	[CCode (has_target = false)]
	public delegate void FrameRawFramebuffer ([CCode (array_length = false)] uint32[] buffer, uint width, uint height, size_t pitch);
	
	[CCode (has_target = false)]
	public delegate void FrameOpenglTexture (uint texture_id, uint texture_target, [CCode (array_length = false)] float[] affine);
	
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

}

