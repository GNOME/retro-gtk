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

#ifndef __RETRO_CORE_INTERFACES_H__
#define __RETRO_CORE_INTERFACES_H__

#include "retro-gobject-internal.h"

typedef struct {
	gpointer set_rumble_state;
} RetroRumbleCallback;

typedef struct {
	gpointer set_sensor_state;
	gpointer get_sensor_input;
} RetroSensorCallback;

typedef struct {
	guint64  caps;
	guint    width;
	guint    height;
	gpointer start;
	gpointer stop;
	gpointer frame_raw_framebuffer;
	gpointer frame_opengl_texture;
	gpointer initialized;
	gpointer deinitialized;
} RetroCameraCallback;

typedef struct {
	gpointer log;
} RetroLogCallback;

typedef struct {
	gpointer get_time_usec;
	gpointer get_cpu_features;
	gpointer get_perf_counter;
	gpointer perf_register;
	gpointer perf_start;
	gpointer perf_stop;
	gpointer perf_log;
} RetroPerformanceCallback;

typedef struct {
	gpointer start;
	gpointer stop;
	gpointer get_position;
	gpointer set_interval;
	gpointer initialized;
	gpointer deinitialized;
} RetroLocationCallback;

gboolean environment_interfaces_command (RetroCore *self, unsigned cmd, gpointer data);

#endif

