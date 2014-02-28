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

#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include <fcntl.h>
#include <dlfcn.h>

int create_temporary_file (retro_library_t *library);
void file_copy (const char *src, const char *dst);
void init_functions (retro_library_t *library);

void retro_library_construct (retro_library_t *library, const char *shared_object) {
	library->src_name = strdup (shared_object);
	
	int tmp_descriptor = create_temporary_file (library);
	file_copy (shared_object, library->tmp_name);
	
	library->handle = dlopen (library->tmp_name, RTLD_NOW);
	
	close (tmp_descriptor);
	
	if (! library->handle) fprintf (stderr, "%s: %s\n", shared_object, dlerror ());
	
	init_functions (library);
}

void retro_library_finalize (retro_library_t *library) {
	if (library) {
		if (library->handle) {
			dlclose (library->handle);
			library->handle = NULL;
		}
		
		if (library->tmp_name) {
			unlink (library->tmp_name);
			free (library->tmp_name);
			library->tmp_name = NULL;
		}
		
		if (library->src_name) {
			free (library->src_name);
			library->src_name = NULL;
		}
	}
}

retro_library_t *retro_library_new (const char *shared_object) {
	retro_library_t *library = (retro_library_t *) malloc (sizeof (retro_library_t));
	retro_library_construct (library, shared_object);
	
	return library;
}

void retro_library_free (retro_library_t *library) {
	if (library) {
		retro_library_finalize (library);
		free (library);
	}
}

// Helper functions

int create_temporary_file (retro_library_t *library) {
	const char *template = "/tmp/retro-library-XXXXXX";
	
	library->tmp_name = strdup (template);
	
	return mkstemp (library->tmp_name);
}

void file_copy (const char *src, const char *dst) {
	int src_desc = open (src, O_RDONLY);
	int dst_desc = open (dst, O_WRONLY);
	
	char buffer[4096];
	ssize_t count;
	
	while ((count = read (src_desc, buffer, 4096))) {
		write (dst_desc, buffer, count);
	}
	
	close (src_desc);
	close (dst_desc);
}

void init_functions (retro_library_t *library) {
	library->set_environment =
	(retro_library_set_environment_t) dlsym (library->handle, "retro_set_environment");
	
	library->set_video_refresh  =
	(retro_library_set_video_refresh_t) dlsym (library->handle, "retro_set_video_refresh");
	
	library->set_audio_sample =
	(retro_library_set_audio_sample_t) dlsym (library->handle, "retro_set_audio_sample");
	
	library->set_audio_sample_batch =
	(retro_library_set_audio_sample_batch_t) dlsym (library->handle, "retro_set_audio_sample_batch");
	
	library->set_input_poll =
	(retro_library_set_input_poll_t) dlsym (library->handle, "retro_set_input_poll");
	
	library->set_input_state =
	(retro_library_set_input_state_t) dlsym (library->handle, "retro_set_input_state");
	
	library->init =
	(retro_library_init_t) dlsym (library->handle, "retro_init");
	
	library->deinit =
	(retro_library_deinit_t) dlsym (library->handle, "retro_deinit");
	
	library->api_version =
	(retro_library_api_version_t) dlsym (library->handle, "retro_api_version");
	
	library->get_system_info =
	(retro_library_get_system_info_t) dlsym (library->handle, "retro_get_system_info");
	
	library->get_system_av_info =
	(retro_library_get_system_av_info_t) dlsym (library->handle, "retro_get_system_av_info");
	
	library->set_controller_port_device =
	(retro_library_set_controller_port_device_t) dlsym (library->handle, "retro_set_controller_port_device");
	
	library->reset =
	(retro_library_reset_t) dlsym (library->handle, "retro_reset");
	
	library->run =
	(retro_library_run_t) dlsym (library->handle, "retro_run");
	
	library->serialize_size =
	(retro_library_serialize_size_t) dlsym (library->handle, "retro_serialize_size");
	
	library->serialize =
	(retro_library_serialize_t) dlsym (library->handle, "retro_serialize");
	
	library->unserialize =
	(retro_library_unserialize_t) dlsym (library->handle, "retro_unserialize");
	
	library->cheat_reset =
	(retro_library_cheat_reset_t) dlsym (library->handle, "retro_cheat_reset");
	
	library->cheat_set =
	(retro_library_cheat_set_t) dlsym (library->handle, "retro_cheat_set");
	
	library->load_game =
	(retro_library_load_game_t) dlsym (library->handle, "retro_load_game");
	
	library->load_game_special =
	(retro_library_load_game_special_t) dlsym (library->handle, "retro_load_game_special");
	
	library->unload_game =
	(retro_library_unload_game_t) dlsym (library->handle, "retro_unload_game");
	
	library->get_region =
	(retro_library_get_region_t) dlsym (library->handle, "retro_get_region");
	
	library->get_memory_data =
	(retro_library_get_memory_data_t) dlsym (library->handle, "retro_get_memory_data");
	
	library->get_memory_size =
	(retro_library_get_memory_size_t) dlsym (library->handle, "retro_get_memory_size");
}

