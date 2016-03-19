// This file is part of Retro. License: GPLv3

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

