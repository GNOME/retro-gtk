// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

// FIXME Remove as soon as possible.
typedef struct _RetroCore RetroCore;

#define RETRO_TYPE_PA_PLAYER (retro_pa_player_get_type())

G_DECLARE_FINAL_TYPE (RetroPaPlayer, retro_pa_player, RETRO, PA_PLAYER, GObject)

RetroPaPlayer *retro_pa_player_new (void) G_GNUC_WARN_UNUSED_RESULT;
void retro_pa_player_set_core (RetroPaPlayer *self,
                               RetroCore     *core);

G_END_DECLS
