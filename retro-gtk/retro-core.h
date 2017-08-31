// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_CORE_H
#define RETRO_CORE_H

#include "retro-disk-control-callback.h"
#include "retro-gtk-internal.h"
#include "retro-module.h"
#include "retro-rotation.h"

G_BEGIN_DECLS

typedef struct {
  void (*callback) (guchar down, guint keycode, guint32 character, guint16 key_modifiers);
} RetroKeyboardCallback;

#define RETRO_CORE_ENVIRONMENT_INTERNAL(core) ((RetroCoreEnvironmentInternal *) ((core)->environment_internal))

typedef struct _RetroCoreEnvironmentInternal RetroCoreEnvironmentInternal;

struct _RetroCoreEnvironmentInternal {
  RetroModule *module;
  RetroDiskControlCallback *disk_control_callback;
  gchar **media_uris;
  RetroSystemInfo *system_info;
  gfloat aspect_ratio;
  gboolean overscan;
  RetroPixelFormat pixel_format;
  RetroRotation rotation;
  gdouble sample_rate;

  RetroKeyboardCallback keyboard_callback;
};

gchar *retro_core_get_name (RetroCore *self);
void retro_core_set_system_av_info (RetroCore         *self,
                                    RetroSystemAvInfo *system_av_info);
void retro_core_set_controller_port_device (RetroCore       *self,
                                            guint            port,
                                            RetroDeviceType  device);

G_END_DECLS

#endif /* RETRO_CORE_H */
