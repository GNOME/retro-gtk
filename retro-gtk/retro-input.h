// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_INPUT_H
#define RETRO_INPUT_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-controller-codes.h"
#include "retro-controller-type.h"

G_BEGIN_DECLS

#define RETRO_TYPE_INPUT (retro_input_get_type())

GType retro_input_get_type (void) G_GNUC_CONST;

typedef union _RetroInput RetroInput;

RetroInput *retro_input_new (void);
RetroInput *retro_input_copy (RetroInput *self);
void retro_input_free (RetroInput *self);
RetroControllerType retro_input_get_controller_type (RetroInput *self);
gboolean retro_input_get_joypad (RetroInput    *self,
                                 RetroJoypadId *id);
gboolean retro_input_get_mouse (RetroInput   *self,
                                RetroMouseId *id);
gboolean retro_input_get_lightgun (RetroInput      *self,
                                   RetroLightgunId *id);
gboolean retro_input_get_analog (RetroInput       *self,
                                 RetroAnalogId    *id,
                                 RetroAnalogIndex *index);
gboolean retro_input_get_pointer (RetroInput     *self,
                                  RetroPointerId *id);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (RetroInput, retro_input_free)

G_END_DECLS

#endif /* RETRO_INPUT_H */
