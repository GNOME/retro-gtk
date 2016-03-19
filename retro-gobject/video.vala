// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * The rotation type of a display.
 */
public enum Rotation {
	NONE,
	COUNTERCLOCKWISE,
	UPSIDEDOWN,
	CLOCKWISE
}

/**
 * Pixel formats used by Libretro.
 */
public enum PixelFormat {
	/**
	 * 0RGB1555, native endian.
	 *
	 * 0 bit must be set to 0.
	 * This pixel format is default for compatibility concerns only.
	 * If a 15/16-bit pixel format is desired, consider using RGB565.
	 */
	ORGB1555,

	/**
	 * XRGB8888, native endian. X bits are ignored.
	 */
	XRGB8888,

	/**
	 * RGB565, native endian.
	 *
	 * This pixel format is the recommended format to use if a 15/16-bit format is desired
	 * as it is the pixel format that is typically available on a wide range of low-power devices.
	 * It is also natively supported in APIs like OpenGL ES.
	 */
	RGB565,

	/**
	 * Unknown format.
	 */
	UNKNOWN = -1;
}

/**
 * An interface to render a {@link Core}'s video.
 */
public interface Video : Object {
	/**
	 * The core to handle video from.
	 */
	public abstract Core core { get; set; }

	/**
	 * Ask to render video.
	 */
	public abstract void render (uint8[] data, uint width, uint height, size_t pitch);

	/**
	 * The rotation of the image sent to {@link render}.
	 *
	 * It will be set by the {@link core}.
	 */
	public abstract Rotation rotation { get; set; }

	/**
	 * Ask the core to render with overscan or not.
	 */
	public abstract bool overscan { get; set; }

	/**
	 * Warn the {@link core} that it can dupe frames.
	 *
	 * If it does so, the data passed to {@link render} may be //null//.
	 */
	public abstract bool can_dupe { get; set; }

	/**
	 * The pixel format of the data sent to {@link render}.
	 *
	 * It will be set by the {@link core}.
	 */
	public abstract PixelFormat pixel_format { get; set; }
}

}
