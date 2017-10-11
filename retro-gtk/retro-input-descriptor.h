// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_INPUT_DESCRIPTOR_H
#define RETRO_INPUT_DESCRIPTOR_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-controller-type.h"

G_BEGIN_DECLS

#define RETRO_TYPE_INPUT_DESCRIPTOR (retro_input_descriptor_get_type())

typedef struct _RetroInputDescriptor RetroInputDescriptor;

struct _RetroInputDescriptor
{
  guint port;
  RetroControllerType controller_type;
  guint index;
  guint id;
  gchar *description;
};

RetroInputDescriptor     *retro_input_descriptor_new   (void);
RetroInputDescriptor     *retro_input_descriptor_copy  (RetroInputDescriptor *self);
void                      retro_input_descriptor_free  (RetroInputDescriptor *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (RetroInputDescriptor, retro_input_descriptor_free)

G_END_DECLS

#endif /* RETRO_INPUT_DESCRIPTOR_H */
