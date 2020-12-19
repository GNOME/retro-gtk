// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-option-iterator.h"

G_BEGIN_DECLS

RetroOptionIterator *retro_option_iterator_new (GHashTable *options) G_GNUC_WARN_UNUSED_RESULT;

G_END_DECLS
