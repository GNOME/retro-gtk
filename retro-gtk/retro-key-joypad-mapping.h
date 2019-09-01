// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include "retro-controller-codes.h"

G_BEGIN_DECLS

#define RETRO_TYPE_KEY_JOYPAD_MAPPING (retro_key_joypad_mapping_get_type())

G_DECLARE_FINAL_TYPE (RetroKeyJoypadMapping, retro_key_joypad_mapping, RETRO, KEY_JOYPAD_MAPPING, GObject)

RetroKeyJoypadMapping *retro_key_joypad_mapping_new ();
RetroKeyJoypadMapping *retro_key_joypad_mapping_new_default ();
void retro_key_joypad_mapping_set_button_key (RetroKeyJoypadMapping *self,
                                              RetroJoypadId          button,
                                              guint16                hardware_keycode);
guint16 retro_key_joypad_mapping_get_button_key (RetroKeyJoypadMapping *self,
                                                 RetroJoypadId          button);

G_END_DECLS
