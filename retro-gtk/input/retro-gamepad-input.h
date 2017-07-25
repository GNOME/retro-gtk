// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_GAMEPAD_BUTTON_H
#define RETRO_GAMEPAD_BUTTON_H

#include <linux/input-event-codes.h>
#include "../retro-gtk-internal.h"

G_BEGIN_DECLS

RetroJoypadId retro_gamepad_button_converter (guint16 button);

G_END_DECLS

#endif /* RETRO_GAMEPAD_BUTTON_H */
