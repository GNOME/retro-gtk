// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef __RETRO_GTK_H__
#define __RETRO_GTK_H__

#define __RETRO_GTK_INSIDE__

#ifndef RETRO_GTK_USE_UNSTABLE_API
#error    retro-gtk is unstable API. You must define RETRO_GTK_USE_UNSTABLE_API before including retro-gtk.h
#endif

#include "retro-controller.h"
#include "retro-controller-codes.h"
#include "retro-controller-type.h"
#include "retro-core.h"
#include "retro-core-descriptor.h"
#include "retro-core-view.h"
#include "retro-input.h"
#include "retro-log.h"
#include "retro-main-loop.h"
#include "retro-memory-type.h"
#include "retro-module-iterator.h"
#include "retro-module-query.h"
#include "retro-pixdata.h"
#include "retro-video-filter.h"

#undef __RETRO_GTK_INSIDE__

#endif
