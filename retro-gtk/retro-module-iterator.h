// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_MODULE_ITERATOR_H
#define RETRO_MODULE_ITERATOR_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-core-descriptor.h"

G_BEGIN_DECLS

#define RETRO_TYPE_MODULE_ITERATOR (retro_module_iterator_get_type())

G_DECLARE_FINAL_TYPE (RetroModuleIterator, retro_module_iterator, RETRO, MODULE_ITERATOR, GObject)

RetroModuleIterator *retro_module_iterator_new (const gchar * const *lookup_paths,
                                                gboolean             recursive);
RetroCoreDescriptor *retro_module_iterator_get (RetroModuleIterator *self);
gboolean retro_module_iterator_next (RetroModuleIterator *self);

G_END_DECLS

#endif /* RETRO_MODULE_ITERATOR_H */

