// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_CONTROLLER_ITERATOR_PRIVATE_H
#define RETRO_CONTROLLER_ITERATOR_PRIVATE_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-controller-iterator.h"

G_BEGIN_DECLS

RetroControllerIterator *retro_controller_iterator_new (GHashTable *controllers);

G_END_DECLS

#endif /* RETRO_CONTROLLER_ITERATOR_PRIVATE_H */

