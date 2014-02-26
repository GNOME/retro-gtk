/* retro_core.c
 * Copyright (c) 2014, Adrien Plazas, All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

#include "retro_core.h"

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include <pthread.h>

/* 
 * A thread local global variable used to mimic a closure.
 */
static __thread retro_core_t *thread_global_retro_core;

// Run a function isolated
void run_isolated (retro_core_t *this, void (*func) (void *), char *thread_name);

void retro_core_construct (retro_core_t *this, char *library_path) {
	this->library = retro_library_new (library_path);
	
	this->environment_cb = NULL;
	this->video_refresh_cb = NULL;
	this->audio_sample_cb = NULL;
	this->audio_sample_batch_cb = NULL;
	this->input_poll_cb = NULL;
	this->input_state_cb = NULL;
	
	this->environment_data = NULL;
	this->video_refresh_data = NULL;
	this->audio_sample_data = NULL;
	this->audio_sample_batch_data = NULL;
	this->input_poll_data = NULL;
	this->input_state_data = NULL;
}

void retro_core_finalize (retro_core_t *this) {
	if (this) {
		retro_core_deinit (this);
		retro_library_free (this->library);
		
		this->library = NULL;
		
		this->environment_cb = NULL;
		this->video_refresh_cb = NULL;
		this->audio_sample_cb = NULL;
		this->audio_sample_batch_cb = NULL;
		this->input_poll_cb = NULL;
		this->input_state_cb = NULL;
		
		this->environment_data = NULL;
		this->video_refresh_data = NULL;
		this->audio_sample_data = NULL;
		this->audio_sample_batch_data = NULL;
		this->input_poll_data = NULL;
		this->input_state_data = NULL;
	}
}

retro_core_t *retro_core_new       (char *library_path) {
	retro_core_t *this = (retro_core_t *) malloc (sizeof (retro_core_t));
	retro_core_construct (this, library_path);
	
	return this;
}

void retro_core_free       (retro_core_t *this) {
	if (this) {
		retro_core_finalize (this);
		free (this);
	}
}

/*
 * Callback setters.
 * 
 * this: a pointer to a retro_core_t to modify.
 * cb:   the callback to set.
 * 
 * lambda: a phony closure, retrieving the value of this from the
 * thread local global variable thread_global_retro_core.
 * 
 * Set lambda as a callback for this->library
 * 
 * If the libretro library call back from an invalid environment
 * (without thread_global_retro_core pointing to a valid retro_core_t)
 * the program will fail (g_assert_not_reached will be called).
 * 
 * These callbacks must be set with thread_global_retro_core set because
 * the libretro library could call back even from a callback setter.
 */

void retro_core_set_environment (retro_core_t *this, retro_core_environment_cb_t cb, void *user_data) {
	// The callback to pass to the libretro library
	bool real_cb (unsigned cmd, void *data) {
		retro_core_t *core = thread_global_retro_core;
		if (core) {
			return core->environment_cb (cmd, data, core->environment_data);
		}
	
		g_assert_not_reached ();
		return false;
	}
	
	void lambda () {
		this->environment_cb = cb;
		this->environment_data = user_data;
		this->library->set_environment (real_cb);
	}
	
	run_isolated (this, lambda, "retro_core_set_environment");
}

void retro_core_set_video_refresh (retro_core_t *this, retro_core_video_refresh_cb_t cb, void *user_data) {
	// The callback to pass to the libretro library
	void real_cb (const void *data, unsigned width, unsigned height, size_t pitch) {
		retro_core_t *core = thread_global_retro_core;
		if (core) {
			core->video_refresh_cb (data, width, height, pitch, core->video_refresh_data);
			return;
		}
		
		g_assert_not_reached ();
	}
	
	void lambda () {
		this->video_refresh_cb = cb;
		this->video_refresh_data = user_data;
		this->library->set_video_refresh (real_cb);
	}
	
	run_isolated (this, lambda, "retro_core_set_video_refresh");
}

void retro_core_set_audio_sample (retro_core_t *this, retro_core_audio_sample_cb_t cb, void *user_data) {
	// The callback to pass to the libretro library
	void real_cb (int16_t left, int16_t right) {
		retro_core_t *core = thread_global_retro_core;
		if (core) {
			core->audio_sample_cb (left, right, core->audio_sample_data);
			return;
		}
		
		g_assert_not_reached ();
	}
	
	void lambda () {
		this->audio_sample_cb = cb;
		this->audio_sample_data = user_data;
		this->library->set_audio_sample (real_cb);
	}
	
	run_isolated (this, lambda, "retro_core_set_audio_sample");
}

void retro_core_set_audio_sample_batch (retro_core_t *this, retro_core_audio_sample_batch_cb_t cb, void *user_data) {
	// The callback to pass to the libretro library
	size_t real_cb (const int16_t *data, size_t frames) {
		retro_core_t *core = thread_global_retro_core;
		if (core) {
			return core->audio_sample_batch_cb (data, frames, core->audio_sample_batch_data);
		}
		
		g_assert_not_reached ();
		return 0;
	}
	
	void lambda () {
		this->audio_sample_batch_cb = cb;
		this->audio_sample_batch_data = user_data;
		this->library->set_audio_sample_batch (real_cb);
	}
	
	run_isolated (this, lambda, "retro_core_set_audio_sample_batch");
}

void retro_core_set_input_poll (retro_core_t *this, retro_core_input_poll_cb_t cb, void *user_data) {
	// The callback to pass to the libretro library
	void real_cb (void) {
		retro_core_t *core = thread_global_retro_core;
		if (core) {
			return core->input_poll_cb (core->input_poll_data);
		}
		
		g_assert_not_reached ();
	}
	
	void lambda () {
		this->input_poll_cb = cb;
		this->input_poll_data = user_data;
		this->library->set_input_poll (real_cb);
	}
	
	run_isolated (this, lambda, "retro_core_set_input_poll");
}

void retro_core_set_input_state (retro_core_t *this, retro_core_input_state_cb_t cb, void *user_data) {
	// The callback to pass to the libretro library
	int16_t real_cb (unsigned port, unsigned device, unsigned index, unsigned id) {
		retro_core_t *core = thread_global_retro_core;
		if (core) {
			return core->input_state_cb (port, device, index, id, core->input_state_data);
		}
		
		g_assert_not_reached ();
		return 0;
	}
	
	void lambda () {
		this->input_state_cb = cb;
		this->input_state_data = user_data;
		this->library->set_input_state (real_cb);
	}
	
	run_isolated (this, lambda, "retro_core_set_input_state");
}

/* 
 * The next functions start a new thread, store the caller (this) in the thread
 * local global variable thread_global_retro_core and call the actual
 * library function from within this thread.
 * 
 * Callbacks called within the library function will live in an environment where
 * thread_global_retro_core as the same value as the function's caller (this).
 * 
 * It allows to mimic a closure where _this_ exists without interfering with
 * other instances of retro_core_t or libretro and so to isolate libretro libraries
 * and their environment, overriding libretro's design faults (an unneeded singleton
 * pattern).
 * 
 * If the thread creation fail, the program will fail (g_assert_not_reached will be
 * called).
 */

void retro_core_init (retro_core_t *this) {
	void lambda () {
		this->library->init ();
	}
	
	run_isolated (this, lambda, "retro_core_init");
}

void retro_core_deinit (retro_core_t *this) {
	void lambda () {
		this->library->deinit ();
	}
	
	run_isolated (this, lambda, "retro_core_deinit");
}

unsigned retro_core_api_version (retro_core_t *this) {
	unsigned result;
	
	void lambda () {
		result = this->library->api_version ();
	}
	
	run_isolated (this, lambda, "retro_core_api_version");
	
	return result;
}

void retro_core_get_system_info (retro_core_t *this, struct retro_system_info *info) {
	void lambda () {
		this->library->get_system_info (info);
	}
	
	run_isolated (this, lambda, "retro_core_get_system_info");
}

void retro_core_get_system_av_info (retro_core_t *this, struct retro_system_av_info *info) {
	void lambda () {
		this->library->get_system_av_info (info);
	}
	
	run_isolated (this, lambda, "retro_core_get_system_av_info");
}

void retro_core_set_controller_port_device (retro_core_t *this, unsigned port, unsigned device) {
	void lambda () {
		this->library->set_controller_port_device (port, device);
	}
	
	run_isolated (this, lambda, "retro_core_set_controller_port_device");
}

void retro_core_reset (retro_core_t *this) {
	void lambda () {
		this->library->reset ();
	}
	
	run_isolated (this, lambda, "retro_core_reset");
}

void retro_core_run (retro_core_t *this) {
	void lambda () {
		this->library->run ();
	}
	
	run_isolated (this, lambda, "retro_core_run");
}

size_t retro_core_serialize_size (retro_core_t *this) {
	size_t result;
	
	void lambda () {
		result = this->library->serialize_size ();
	}
	
	run_isolated (this, lambda, "retro_core_serialize_size");
	
	return result;
}

bool retro_core_serialize (retro_core_t *this, void *data, size_t size) {
	bool result;
	
	void lambda () {
		result = this->library->serialize (data, size);
	}
	
	run_isolated (this, lambda, "retro_core_serialize");
	
	return result;
}

bool retro_core_unserialize (retro_core_t *this, const void *data, size_t size) {
	bool result;
	
	void lambda () {
		result = this->library->unserialize (data, size);
	}
	
	run_isolated (this, lambda, "retro_core_unserialize");
	
	return result;
}

void retro_core_cheat_reset (retro_core_t *this) {
	void lambda () {
		this->library->cheat_reset ();
	}
	
	run_isolated (this, lambda, "retro_core_cheat_reset");
}

void retro_core_cheat_set (retro_core_t *this, unsigned index, bool enabled, const char *code) {
	void lambda () {
		this->library->cheat_set (index, enabled, code);
	}
	
	run_isolated (this, lambda, "retro_core_cheat_set");
}

bool retro_core_load_game (retro_core_t *this, const struct retro_game_info *game) {
	bool result;
	
	void lambda () {
		result = this->library->load_game(game);
	}
	
	run_isolated (this, lambda, "retro_core_load_game");
	
	return result;
}

bool retro_core_load_game_special (retro_core_t *this, unsigned game_type, const struct retro_game_info *info, size_t num_info) {
	bool result;
	
	void lambda () {
		result = this->library->load_game_special(game_type, info, num_info);
	}
	
	run_isolated (this, lambda, "retro_core_load_game_special");
	
	return result;
}

void retro_core_unload_game (retro_core_t *this) {
	void lambda () {
		this->library->unload_game ();
	}
	
	run_isolated (this, lambda, "retro_core_unload_game");
}

unsigned retro_core_get_region (retro_core_t *this) {
	unsigned result;
	
	void lambda () {
		result = this->library->get_region();
	}
	
	run_isolated (this, lambda, "retro_core_load_get_region");
	
	return result;
}

void *retro_core_get_memory_data (retro_core_t *this, unsigned id) {
	void lambda () {
		this->library->get_memory_data (id);
	}
	
	run_isolated (this, lambda, "retro_core_get_memory_data");
}

size_t retro_core_get_memory_size (retro_core_t *this, unsigned id) {
	size_t result;
	
	void lambda () {
		result = this->library->get_memory_size(id);
	}
	
	run_isolated (this, lambda, "retro_core_get_memory_size");
	
	return result;
}

void run_isolated (retro_core_t *this, void (*func) (), char *thread_name) {
	if (thread_global_retro_core == this) {
		func ();
	}
	else {
		pthread_t thread;
		
		void *lambda (void *args) {
			thread_global_retro_core = this;
			func ();
			pthread_exit (NULL);
		}
		
		if (pthread_create (&thread, NULL, lambda, thread_name) < 0) {
			fprintf (stderr, "pthread_create error for thread %s\n", thread_name);
			g_assert_not_reached ();
		}
		
		pthread_join (thread, NULL);
	}
}

