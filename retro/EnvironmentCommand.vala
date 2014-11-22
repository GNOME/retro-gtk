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

public enum EnvironmentCommand {
	EXPERIMENTAL_MASK             = 0x10000,
	PRIVATE_MASK                  = 0x20000,
	SET_ROTATION                  = 1,
	GET_OVERSCAN                  = 2,
	GET_CAN_DUPE                  = 3,
	SET_MESSAGE                   = 6,
	SHUTDOWN                      = 7,
	SET_PERFORMANCE_LEVEL         = 8,
	GET_SYSTEM_DIRECTORY          = 9,
	SET_PIXEL_FORMAT              = 10,
	SET_INPUT_DESCRIPTORS         = 11,
	SET_KEYBOARD_CALLBACK         = 12,
	SET_DISK_CONTROL_INTERFACE    = 13,
	SET_HW_RENDER                 = 14,
	GET_VARIABLE                  = 15,
	SET_VARIABLES                 = 16,
	GET_VARIABLE_UPDATE           = 17,
	SET_SUPPORT_NO_GAME           = 18,
	GET_LIBRETRO_PATH             = 19,
	SET_AUDIO_CALLBACK            = 22,
	SET_FRAME_TIME_CALLBACK       = 21,
	GET_RUMBLE_INTERFACE          = 23,
	GET_INPUT_DEVICE_CAPABILITIES = 24,
	GET_SENSOR_INTERFACE          = (25 | EnvironmentCommand.EXPERIMENTAL_MASK),
	GET_CAMERA_INTERFACE          = (26 | EnvironmentCommand.EXPERIMENTAL_MASK),
	GET_LOG_INTERFACE             = 27,
	GET_PERF_INTERFACE            = 28,
	GET_LOCATION_INTERFACE        = 29,
	GET_CONTENT_DIRECTORY         = 30,
	GET_SAVE_DIRECTORY            = 31,
	SET_SYSTEM_AV_INFO            = 32
}

}

