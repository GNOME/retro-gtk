// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * TODO Change visibility once the interface have been tested.
 */
internal enum CameraBuffer {
	OPENGL_TEXTURE,
	RAW_FRAMEBUFFER
}

/**
 * TODO Change visibility once the interface have been tested.
 */
internal interface Camera: Object {
	public abstract uint64 caps { set; get; }
	public abstract uint width { set; get; }
	public abstract uint height { set; get; }

	public abstract bool start ();
	public abstract void stop ();

	public abstract void frame_raw_framebuffer ([CCode (array_length = false)] uint32[] buffer, uint width, uint height, size_t pitch);
	public abstract void frame_opengl_texture (uint texture_id, uint texture_target, [CCode (array_length = false)] float[] affine);

	public abstract void initialized ();
	public abstract void deinitialized ();
}

}

