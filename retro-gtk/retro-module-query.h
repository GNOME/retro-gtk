// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_MODULE_QUERY_H
#define RETRO_MODULE_QUERY_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

// FIXME Remove as soon as possible.
typedef struct _RetroModuleIterator RetroModuleIterator;

#define RETRO_TYPE_MODULE_QUERY (retro_module_query_get_type())

G_DECLARE_FINAL_TYPE (RetroModuleQuery, retro_module_query, RETRO, MODULE_QUERY, GObject)

RetroModuleQuery *retro_module_query_new (gboolean recursive);
RetroModuleIterator *retro_module_query_iterator (RetroModuleQuery *self);

G_END_DECLS

#endif /* RETRO_MODULE_QUERY_H */

