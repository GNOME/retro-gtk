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

#ifndef __RETRO_ENVIRONMENT_COMMAND_H__
#define __RETRO_ENVIRONMENT_COMMAND_H__

typedef enum {
	RETRO_ENVIRONMENT_COMMAND_EXPERIMENTAL_MASK             = 0x10000,
	RETRO_ENVIRONMENT_COMMAND_PRIVATE_MASK                  = 0x20000,
	RETRO_ENVIRONMENT_COMMAND_SET_ROTATION                  = 1,
	RETRO_ENVIRONMENT_COMMAND_GET_OVERSCAN                  = 2,
	RETRO_ENVIRONMENT_COMMAND_GET_CAN_DUPE                  = 3,
	RETRO_ENVIRONMENT_COMMAND_SET_MESSAGE                   = 6,
	RETRO_ENVIRONMENT_COMMAND_SHUTDOWN                      = 7,
	RETRO_ENVIRONMENT_COMMAND_SET_PERFORMANCE_LEVEL         = 8,
	RETRO_ENVIRONMENT_COMMAND_GET_SYSTEM_DIRECTORY          = 9,
	RETRO_ENVIRONMENT_COMMAND_SET_PIXEL_FORMAT              = 10,
	RETRO_ENVIRONMENT_COMMAND_SET_INPUT_DESCRIPTORS         = 11,
	RETRO_ENVIRONMENT_COMMAND_SET_KEYBOARD_CALLBACK         = 12,
	RETRO_ENVIRONMENT_COMMAND_SET_DISK_CONTROL_INTERFACE    = 13,
	RETRO_ENVIRONMENT_COMMAND_SET_HW_RENDER                 = 14,
	RETRO_ENVIRONMENT_COMMAND_GET_VARIABLE                  = 15,
	RETRO_ENVIRONMENT_COMMAND_SET_VARIABLES                 = 16,
	RETRO_ENVIRONMENT_COMMAND_GET_VARIABLE_UPDATE           = 17,
	RETRO_ENVIRONMENT_COMMAND_SET_SUPPORT_NO_GAME           = 18,
	RETRO_ENVIRONMENT_COMMAND_GET_LIBRETRO_PATH             = 19,
	RETRO_ENVIRONMENT_COMMAND_SET_AUDIO_CALLBACK            = 22,
	RETRO_ENVIRONMENT_COMMAND_SET_FRAME_TIME_CALLBACK       = 21,
	RETRO_ENVIRONMENT_COMMAND_GET_RUMBLE_INTERFACE          = 23,
	RETRO_ENVIRONMENT_COMMAND_GET_INPUT_DEVICE_CAPABILITIES = 24,
	RETRO_ENVIRONMENT_COMMAND_GET_SENSOR_INTERFACE          = (25 | RETRO_ENVIRONMENT_COMMAND_EXPERIMENTAL_MASK),
	RETRO_ENVIRONMENT_COMMAND_GET_CAMERA_INTERFACE          = (26 | RETRO_ENVIRONMENT_COMMAND_EXPERIMENTAL_MASK),
	RETRO_ENVIRONMENT_COMMAND_GET_LOG_INTERFACE             = 27,
	RETRO_ENVIRONMENT_COMMAND_GET_PERF_INTERFACE            = 28,
	RETRO_ENVIRONMENT_COMMAND_GET_LOCATION_INTERFACE        = 29,
	RETRO_ENVIRONMENT_COMMAND_GET_CONTENT_DIRECTORY         = 30,
	RETRO_ENVIRONMENT_COMMAND_GET_SAVE_DIRECTORY            = 31,
	RETRO_ENVIRONMENT_COMMAND_SET_SYSTEM_AV_INFO            = 32
} RetroEnvironmentCommand;

#endif
