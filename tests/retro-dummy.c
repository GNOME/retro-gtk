/* retro-dummy.c
 *
 * Copyright 2018 Adrien Plazas <kekun.plazas@laposte.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>
#include <libretro.h>
#include <stdlib.h>
#include <string.h>

#define FPS 60.0
#define SAMPLE_RATE 30000.0
#define WIDTH 320
#define HEIGHT 240
#define ASPECT_RATIO (((float) WIDTH) / ((float) HEIGHT))

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environment_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static uint16_t frame_buffer[WIDTH * HEIGHT];
static enum retro_pixel_format pixel_format = RETRO_PIXEL_FORMAT_RGB565;

void
retro_init (void)
{
  memset (frame_buffer, 0xff, WIDTH * HEIGHT * sizeof (uint16_t));
}

void
retro_deinit (void)
{
}

unsigned
retro_api_version (void)
{
  return RETRO_API_VERSION;
}

void
retro_set_controller_port_device (unsigned port,
                                  unsigned device)
{
}

void
retro_get_system_info (struct retro_system_info *info)
{
  memset (info, 0, sizeof (struct retro_system_info));
  info->library_name = "";
  info->library_version = "";
  info->need_fullpath = false;
  info->valid_extensions = "";
}

void
retro_get_system_av_info (struct retro_system_av_info *info)
{
  info->timing.fps = FPS;
  info->timing.sample_rate = SAMPLE_RATE;

  info->geometry.base_width = WIDTH;
  info->geometry.base_height = HEIGHT;
  info->geometry.max_width = WIDTH;
  info->geometry.max_height = HEIGHT;
  info->geometry.aspect_ratio = ASPECT_RATIO;
}

void
retro_set_environment (retro_environment_t cb)
{
  environment_cb = cb;

  environment_cb (RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixel_format);
}

void
retro_set_audio_sample (retro_audio_sample_t cb)
{
  audio_cb = cb;
}

void
retro_set_audio_sample_batch (retro_audio_sample_batch_t cb)
{
  audio_batch_cb = cb;
}

void
retro_set_input_poll (retro_input_poll_t cb)
{
  input_poll_cb = cb;
}

void
retro_set_input_state (retro_input_state_t cb)
{
  input_state_cb = cb;
}

void
retro_set_video_refresh (retro_video_refresh_t cb)
{
  video_cb = cb;
}

void
retro_reset (void)
{
}

void
retro_run (void)
{
  input_poll_cb ();
  video_cb (frame_buffer, WIDTH, HEIGHT, WIDTH * sizeof (uint16_t));
}

bool
retro_load_game (const struct retro_game_info *info)
{
  return false;
}

void
retro_unload_game (void)
{
}

unsigned
retro_get_region (void)
{
  return RETRO_REGION_NTSC;
}

bool
retro_load_game_special (unsigned                      type,
                         const struct retro_game_info *info,
                         size_t                        num)
{
  return false;
}

size_t
retro_serialize_size (void)
{
  return 0;
}

bool
retro_serialize (void   *data,
                 size_t  size)
{
  return false;
}

bool
retro_unserialize (const void *data,
                   size_t      size)
{
  return false;
}

void *
retro_get_memory_data (unsigned id)
{
  return NULL;
}

size_t
retro_get_memory_size (unsigned id)
{
  return 0;
}

void
retro_cheat_reset (void)
{
}

void
retro_cheat_set (unsigned    idx,
                 bool        enabled,
                 const char *code)
{
}
