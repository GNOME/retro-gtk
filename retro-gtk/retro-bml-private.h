// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gio/gio.h>

G_BEGIN_DECLS

#define RETRO_BML_ERROR (retro_bml_error_quark ())

typedef enum
{
  RETRO_BML_ERROR_NOT_NAME,
  RETRO_BML_ERROR_NOT_QUOTED_VALUE,
  RETRO_BML_ERROR_NOT_VALUE,
} RetroBmlError;

GQuark retro_bml_error_quark (void);

#define RETRO_TYPE_BML (retro_bml_get_type())

G_DECLARE_FINAL_TYPE (RetroBml, retro_bml, RETRO, BML, GObject)

RetroBml *retro_bml_new (void);
void retro_bml_parse_file (RetroBml  *self,
                           GFile     *file,
                           GError   **error);
GNode *retro_bml_get_root (RetroBml  *self);
gchar *retro_bml_node_get_name (GNode *node);
gchar *retro_bml_node_get_value (GNode *node);
GHashTable *retro_bml_node_get_attributes (GNode *node);

G_END_DECLS
