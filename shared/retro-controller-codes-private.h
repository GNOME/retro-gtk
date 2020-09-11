// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-controller-codes.h"

G_BEGIN_DECLS

#define RETRO_ANALOG_ID_COUNT (RETRO_ANALOG_ID_Y + 1)
#define RETRO_ANALOG_INDEX_COUNT (RETRO_ANALOG_INDEX_RIGHT + 1)
#define RETRO_JOYPAD_ID_COUNT (RETRO_JOYPAD_ID_R3 + 1)
#define RETRO_LIGHTGUN_ID_COUNT (RETRO_LIGHTGUN_ID_START + 1)
#define RETRO_MOUSE_ID_COUNT (RETRO_MOUSE_ID_HORIZ_WHEELDOWN + 1)
#define RETRO_POINTER_ID_COUNT (RETRO_POINTER_ID_PRESSED + 1)

G_END_DECLS
