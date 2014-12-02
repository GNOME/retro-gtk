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

#include "retro-environment-interfaces.h"
#include "libretro-environment.h"

inline gboolean environment_interfaces_command (RetroCore *self, unsigned cmd, gpointer data) {
	if (!self) return FALSE;

	switch (cmd) {
		case RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE:
			return get_rumble_callback (self, (RetroRumbleCallback *) data);

		case RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE:
			return get_sensor_callback (self, (RetroSensorCallback *) data);

		case RETRO_ENVIRONMENT_GET_CAMERA_INTERFACE:
			return get_camera_callback (self, (RetroCameraCallback *) data);

		case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
			return get_log_callback (self, (RetroLogCallback *) data);

		case RETRO_ENVIRONMENT_GET_PERF_INTERFACE:
			return get_performance_callback (self, (RetroPerformanceCallback *) data);

		case RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE:
			return get_location_callback (self, (RetroLocationCallback *) data);

		default:
			return FALSE;
	}
}

inline gboolean get_rumble_callback (RetroCore *self, RetroRumbleCallback *cb) {
	void *interface_exists = retro_core_get_rumble_interface (self);
	if (!interface_exists) return FALSE;

	gboolean real_set_rumble_state (guint port, RetroRumbleEffect effect, guint16 strength) {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_val_if_reached (FALSE);

		RetroRumble *interface = retro_core_get_rumble_interface (cb_data);
		if (!interface) g_return_val_if_reached (FALSE);

		return RETRO_RUMBLE_GET_INTERFACE (interface)->set_rumble_state (interface, port, effect, strength);
	}

	cb->set_rumble_state = real_set_rumble_state;

	return TRUE;
}

inline gboolean get_sensor_callback (RetroCore *self, RetroSensorCallback *cb) {
	void *interface_exists = retro_core_get_sensor_interface (self);
	if (!interface_exists) return FALSE;

	gboolean real_set_sensor_state (guint port, RetroSensorAction action, guint rate) {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_val_if_reached (FALSE);

		RetroSensor *interface = retro_core_get_sensor_interface (cb_data);
		if (!interface) g_return_val_if_reached (FALSE);

		return RETRO_SENSOR_GET_INTERFACE (interface)->set_sensor_state (interface, port, action, rate);
	}

	gfloat real_get_sensor_input (guint port, guint id) {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_val_if_reached (0.0);

		RetroSensor *interface = retro_core_get_sensor_interface (cb_data);
		if (!interface) g_return_val_if_reached (0.0);

		return RETRO_SENSOR_GET_INTERFACE (interface)->get_sensor_input (interface, port, id);
	}

	cb->set_sensor_state = real_set_sensor_state;
	cb->get_sensor_input = real_get_sensor_input;

	return TRUE;
}

inline gboolean get_camera_callback (RetroCore *self, RetroCameraCallback *cb) {
	void *interface_exists = retro_core_get_camera_interface (self);
	if (!interface_exists) return FALSE;

	gboolean real_start () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_val_if_reached (FALSE);

		RetroCamera *interface = retro_core_get_camera_interface (cb_data);
		if (!interface) g_return_val_if_reached (FALSE);

		return RETRO_CAMERA_GET_INTERFACE (interface)->start (interface);
	}

	void real_stop () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroCamera *interface = retro_core_get_camera_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_CAMERA_GET_INTERFACE (interface)->stop (interface);
	}

	void real_frame_raw_framebuffer (guint32 *buffer, guint width, guint height, gsize pitch) {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroCamera *interface = retro_core_get_camera_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_CAMERA_GET_INTERFACE (interface)->frame_raw_framebuffer (interface, buffer, width, height, pitch);
	}

	void real_frame_opengl_texture (guint texture_id, guint texture_target, gfloat *affine) {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroCamera *interface = retro_core_get_camera_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_CAMERA_GET_INTERFACE (interface)->frame_opengl_texture (interface, texture_id, texture_target, affine);
	}

	void real_initialized () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroCamera *interface = retro_core_get_camera_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_CAMERA_GET_INTERFACE (interface)->initialized (interface);
	}

	void real_deinitialized () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroCamera *interface = retro_core_get_camera_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_CAMERA_GET_INTERFACE (interface)->deinitialized (interface);
	}

	RetroCamera *interface = retro_core_get_camera_interface (self);

	cb->caps = RETRO_CAMERA_GET_INTERFACE (interface)->get_caps (interface);
	cb->width = RETRO_CAMERA_GET_INTERFACE (interface)->get_width (interface);
	cb->height = RETRO_CAMERA_GET_INTERFACE (interface)->get_height (interface);
	cb->start = real_start;
	cb->stop = real_stop;
	cb->frame_raw_framebuffer = real_frame_raw_framebuffer;
	cb->frame_opengl_texture = real_frame_opengl_texture;
	cb->initialized = real_initialized;
	cb->deinitialized = real_deinitialized;

	return TRUE;
}

inline gboolean get_log_callback (RetroCore *self, RetroLogCallback *cb) {
	void *interface_exists = retro_core_get_log_interface (self);
	if (!interface_exists) return FALSE;

	void real_log (guint level, const char *format, ...) {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroLog *interface = retro_core_get_log_interface (cb_data);
		if (!interface) g_return_if_reached ();

		// Get the arguments, set up the formatted message,
		// pass it to the logging method and free it.
		va_list args;
		va_start (args, format);
		char *message = g_strdup_vprintf (format, args);
		RETRO_LOG_GET_INTERFACE (interface)->log (interface, level, message);
		g_free (message);
	}

	cb->log = real_log;

	return TRUE;
}

inline gboolean get_performance_callback (RetroCore *self, RetroPerformanceCallback *cb) {
	void *interface_exists = retro_core_get_performance_interface (self);
	if (!interface_exists) return FALSE;

	gint64 real_get_time_usec () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_val_if_reached (0);

		RetroPerformance *interface = retro_core_get_performance_interface (cb_data);
		if (!interface) g_return_val_if_reached (0);

		return RETRO_PERFORMANCE_GET_INTERFACE (interface)->get_time_usec (interface);
	}

	guint64 real_get_cpu_features () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_val_if_reached (0);

		RetroPerformance *interface = retro_core_get_performance_interface (cb_data);
		if (!interface) g_return_val_if_reached (0);

		return RETRO_PERFORMANCE_GET_INTERFACE (interface)->get_cpu_features (interface);
	}

	guint64 real_get_perf_counter () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_val_if_reached (0);

		RetroPerformance *interface = retro_core_get_performance_interface (cb_data);
		if (!interface) g_return_val_if_reached (0);

		return RETRO_PERFORMANCE_GET_INTERFACE (interface)->get_perf_counter (interface);
	}

	void real_perf_register (RetroPerfCounter *counter) {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroPerformance *interface = retro_core_get_performance_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_PERFORMANCE_GET_INTERFACE (interface)->perf_register (interface, counter);
	}

	void real_perf_start (RetroPerfCounter *counter) {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroPerformance *interface = retro_core_get_performance_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_PERFORMANCE_GET_INTERFACE (interface)->perf_start (interface, counter);
	}

	void real_perf_stop (RetroPerfCounter *counter) {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroPerformance *interface = retro_core_get_performance_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_PERFORMANCE_GET_INTERFACE (interface)->perf_stop (interface, counter);
	}

	void real_perf_log () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroPerformance *interface = retro_core_get_performance_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_PERFORMANCE_GET_INTERFACE (interface)->perf_log (interface);
	}

	cb->get_time_usec = real_get_time_usec;
	cb->get_cpu_features = real_get_cpu_features;
	cb->get_perf_counter = real_get_perf_counter;
	cb->perf_register = real_perf_register;
	cb->perf_start = real_perf_start;
	cb->perf_stop = real_perf_stop;
	cb->perf_log = real_perf_log;

	return TRUE;
}

inline gboolean get_location_callback (RetroCore *self, RetroLocationCallback *cb) {
	void *interface_exists = retro_core_get_location_interface (self);
	if (!interface_exists) return FALSE;

	gboolean real_start () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_val_if_reached (FALSE);

		RetroLocation *interface = retro_core_get_location_interface (cb_data);
		if (!interface) g_return_val_if_reached (FALSE);

		return RETRO_LOCATION_GET_INTERFACE (interface)->start (interface);
	}

	void real_stop () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroLocation *interface = retro_core_get_location_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_LOCATION_GET_INTERFACE (interface)->stop (interface);
	}

	gboolean real_get_position (gdouble *lat, gdouble *lon, gdouble *horiz_accuracy, gdouble *vert_accuracy) {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_val_if_reached (FALSE);

		RetroLocation *interface = retro_core_get_location_interface (cb_data);
		if (!interface) g_return_val_if_reached (FALSE);

		return RETRO_LOCATION_GET_INTERFACE (interface)->get_position (interface, lat, lon, horiz_accuracy, vert_accuracy);
	}

	void real_set_interval (guint interval_ms, guint interval_distance) {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroLocation *interface = retro_core_get_location_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_LOCATION_GET_INTERFACE (interface)->set_interval (interface, interval_ms, interval_distance);
	}

	void real_initialized () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroLocation *interface = retro_core_get_location_interface (cb_data);
		if (!interface) g_return_if_reached ();

		RETRO_LOCATION_GET_INTERFACE (interface)->initialized (interface);
	}

	void real_deinitialized () {
		RetroCore *cb_data = retro_core_get_cb_data ();
		if (!cb_data) g_return_if_reached ();

		RetroLocation *interface = retro_core_get_location_interface (cb_data);
		if (!interface) g_return_if_reached ();

			RETRO_LOCATION_GET_INTERFACE (interface)->deinitialized (interface);
	}

	cb->start = real_start;
	cb->stop = real_stop;
	cb->get_position = real_get_position;
	cb->set_interval = real_set_interval;
	cb->initialized = real_initialized;
	cb->deinitialized = real_deinitialized;

	return TRUE;
}

