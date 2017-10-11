// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_CONTROLLER_ITERATOR_H
#define RETRO_CONTROLLER_ITERATOR_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-controller.h"

G_BEGIN_DECLS

#define RETRO_TYPE_CONTROLLER_ITERATOR (retro_controller_iterator_get_type())

G_DECLARE_FINAL_TYPE (RetroControllerIterator, retro_controller_iterator, RETRO, CONTROLLER_ITERATOR, GObject)

gboolean retro_controller_iterator_next (RetroControllerIterator  *self,
                                         guint                   **port,
                                         RetroController         **controller);

G_END_DECLS

#endif /* RETRO_CONTROLLER_ITERATOR_H */

