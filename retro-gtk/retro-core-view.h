// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_CORE_VIEW_H
#define RETRO_CORE_VIEW_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include "retro-core.h"
#include "retro-controller.h"
#include "retro-input.h"
#include "retro-key-joypad-mapping.h"
#include "retro-video-filter.h"

G_BEGIN_DECLS

#define RETRO_TYPE_CORE_VIEW (retro_core_view_get_type())

G_DECLARE_FINAL_TYPE (RetroCoreView, retro_core_view, RETRO, CORE_VIEW, GtkEventBox)

RetroCoreView *retro_core_view_new (void);

void retro_core_view_set_core (RetroCoreView *self,
                               RetroCore     *core);
void retro_core_view_set_pixbuf (RetroCoreView *self,
                                 GdkPixbuf     *pixbuf);
GdkPixbuf *retro_core_view_get_pixbuf (RetroCoreView *self);
void retro_core_view_set_filter (RetroCoreView    *self,
                                 RetroVideoFilter  filter);
void retro_core_view_set_key_joypad_mapping (RetroCoreView         *self,
                                             RetroKeyJoypadMapping *mapping);
RetroKeyJoypadMapping *retro_core_view_get_key_joypad_mapping (RetroCoreView *self);
RetroController *retro_core_view_as_controller (RetroCoreView       *self,
                                                RetroControllerType  controller_type);
void retro_core_view_set_as_default_controller (RetroCoreView *self,
                                                RetroCore     *core);
gint16 retro_core_view_get_input_state (RetroCoreView *self,
                                        RetroInput    *input);
guint64 retro_core_view_get_controller_capabilities (RetroCoreView *self);
gboolean retro_core_view_get_can_grab_pointer (RetroCoreView *self);
void retro_core_view_set_can_grab_pointer (RetroCoreView *self,
                                           gboolean       can_grab_pointer);
gboolean retro_core_view_get_snap_pointer_to_borders (RetroCoreView *self);
void retro_core_view_set_snap_pointer_to_borders (RetroCoreView *self,
                                                  gboolean       snap_pointer_to_borders);

G_END_DECLS

#endif /* RETRO_CORE_VIEW_H */
