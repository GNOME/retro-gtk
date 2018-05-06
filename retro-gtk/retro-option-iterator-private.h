// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_OPTION_ITERATOR_PRIVATE_H
#define RETRO_OPTION_ITERATOR_PRIVATE_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-option-iterator.h"

G_BEGIN_DECLS

RetroOptionIterator *retro_option_iterator_new (GHashTable *options);

G_END_DECLS

#endif /* RETRO_OPTION_ITERATOR_PRIVATE_H */
