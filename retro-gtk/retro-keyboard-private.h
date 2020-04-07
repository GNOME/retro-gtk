// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gdk/gdk.h>
#include "retro-keyboard-key-private.h"

G_BEGIN_DECLS

RetroKeyboardModifierKey retro_keyboard_modifier_key_converter (guint keyval,
                                                                GdkModifierType modifiers);
RetroKeyboardKey retro_keyboard_key_converter (guint16 keycode);
guint retro_keyboard_key_to_val_converter (RetroKeyboardKey key);

G_END_DECLS
