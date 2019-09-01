// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-core.h"

G_BEGIN_DECLS

#define RETRO_TYPE_MAIN_LOOP (retro_main_loop_get_type())

G_DECLARE_FINAL_TYPE (RetroMainLoop, retro_main_loop, RETRO, MAIN_LOOP, GObject)

RetroMainLoop *retro_main_loop_new (RetroCore *core);
RetroCore *retro_main_loop_get_core (RetroMainLoop *self);
void retro_main_loop_set_core (RetroMainLoop *self,
                               RetroCore     *core);
void retro_main_loop_start (RetroMainLoop *self);
void retro_main_loop_reset (RetroMainLoop *self);
void retro_main_loop_stop (RetroMainLoop *self);
gdouble retro_main_loop_get_speed_rate (RetroMainLoop *self);
void retro_main_loop_set_speed_rate (RetroMainLoop *self,
                                     gdouble        speed_rate);

G_END_DECLS
