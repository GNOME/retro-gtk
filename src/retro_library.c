/* retro_library.c
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

#include "retro_library.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <dlfcn.h>

int create_temporary_file (retro_library_t *library) {
	char *template = "/tmp/retro-library-XXXXXX";
	
	library->tmp_name = strdup (template);
	
	return mkstemp (library->tmp_name);
}

void file_copy (char *src, char *dst) {
	int src_desc = open (src, O_RDONLY);
	int dst_desc = open (dst, O_WRONLY);
	
	char buffer[8];
	ssize_t count;
	
	while ((count = read (src_desc, buffer, 8))) {
		write (dst_desc, buffer, count);
	}
	
	close (src_desc);
	close (dst_desc);
}

retro_library_t *retro_library_new (char *shared_object) {
	retro_library_t *library = (retro_library_t *) malloc (sizeof (retro_library_t));
	
	library->src_name = strdup (shared_object);
	
	int tmp_descriptor = create_temporary_file (library);
	file_copy (shared_object, library->tmp_name);
	
	library->handle = dlopen (library->tmp_name, RTLD_NOW);
	
	close (tmp_descriptor);
	
	if (! library) fprintf (stderr, "%s\n", shared_object, dlerror ());
	
	return library;
}

void retro_library_free (retro_library_t *library) {
	if (library) {
		if (library->handle) {
			dlclose (library->handle);
		}
		
		if (library->tmp_name) {
			unlink (library->tmp_name);
			free (library->tmp_name);
		}
		
		if (library->src_name) {
			free (library->src_name);
		}
		
		free (library);
	}
}

void retro_library_set_environment(retro_library_t *library, retro_environment_t environment) {
	void (*f)(retro_environment_t) = dlsym (library->handle, "retro_set_environment");
	f (environment);
}

void retro_library_set_video_refresh(retro_library_t *library, retro_video_refresh_t video_refresh) {
	void (*f)(retro_video_refresh_t) = dlsym (library->handle, "retro_set_video_refresh");
	f (video_refresh);
}

void retro_library_set_audio_sample(retro_library_t *library, retro_audio_sample_t audio_sample) {
	void (*f)(retro_audio_sample_t) = dlsym (library->handle, "retro_set_audio_sample");
	f (audio_sample);
}

void retro_library_set_audio_sample_batch(retro_library_t *library, retro_audio_sample_batch_t audio_sample_batch) {
	void (*f)(retro_audio_sample_batch_t) = dlsym (library->handle, "retro_set_audio_sample_batch");
	f (audio_sample_batch);
}

void retro_library_set_input_poll(retro_library_t *library, retro_input_poll_t input_poll) {
	void (*f)(retro_input_poll_t) = dlsym (library->handle, "retro_set_input_poll");
	f (input_poll);
}

void retro_library_set_input_state(retro_library_t *library, retro_input_state_t input_state) {
	void (*f)(retro_input_state_t) = dlsym (library->handle, "retro_set_input_state");
	f (input_state);
}

void retro_library_init(retro_library_t *library) {
	void (*f)(void) = dlsym (library->handle, "retro_init");
	f ();
}

void retro_library_deinit(retro_library_t *library) {
	void (*f)(void) = dlsym (library->handle, "retro_deinit");
	f ();
}

unsigned retro_library_api_version(retro_library_t *library) {
	unsigned (*f)(void) = dlsym (library->handle, "retro_api_version");
	return f ();
}

void retro_library_get_system_info(retro_library_t *library, struct retro_system_info *info) {
	void (*f)(struct retro_system_info *) = dlsym (library->handle, "retro_get_system_info");
	f (info);
}

void retro_library_get_system_av_info(retro_library_t *library, struct retro_system_av_info *info) {
	void (*f)(struct retro_system_av_info *) = dlsym (library->handle, "retro_get_system_info");
	f (info);
}

void retro_library_reset(retro_library_t *library) {
	void (*f)(void) = dlsym (library->handle, "retro_reset");
	f ();
}

void retro_library_run(retro_library_t *library) {
	void (*f)(void) = dlsym (library->handle, "retro_run");
	f ();
}

bool retro_library_load_game(retro_library_t *library, const struct retro_game_info *game) {
	bool (*f)(const struct retro_game_info *) = dlsym (library->handle, "retro_load_game");
	return f (game);
}

