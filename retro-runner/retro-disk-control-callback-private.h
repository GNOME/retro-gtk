// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-game-info-private.h"

G_BEGIN_DECLS

typedef struct _RetroDiskControlCallback RetroDiskControlCallback;

typedef gboolean (*RetroDiskControlCallbackSetEjectState) (gboolean ejected);
typedef gboolean (*RetroDiskControlCallbackGetEjectState) ();
typedef guint (*RetroDiskControlCallbackGetImageIndex) ();
typedef gboolean (*RetroDiskControlCallbackSetImageIndex) (guint index);
typedef guint (*RetroDiskControlCallbackGetNumImages) ();
typedef gboolean (*RetroDiskControlCallbackReplaceImageIndex) (guint index, RetroGameInfo *info);
typedef gboolean (*RetroDiskControlCallbackAddImageIndex) ();

struct _RetroDiskControlCallback
{
  RetroDiskControlCallbackSetEjectState set_eject_state;
  RetroDiskControlCallbackGetEjectState get_eject_state;
  RetroDiskControlCallbackGetImageIndex get_image_index;
  RetroDiskControlCallbackSetImageIndex set_image_index;
  RetroDiskControlCallbackGetNumImages get_num_images;
  RetroDiskControlCallbackReplaceImageIndex replace_image_index;
  RetroDiskControlCallbackAddImageIndex add_image_index;
};

G_END_DECLS
