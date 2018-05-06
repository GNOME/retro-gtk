// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_OPTION_ITERATOR_H
#define RETRO_OPTION_ITERATOR_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-option.h"

G_BEGIN_DECLS

#define RETRO_TYPE_OPTION_ITERATOR (retro_option_iterator_get_type())

G_DECLARE_FINAL_TYPE (RetroOptionIterator, retro_option_iterator, RETRO, OPTION_ITERATOR, GObject)

gboolean retro_option_iterator_next (RetroOptionIterator  *self,
                                     guint               **key,
                                     RetroOption         **option);

G_END_DECLS

#endif /* RETRO_OPTION_ITERATOR_H */
