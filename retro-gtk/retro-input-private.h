// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_INPUT_PRIVATE_H
#define RETRO_INPUT_PRIVATE_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include "retro-input.h"
#include "retro-keyboard-key.h"

G_BEGIN_DECLS

typedef struct _RetroInputAny RetroInputAny;
typedef struct _RetroInputJoypad RetroInputJoypad;
typedef struct _RetroInputMouse RetroInputMouse;
typedef struct _RetroInputKeyboard RetroInputKeyboard;
typedef struct _RetroInputLightgun RetroInputLightgun;
typedef struct _RetroInputAnalog RetroInputAnalog;
typedef struct _RetroInputPointer RetroInputPointer;

struct _RetroInputAny {
  RetroControllerType type;
  gint id;
  gint index;
};

struct _RetroInputJoypad {
  RetroControllerType type;
  RetroJoypadId id;
};

struct _RetroInputMouse {
  RetroControllerType type;
  RetroMouseId id;
};

struct _RetroInputKeyboard {
  RetroControllerType type;
  RetroKeyboardKey key;
};

struct _RetroInputLightgun {
  RetroControllerType type;
  RetroLightgunId id;
};

struct _RetroInputAnalog {
  RetroControllerType type;
  RetroAnalogId id;
  RetroAnalogIndex index;
};

struct _RetroInputPointer {
  RetroControllerType type;
  RetroPointerId id;
};

union _RetroInput {
  RetroInputAny any;
  RetroInputJoypad joypad;
  RetroInputMouse mouse;
  RetroInputKeyboard keyboard;
  RetroInputLightgun lightgun;
  RetroInputAnalog analog;
  RetroInputPointer pointer;
};

void retro_input_init (RetroInput          *self,
                       RetroControllerType  controller_type,
                       guint                id,
                       guint                index);

gboolean retro_input_get_keyboard (RetroInput       *self,
                                   RetroKeyboardKey *key);

G_END_DECLS

#endif /* RETRO_INPUT_PRIVATE_H */
