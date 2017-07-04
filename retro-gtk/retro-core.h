// This file is part of Retro. License: GPLv3

#ifndef RETRO_CORE_H
#define RETRO_CORE_H

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct {
  void (*callback) (guchar down, guint keycode, guint32 character, guint16 key_modifiers);
} RetroKeyboardCallback;

#define RETRO_CORE_ENVIRONMENT_INTERNAL(core) ((RetroCoreEnvironmentInternal *) ((core)->environment_internal))

typedef struct _RetroCoreEnvironmentInternal RetroCoreEnvironmentInternal;

struct _RetroCoreEnvironmentInternal {
  RetroKeyboardCallback keyboard_callback;
};

G_END_DECLS

#endif /* RETRO_CORE_H */
