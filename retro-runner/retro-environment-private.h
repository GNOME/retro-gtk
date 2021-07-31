// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-core.h"

G_BEGIN_DECLS

void retro_core_set_callbacks (RetroCore *self);
void retro_core_set_environment_interface (RetroCore *self);

G_END_DECLS
