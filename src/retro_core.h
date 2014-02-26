/* retro_core.h
 * Isolated libretro implementation to avoid libretro's singleton
 * pattern.
 * 
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


#ifndef __RETRO_CORE_H__
#define __RETRO_CORE_H__

#include "retro_library.h"

typedef struct retro_core retro_core_t;

typedef bool    (*retro_core_environment_cb_t)        (unsigned cmd, void *data, void *user_data);
typedef void    (*retro_core_video_refresh_cb_t)      (const void *data, unsigned width, unsigned height, size_t pitch, void *user_data);
typedef void    (*retro_core_audio_sample_cb_t)       (int16_t left, int16_t right, void *user_data);
typedef size_t  (*retro_core_audio_sample_batch_cb_t) (const int16_t *data, size_t frames, void *user_data);
typedef void    (*retro_core_input_poll_cb_t)         (void *user_data);
typedef int16_t (*retro_core_input_state_cb_t)        (unsigned port, unsigned device, unsigned index, unsigned id, void *user_data);

struct retro_core {
	retro_library_t *library;
	
	retro_core_environment_cb_t        environment_cb;
	retro_core_video_refresh_cb_t      video_refresh_cb;
	retro_core_audio_sample_cb_t       audio_sample_cb;
	retro_core_audio_sample_batch_cb_t audio_sample_batch_cb;
	retro_core_input_poll_cb_t         input_poll_cb;
	retro_core_input_state_cb_t        input_state_cb;
	
	void *environment_data;
	void *video_refresh_data;
	void *audio_sample_data;
	void *audio_sample_batch_data;
	void *input_poll_data;
	void *input_state_data;
};

void retro_core_construct (retro_core_t *this, char *library_path);
void retro_core_finalize  (retro_core_t *this);

retro_core_t *retro_core_new  (char *library_path);
void retro_core_free (retro_core_t *this);

void retro_core_set_environment        (retro_core_t *this, retro_core_environment_cb_t, void *user_data);
void retro_core_set_video_refresh      (retro_core_t *this, retro_core_video_refresh_cb_t, void *user_data);
void retro_core_set_audio_sample       (retro_core_t *this, retro_core_audio_sample_cb_t, void *user_data);
void retro_core_set_audio_sample_batch (retro_core_t *this, retro_core_audio_sample_batch_cb_t, void *user_data);
void retro_core_set_input_poll         (retro_core_t *this, retro_core_input_poll_cb_t, void *user_data);
void retro_core_set_input_state        (retro_core_t *this, retro_core_input_state_cb_t, void *user_data);

void retro_core_init   (retro_core_t *this);
void retro_core_deinit (retro_core_t *this);

unsigned retro_core_api_version (retro_core_t *this);

void retro_core_get_system_info    (retro_core_t *this, struct retro_system_info *info);
void retro_core_get_system_av_info (retro_core_t *this, struct retro_system_av_info *info);

void retro_core_set_controller_port_device (retro_core_t *this, unsigned port, unsigned device);

void retro_core_reset (retro_core_t *this);
void retro_core_run   (retro_core_t *this);

size_t retro_core_serialize_size (retro_core_t *this);
bool   retro_core_serialize      (retro_core_t *this, void *data, size_t size);
bool   retro_core_unserialize    (retro_core_t *this, const void *data, size_t size);

void retro_core_cheat_reset (retro_core_t *this);
void retro_core_cheat_set   (retro_core_t *this, unsigned index, bool enabled, const char *code);

bool retro_core_load_game         (retro_core_t *this, const struct retro_game_info *game);
bool retro_core_load_game_special (retro_core_t *this, unsigned game_type, const struct retro_game_info *info, size_t num_info);
void retro_core_unload_game       (retro_core_t *this);

unsigned retro_core_get_region (retro_core_t *this);

void  *retro_core_get_memory_data (retro_core_t *this, unsigned id);
size_t retro_core_get_memory_size (retro_core_t *this, unsigned id);

#endif

