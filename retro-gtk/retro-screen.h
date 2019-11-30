// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_SCREEN_H
#define RETRO_SCREEN_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gdk/gdk.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_SCREEN (retro_screen_get_type())

G_DECLARE_FINAL_TYPE (RetroScreen, retro_screen, RETRO, SCREEN, GObject)

RetroScreen *retro_screen_new (void);

const GdkRectangle *retro_screen_get_view (RetroScreen *self);
void                retro_screen_set_view (RetroScreen        *self,
                                           const GdkRectangle *view);

G_END_DECLS

#endif /* RETRO_SCREEN_H */
