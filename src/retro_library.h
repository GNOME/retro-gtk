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

typedef struct {
	void *handle;
	char *src_name;
	char *tmp_name;
} retro_library_t;

retro_library_t *retro_library_new (char *shared_object);
void retro_library_free (retro_library_t *library);

void retro_library_set_environment        (retro_library_t *library, retro_environment_t);
void retro_library_set_video_refresh      (retro_library_t *library, retro_video_refresh_t);
void retro_library_set_audio_sample       (retro_library_t *library, retro_audio_sample_t);
void retro_library_set_audio_sample_batch (retro_library_t *library, retro_audio_sample_batch_t);
void retro_library_set_input_poll         (retro_library_t *library, retro_input_poll_t);
void retro_library_set_input_state        (retro_library_t *library, retro_input_state_t);

void retro_library_init   (retro_library_t *library);
void retro_library_deinit (retro_library_t *library);

unsigned retro_library_api_version(retro_library_t *library);
void retro_library_get_system_info    (retro_library_t *library, struct retro_system_info *info);
void retro_library_get_system_av_info (retro_library_t *library, struct retro_system_av_info *info);

void retro_library_set_controller_port_device (retro_library_t *library, unsigned port, unsigned device);

void retro_library_reset (retro_library_t *library);
void retro_library_run   (retro_library_t *library);

size_t retro_library_serialize_size (retro_library_t *library);
bool   retro_library_serialize      (retro_library_t *library, void *data, size_t size);
bool   retro_library_unserialize    (retro_library_t *library, const void *data, size_t size);

void retro_library_cheat_reset (retro_library_t *library);
void retro_library_cheat_set   (retro_library_t *library, unsigned index, bool enabled, const char *code);

bool retro_library_load_game         (retro_library_t *library, const struct retro_game_info *game);
bool retro_library_load_game_special (retro_library_t *library, unsigned game_type, const struct retro_game_info *info, size_t num_info);
void retro_library_unload_game       (retro_library_t *library);

unsigned retro_library_get_region (retro_library_t *library);

void  *retro_library_get_memory_data (retro_library_t *library, unsigned id);
size_t retro_library_get_memory_size (retro_library_t *library, unsigned id);

#endif

