/* retro-test-controller.c
 *
 * Copyright 2018 Adrien Plazas <kekun.plazas@laposte.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "retro-test-controller.h"

#define RETRO_ANALOG_ID_INDEX_COUNT 4
#define RETRO_ANALOG_ID_INDEX(id, index) ((index << 1) | id)

struct _RetroTestController
{
  GObject parent_instance;
  RetroControllerType controller_type;
  gint16 **state;
  guint16 rumble[RETRO_RUMBLE_EFFECT_COUNT];
};

static void retro_controller_interface_init (RetroControllerInterface *iface);

G_DEFINE_TYPE_WITH_CODE (RetroTestController, retro_test_controller, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (RETRO_TYPE_CONTROLLER,
                                                retro_controller_interface_init))

/* Private */

static void
retro_test_controller_poll (RetroController *base)
{
}

static gint16
retro_test_controller_get_input_state (RetroController *base,
                                       RetroInput      *input)
{
  RetroTestController *self = RETRO_TEST_CONTROLLER (base);

  g_return_val_if_fail (self != NULL, 0);

  switch (retro_input_get_controller_type (input) & RETRO_CONTROLLER_TYPE_TYPE_MASK) {
  case RETRO_CONTROLLER_TYPE_JOYPAD: {
    RetroJoypadId id;
    if (!retro_input_get_joypad (input, &id))
      return 0;

    return self->state[RETRO_CONTROLLER_TYPE_JOYPAD][id];
  }
  case RETRO_CONTROLLER_TYPE_MOUSE: {
    RetroMouseId id;
    if (!retro_input_get_mouse (input, &id))
      return 0;

    return self->state[RETRO_CONTROLLER_TYPE_MOUSE][id];
  }
  case RETRO_CONTROLLER_TYPE_LIGHTGUN: {
    RetroLightgunId id;
    if (!retro_input_get_lightgun (input, &id))
      return 0;

    return self->state[RETRO_CONTROLLER_TYPE_LIGHTGUN][id];
  }
  case RETRO_CONTROLLER_TYPE_ANALOG: {
    RetroAnalogId id;
    RetroAnalogIndex index;
    if (!retro_input_get_analog (input, &id, &index))
      return 0;

    return self->state[RETRO_CONTROLLER_TYPE_ANALOG][RETRO_ANALOG_ID_INDEX(id, index)];
  }
  case RETRO_CONTROLLER_TYPE_POINTER: {
    RetroPointerId id;
    if (!retro_input_get_pointer (input, &id))
      return 0;

    return self->state[RETRO_CONTROLLER_TYPE_POINTER][id];
  }
  case RETRO_CONTROLLER_TYPE_NONE:
  case RETRO_CONTROLLER_TYPE_KEYBOARD:
  case RETRO_CONTROLLER_TYPE_COUNT:
  default:
    return 0;
  }
}

static RetroControllerType
retro_test_controller_get_controller_type (RetroController *base)
{
  RetroTestController *self = RETRO_TEST_CONTROLLER (base);

  g_return_val_if_fail (self != NULL, 0);

  return self->controller_type;
}

static guint64
retro_test_controller_get_capabilities (RetroController *base)
{
  /* The test controller can mimic anything. */
  return G_MAXUINT64;
}

static gboolean
retro_test_controller_get_supports_rumble (RetroController *base)
{
  return FALSE;
}

static void
retro_test_controller_set_rumble_state (RetroController   *base,
                                        RetroRumbleEffect  effect,
                                        guint16            strength)
{
  RetroTestController *self = RETRO_TEST_CONTROLLER (base);

  self->rumble[effect] = strength;
}

static void
retro_test_controller_finalize (GObject *object)
{
  RetroTestController *self = RETRO_TEST_CONTROLLER (object);
  gsize i;

  for (i = 0; i < RETRO_CONTROLLER_TYPE_COUNT; i++)
    if (self->state[i] != NULL)
      g_free (self->state[i]);
  g_free (self->state);

  G_OBJECT_CLASS (retro_test_controller_parent_class)->finalize (object);
}

static void
retro_test_controller_class_init (RetroTestControllerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_test_controller_finalize;
}

static void
retro_test_controller_init (RetroTestController *self)
{
  self->state = g_new0 (gint16 *, RETRO_CONTROLLER_TYPE_COUNT);
  self->state[RETRO_CONTROLLER_TYPE_JOYPAD] = g_new0 (gint16, RETRO_JOYPAD_ID_COUNT);
  self->state[RETRO_CONTROLLER_TYPE_MOUSE] = g_new0 (gint16, RETRO_MOUSE_ID_COUNT);
  self->state[RETRO_CONTROLLER_TYPE_LIGHTGUN] = g_new0 (gint16, RETRO_LIGHTGUN_ID_COUNT);
  self->state[RETRO_CONTROLLER_TYPE_ANALOG] = g_new0 (gint16, RETRO_ANALOG_ID_INDEX_COUNT);
  self->state[RETRO_CONTROLLER_TYPE_POINTER] = g_new0 (gint16, RETRO_POINTER_ID_COUNT);
}

static void
retro_controller_interface_init (RetroControllerInterface *iface)
{
  iface->poll = retro_test_controller_poll;
  iface->get_input_state =  retro_test_controller_get_input_state;
  iface->get_controller_type = retro_test_controller_get_controller_type;
  iface->get_capabilities = retro_test_controller_get_capabilities;
  iface->get_supports_rumble = retro_test_controller_get_supports_rumble;
  iface->set_rumble_state = retro_test_controller_set_rumble_state;
}

/* Public */

RetroTestController *
retro_test_controller_new (RetroControllerType controller_type)
{
  RetroTestController *self = NULL;

  self = g_object_new (RETRO_TYPE_TEST_CONTROLLER, NULL);
  self->controller_type = controller_type;

  return self;
}

void
retro_test_controller_reset (RetroTestController *self)
{
  memset (self->state[RETRO_CONTROLLER_TYPE_JOYPAD], 0, RETRO_JOYPAD_ID_COUNT * sizeof (gint16));
  memset (self->state[RETRO_CONTROLLER_TYPE_MOUSE], 0, RETRO_MOUSE_ID_COUNT * sizeof (gint16));
  memset (self->state[RETRO_CONTROLLER_TYPE_LIGHTGUN], 0, RETRO_LIGHTGUN_ID_COUNT * sizeof (gint16));
  memset (self->state[RETRO_CONTROLLER_TYPE_ANALOG], 0, RETRO_ANALOG_ID_INDEX_COUNT * sizeof (gint16));
  memset (self->state[RETRO_CONTROLLER_TYPE_POINTER], 0, RETRO_POINTER_ID_COUNT * sizeof (gint16));
}

void
retro_test_controller_set_input_state (RetroTestController  *self,
                                       RetroControllerState *state)
{
  g_return_if_fail (self != NULL);

  switch (state->type & RETRO_CONTROLLER_TYPE_TYPE_MASK) {
  case RETRO_CONTROLLER_TYPE_JOYPAD:
    if (state->id < RETRO_JOYPAD_ID_COUNT)
      self->state[RETRO_CONTROLLER_TYPE_JOYPAD][state->id] = state->value;

    break;
  case RETRO_CONTROLLER_TYPE_MOUSE:
    if (state->id < RETRO_MOUSE_ID_COUNT)
      self->state[RETRO_CONTROLLER_TYPE_MOUSE][state->id] = state->value;

    break;
  case RETRO_CONTROLLER_TYPE_LIGHTGUN:
    if (state->id < RETRO_LIGHTGUN_ID_COUNT)
      self->state[RETRO_CONTROLLER_TYPE_LIGHTGUN][state->id] = state->value;

    break;
  case RETRO_CONTROLLER_TYPE_ANALOG:
    if (state->id < RETRO_ANALOG_ID_COUNT && state->index < RETRO_ANALOG_INDEX_COUNT)
      self->state[RETRO_CONTROLLER_TYPE_ANALOG][RETRO_ANALOG_ID_INDEX (state->id, state->index)] = state->value;

    break;
  case RETRO_CONTROLLER_TYPE_POINTER:
    if (state->id < RETRO_POINTER_ID_COUNT)
      self->state[RETRO_CONTROLLER_TYPE_POINTER][state->id] = state->value;

    break;
  case RETRO_CONTROLLER_TYPE_NONE:
  case RETRO_CONTROLLER_TYPE_KEYBOARD:
  case RETRO_CONTROLLER_TYPE_COUNT:
  default:
    break;
  }
}

guint16
retro_test_controller_get_rumble_state (RetroTestController *self,
                                        RetroRumbleEffect    effect)
{
  g_return_val_if_fail (self != NULL, 0);
  g_return_val_if_fail (effect >= RETRO_RUMBLE_EFFECT_COUNT, 0);

  return self->rumble[effect];
}
