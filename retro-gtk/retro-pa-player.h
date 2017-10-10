// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_PA_PLAYER_H
#define RETRO_PA_PLAYER_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

// FIXME Remove as soon as possible.
typedef struct _RetroCore RetroCore;

#define RETRO_TYPE_PA_PLAYER (retro_pa_player_get_type())

G_DECLARE_FINAL_TYPE (RetroPaPlayer, retro_pa_player, RETRO, PA_PLAYER, GObject)

RetroPaPlayer *retro_pa_player_new (void);
void retro_pa_player_set_core (RetroPaPlayer *self,
                               RetroCore     *core);

G_END_DECLS

#endif /* RETRO_PA_PLAYER_H */
