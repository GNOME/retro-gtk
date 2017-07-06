// This file is part of retro-gtk. License: GPLv3

namespace Retro {

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
	XRGB1555,

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

}
