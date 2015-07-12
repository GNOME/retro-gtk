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
