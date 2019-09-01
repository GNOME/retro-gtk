// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-option.h"
#include "retro-variable.h"

G_BEGIN_DECLS

RetroOption *retro_option_new (const RetroVariable  *variable,
                               GError              **error);

G_END_DECLS
