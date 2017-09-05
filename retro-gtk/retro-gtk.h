// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef __RETRO_GTK_H__
#define __RETRO_GTK_H__

#define __RETRO_GTK_INSIDE__

#ifndef RETRO_GTK_USE_UNSTABLE_API
#error    retro-gtk is unstable API. You must define RETRO_GTK_USE_UNSTABLE_API before including retro-gtk.h
#endif

#include "retro-analog-id.h"
#include "retro-analog-index.h"
#include "retro-device-type.h"
#include "retro-gtk-vala.h"
#include "retro-joypad-id.h"
#include "retro-lightgun-id.h"
#include "retro-mouse-id.h"
#include "retro-pointer-id.h"

#undef __RETRO_GTK_INSIDE__

#endif
