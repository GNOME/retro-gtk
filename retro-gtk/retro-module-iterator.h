// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-core-descriptor.h"

G_BEGIN_DECLS

#define RETRO_TYPE_MODULE_ITERATOR (retro_module_iterator_get_type())

G_DECLARE_FINAL_TYPE (RetroModuleIterator, retro_module_iterator, RETRO, MODULE_ITERATOR, GObject)

RetroModuleIterator *retro_module_iterator_new (const gchar * const *lookup_paths,
                                                gboolean             recursive) G_GNUC_WARN_UNUSED_RESULT;
RetroCoreDescriptor *retro_module_iterator_get (RetroModuleIterator *self) G_GNUC_WARN_UNUSED_RESULT;
gboolean retro_module_iterator_next (RetroModuleIterator *self);

G_END_DECLS
