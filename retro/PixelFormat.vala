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

/**
 * Pixel formats used by libretro.
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

}

