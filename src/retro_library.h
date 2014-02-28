/* retro_library.h
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

#ifndef __RETRO_LIBRARY_H__
#define __RETRO_LIBRARY_H__

#include "libretro.h"

typedef struct retro_library retro_library_t;

typedef void (*retro_library_set_environment_t)        (retro_environment_t);
typedef void (*retro_library_set_video_refresh_t)      (retro_video_refresh_t);
typedef void (*retro_library_set_audio_sample_t)       (retro_audio_sample_t);
typedef void (*retro_library_set_audio_sample_batch_t) (retro_audio_sample_batch_t);
typedef void (*retro_library_set_input_poll_t)         (retro_input_poll_t);
typedef void (*retro_library_set_input_state_t)        (retro_input_state_t);

typedef void (*retro_library_init_t) (void);
typedef void (*retro_library_deinit_t) (void);

typedef unsigned (*retro_library_api_version_t) (void);

typedef void (*retro_library_get_system_info_t) (struct retro_system_info *info);
typedef void (*retro_library_get_system_av_info_t) (struct retro_system_av_info *info);

typedef void (*retro_library_set_controller_port_device_t) (unsigned port, unsigned device);

typedef void (*retro_library_reset_t) (void);
typedef void (*retro_library_run_t) (void);

typedef size_t (*retro_library_serialize_size_t) (void);
typedef bool   (*retro_library_serialize_t) (void *data, size_t size);
typedef bool   (*retro_library_unserialize_t) (const void *data, size_t size);

typedef void (*retro_library_cheat_reset_t) (void);
typedef void (*retro_library_cheat_set_t) (unsigned index, bool enabled, const char *code);

typedef bool (*retro_library_load_game_t) (const struct retro_game_info *game);
typedef bool (*retro_library_load_game_special_t) (unsigned game_type, const struct retro_game_info *info, size_t num_info);
typedef void (*retro_library_unload_game_t) (void);

typedef unsigned (*retro_library_get_region_t) (void);

typedef void  *(*retro_library_get_memory_data_t) (unsigned id);
typedef size_t (*retro_library_get_memory_size_t) (unsigned id);

struct retro_library {
	void *handle;
	char *src_name;
	char *tmp_name;
	
	retro_library_set_environment_t        set_environment;
	retro_library_set_video_refresh_t      set_video_refresh;
	retro_library_set_audio_sample_t       set_audio_sample;
	retro_library_set_audio_sample_batch_t set_audio_sample_batch;
	retro_library_set_input_poll_t         set_input_poll;
	retro_library_set_input_state_t        set_input_state;
	
	retro_library_init_t   init;
	retro_library_deinit_t deinit;
	
	retro_library_api_version_t api_version;
	
	retro_library_get_system_info_t    get_system_info;
	retro_library_get_system_av_info_t get_system_av_info;
	
	retro_library_set_controller_port_device_t set_controller_port_device;
	
	retro_library_reset_t reset;
	retro_library_run_t   run;
	
	retro_library_serialize_size_t serialize_size;
	retro_library_serialize_t      serialize;
	retro_library_unserialize_t    unserialize;
	
	retro_library_cheat_reset_t cheat_reset;
	retro_library_cheat_set_t   cheat_set;
	
	retro_library_load_game_t         load_game;
	retro_library_load_game_special_t load_game_special;
	retro_library_unload_game_t       unload_game;
	
	retro_library_get_region_t get_region;
	
	retro_library_get_memory_data_t   get_memory_data;
	retro_library_get_memory_size_t   get_memory_size;
};

void retro_library_construct (retro_library_t *library, const char *shared_object);
void retro_library_finalize  (retro_library_t *library);

retro_library_t *retro_library_new  (const char *shared_object);
void             retro_library_free (retro_library_t *library);

#endif

