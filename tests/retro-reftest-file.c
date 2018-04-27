/* retro-reftest-file.c
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

#include "retro-reftest-file.h"

#include <errno.h>
#include "retro-test-controller.h"

struct _RetroReftestFile
{
  GObject parent_instance;

  GFile *file;
  GKeyFile *key_file;
  gchar *path;
  GHashTable *frames;
};

G_DEFINE_TYPE (RetroReftestFile, retro_reftest_file, G_TYPE_OBJECT)

#define RETRO_REFTEST_FILE_ERROR (retro_reftest_file_error_quark ())

typedef enum
{
  RETRO_REFTEST_FILE_ERROR_NOT_A_UINT,
} ManetteMappingError;

GQuark retro_reftest_file_error_quark (void);

#define RETRO_REFTEST_FILE_RETRO_REFTEST_GROUP "Retro Reftest"
#define RETRO_REFTEST_FILE_PATH_KEY "Path"
#define RETRO_REFTEST_FILE_CORE_KEY "Core"
#define RETRO_REFTEST_FILE_CONTROLLERS_KEY "Controllers"
#define RETRO_REFTEST_FILE_MEDIAS_KEY "Medias"

#define RETRO_REFTEST_FILE_OPTIONS_GROUP "Options"

#define RETRO_REFTEST_FILE_FRAME_GROUP_PREFIX "Frame "
#define RETRO_REFTEST_FILE_FRAME_GROUP_PREFIX_LENGTH 6
#define RETRO_REFTEST_FILE_FRAME_CONTROLLER_PREFIX "Controller "
#define RETRO_REFTEST_FILE_FRAME_TESTS_KEY "Tests"
#define RETRO_REFTEST_FILE_FRAME_STATE_KEY "State"
#define RETRO_REFTEST_FILE_FRAME_VIDEO_KEY "Video"

enum {
  PROP_0,
  PROP_FILE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
g_pointer_free (gpointer *pointer)
{
  if (pointer != NULL)
    g_free (*pointer);
}

static void
g_object_pointer_unref (GObject **pointer)
{
  if (pointer != NULL)
    g_object_unref (*pointer);
}

static gint uint_compare (guint *a, guint *b) {
  if (*a == *b)
    return 0;

  return *a < *b ? -1 : 1;
}

RetroReftestFile *
retro_reftest_file_new (GFile *file)
{
  return g_object_new (RETRO_TYPE_REFTEST_FILE,
                       "file", file,
                       NULL);
}

static void
retro_reftest_file_constructed (GObject *object)
{
  RetroReftestFile *self = (RetroReftestFile *) object;
  gchar *path;
  GError *error = NULL;

  self->key_file = g_key_file_new ();

  path = g_file_get_path (self->file);
  g_key_file_load_from_file (self->key_file, path, G_KEY_FILE_NONE, &error);
  if (G_UNLIKELY (error != NULL)) {
    g_critical ("Couldn't load test file: %s", error->message);
    g_clear_error (&error);
  }

  g_free (path);

  G_OBJECT_CLASS (retro_reftest_file_parent_class)->constructed (object);
}

static void
retro_reftest_file_finalize (GObject *object)
{
  RetroReftestFile *self = (RetroReftestFile *)object;

  g_object_unref (self->file);
  g_key_file_unref (self->key_file);
  if (self->path != NULL)
    g_free (self->path);
  if (self->frames != NULL)
    g_hash_table_unref (self->frames);

  G_OBJECT_CLASS (retro_reftest_file_parent_class)->finalize (object);
}

static void
retro_reftest_file_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  RetroReftestFile *self = RETRO_REFTEST_FILE (object);

  switch (prop_id) {
  case PROP_FILE:
    g_value_set_object (value, self->file);

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
retro_reftest_file_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  RetroReftestFile *self = RETRO_REFTEST_FILE (object);

  switch (prop_id) {
  case PROP_FILE:
    g_set_object (&self->file, g_value_get_object (value));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
retro_reftest_file_class_init (RetroReftestFileClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = retro_reftest_file_constructed;
  object_class->finalize = retro_reftest_file_finalize;
  object_class->get_property = retro_reftest_file_get_property;
  object_class->set_property = retro_reftest_file_set_property;

  properties[PROP_FILE] =
    g_param_spec_object ("file",
                         "File",
                         "The file",
                         G_TYPE_FILE,
                         G_PARAM_CONSTRUCT_ONLY |
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB);

  g_object_class_install_properties (G_OBJECT_CLASS (klass), N_PROPS, properties);
}

static void
retro_reftest_file_init (RetroReftestFile *self)
{
}

static GFile *
retro_reftest_file_get_sibbling (RetroReftestFile *self,
                                 const gchar      *path)
{
  GFile *parent, *sibbling;

  if (path[0] == '/')
    return g_file_new_for_path (path);

  parent = g_file_get_parent (self->file);
  sibbling = g_file_get_child (parent, path);
  g_object_unref (parent);

  return sibbling;
}

const gchar *
retro_reftest_file_peek_path (RetroReftestFile *self)
{
  GError *error = NULL;

  if (self->path != NULL)
    return self->path;

  return g_key_file_get_string (self->key_file,
                                RETRO_REFTEST_FILE_RETRO_REFTEST_GROUP,
                                RETRO_REFTEST_FILE_PATH_KEY,
                                &error);
  g_assert_no_error (error);

  return self->path;
}

static guint
retro_reftest_file_str_to_uint (gchar   *string,
                                GError **error)
{
  gchar *string_end;
  guint64 number_long;

  if (string == NULL) {
    g_set_error (error,
                 RETRO_REFTEST_FILE_ERROR,
                 RETRO_REFTEST_FILE_ERROR_NOT_A_UINT,
                 "Unexpected NULL string.");

    return 0;
  }

  if (!g_ascii_isdigit (string[0])) {
    g_set_error (error,
                 RETRO_REFTEST_FILE_ERROR,
                 RETRO_REFTEST_FILE_ERROR_NOT_A_UINT,
                 "Not a number: %s.", string);

    return 0;
  }

  errno = 0;
  number_long = g_ascii_strtoull (string, &string_end, 10);
  if (errno != 0) {
    g_set_error (error,
                 RETRO_REFTEST_FILE_ERROR,
                 RETRO_REFTEST_FILE_ERROR_NOT_A_UINT,
                 "%s.", strerror (errno));

    return 0;
  }

  if (string_end[0] != '\0') {
    g_set_error (error,
                 RETRO_REFTEST_FILE_ERROR,
                 RETRO_REFTEST_FILE_ERROR_NOT_A_UINT,
                 "Not a number: %s.", string);

    return 0;
  }

  if (number_long > G_MAXUINT) {
    g_set_error (error,
                 RETRO_REFTEST_FILE_ERROR,
                 RETRO_REFTEST_FILE_ERROR_NOT_A_UINT,
                 "Not an unsigned interger: %s.", string);

    return 0;
  }

  return (guint) number_long;
}

static RetroControllerState *
retro_controller_state_from_string (gchar *string)
{
  RetroControllerState *state;
  RetroControllerType type;
  guint id, index;
  gint16 value;
  gchar *remaining;

  if (g_str_has_prefix (string, "Joypad ")) {
    remaining = string + strlen ("Joypad ");

    type = RETRO_CONTROLLER_TYPE_JOYPAD;
    index = 0;
    value = G_MAXINT16;

    if (g_str_equal (remaining, "B"))
      id = RETRO_JOYPAD_ID_B;
    else if (g_str_equal (remaining, "Y"))
      id = RETRO_JOYPAD_ID_Y;
    else if (g_str_equal (remaining, "SELECT"))
      id = RETRO_JOYPAD_ID_SELECT;
    else if (g_str_equal (remaining, "START"))
      id = RETRO_JOYPAD_ID_START;
    else if (g_str_equal (remaining, "UP"))
      id = RETRO_JOYPAD_ID_UP;
    else if (g_str_equal (remaining, "DOWN"))
      id = RETRO_JOYPAD_ID_DOWN;
    else if (g_str_equal (remaining, "LEFT"))
      id = RETRO_JOYPAD_ID_LEFT;
    else if (g_str_equal (remaining, "RIGHT"))
      id = RETRO_JOYPAD_ID_RIGHT;
    else if (g_str_equal (remaining, "A"))
      id = RETRO_JOYPAD_ID_A;
    else if (g_str_equal (remaining, "X"))
      id = RETRO_JOYPAD_ID_X;
    else if (g_str_equal (remaining, "L"))
      id = RETRO_JOYPAD_ID_L;
    else if (g_str_equal (remaining, "R"))
      id = RETRO_JOYPAD_ID_R;
    else if (g_str_equal (remaining, "L2"))
      id = RETRO_JOYPAD_ID_L2;
    else if (g_str_equal (remaining, "R2"))
      id = RETRO_JOYPAD_ID_R2;
    else if (g_str_equal (remaining, "L3"))
      id = RETRO_JOYPAD_ID_L3;
    else if (g_str_equal (remaining, "R3"))
      id = RETRO_JOYPAD_ID_R3;
    else
      return NULL;
  }
  else
    return NULL;

  state = g_new0 (RetroControllerState, 1);
  state->type = type;
  state->id = id;
  state->index = index;
  state->value = value;

  return state;
}

RetroCore *
retro_reftest_file_get_core (RetroReftestFile  *self,
                             GError           **error)
{
  RetroCore *core;
  gchar *key_file_core;
  GFile *core_file;
  gchar *path;
  gchar **key_file_medias;
  gsize key_file_medias_length = 0;
  gchar **media_uris;
  gint i;
  GFile *media_file;
  GError *tmp_error = NULL;

  key_file_core = g_key_file_get_string (self->key_file,
                                         RETRO_REFTEST_FILE_RETRO_REFTEST_GROUP,
                                         RETRO_REFTEST_FILE_CORE_KEY,
                                         &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return NULL;
  }

  core_file = retro_reftest_file_get_sibbling (self, key_file_core);
  g_free (key_file_core);
  path = g_file_get_path (core_file);
  g_object_unref (core_file);
  core = retro_core_new (path);
  g_free (path);

  key_file_medias = g_key_file_get_string_list (self->key_file,
                                                RETRO_REFTEST_FILE_RETRO_REFTEST_GROUP,
                                                RETRO_REFTEST_FILE_MEDIAS_KEY,
                                                &key_file_medias_length,
                                                &tmp_error);
  g_clear_error (&tmp_error);

  if (key_file_medias == NULL)
    return core;

  if (key_file_medias_length == 0) {
    g_strfreev (key_file_medias);

    return core;
  }

  media_uris = g_new0 (gchar *, key_file_medias_length + 1);
  for (i = 0; i < key_file_medias_length; i++) {
    media_file = retro_reftest_file_get_sibbling (self, key_file_medias[i]);
    media_uris[i] = g_file_get_uri (media_file);
    g_object_unref (media_file);
  }
  g_strfreev (key_file_medias);

  retro_core_set_medias (core, (const gchar* const *) media_uris);
  g_strfreev (media_uris);

  return core;
}

gboolean
retro_reftest_file_has_options (RetroReftestFile *self)
{
  return g_key_file_has_group (self->key_file, RETRO_REFTEST_FILE_OPTIONS_GROUP);
}

GHashTable *
retro_reftest_file_get_options (RetroReftestFile  *self,
                                GError           **error)
{
  gchar **keys, **values;
  gsize i, keys_length = 0;
  GHashTable *options;
  GError *tmp_error = NULL;

  keys = g_key_file_get_keys (self->key_file,
                              RETRO_REFTEST_FILE_OPTIONS_GROUP,
                              &keys_length,
                              &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return NULL;
  }

  options = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GDestroyNotify) g_strfreev);
  for (i = 0; i < keys_length; i++) {
    values = g_key_file_get_string_list (self->key_file,
                                         RETRO_REFTEST_FILE_OPTIONS_GROUP,
                                         keys[i],
                                         NULL,
                                         &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);
      g_strfreev (keys);
      g_hash_table_unref (options);

      return NULL;
    }

    g_hash_table_insert (options, g_strdup (keys[i]), values);
  }

  g_strfreev (keys);

  return options;
}

GArray *
retro_reftest_file_get_controllers (RetroReftestFile  *self,
                                    gsize             *length,
                                    GError           **error)
{
  gboolean has_controllers;
  gchar **controller_names;
  gsize i;
  GArray *controllers;
  RetroControllerType type;
  GError *tmp_error = NULL;

  has_controllers = g_key_file_has_key (self->key_file,
                                        RETRO_REFTEST_FILE_RETRO_REFTEST_GROUP,
                                        RETRO_REFTEST_FILE_CONTROLLERS_KEY,
                                        &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return NULL;
  }

  if (!has_controllers)
    return NULL;

  controller_names = g_key_file_get_string_list (self->key_file,
                                                 RETRO_REFTEST_FILE_RETRO_REFTEST_GROUP,
                                                 RETRO_REFTEST_FILE_CONTROLLERS_KEY,
                                                 length,
                                                 &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return NULL;
  }

  controllers = g_array_sized_new (TRUE, TRUE, sizeof (RetroTestController *), *length);
  g_array_set_clear_func (controllers, (GDestroyNotify) g_object_pointer_unref);
  for (i = 0; controller_names[i] != NULL; i++) {
    if (g_str_equal (controller_names[i], "Joypad"))
      type = RETRO_CONTROLLER_TYPE_JOYPAD;
    else
      continue;

    g_array_index (controllers, RetroTestController *, i) = retro_test_controller_new (type);
  }
  g_strfreev (controller_names);

  return controllers;
}

GList *
retro_reftest_file_get_frames (RetroReftestFile *self)
{
  gsize i, groups_length = 0;
  gchar **groups;
  gchar *frame_number_string;
  guint frame_number;
  guint *key;
  GError *error = NULL;

  if (self->frames != NULL) {
    return g_list_sort (g_hash_table_get_keys (self->frames),
                        (GCompareFunc) uint_compare);
  }

  self->frames = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);

  groups = g_key_file_get_groups (self->key_file, &groups_length);

  for (i = 0; i < groups_length; i++) {
    if (!g_str_has_prefix (groups[i], RETRO_REFTEST_FILE_FRAME_GROUP_PREFIX))
      continue;

    frame_number_string = groups[i] + strlen (RETRO_REFTEST_FILE_FRAME_GROUP_PREFIX);
    frame_number = retro_reftest_file_str_to_uint (frame_number_string, &error);
    if (G_UNLIKELY (error != NULL)) {
      g_critical ("Invalid frame group [%s]: %s", groups[i], error->message);
      g_clear_error (&error);

      continue;
    }

    if (g_hash_table_contains (self->frames, &frame_number)) {
      g_critical ("Can't use [%s], frame %u is already implemented by [%s].",
                  groups[i],
                  frame_number,
                  (gchar *) g_hash_table_lookup (self->frames, &frame_number));

      continue;
    }

    key = g_new (guint, 1);
    *key = frame_number;

    g_hash_table_insert (self->frames, key, g_strdup (groups[i]));
  }

  g_strfreev (groups);

  return g_list_sort (g_hash_table_get_keys (self->frames),
                      (GCompareFunc) uint_compare);
}

gboolean
retro_reftest_file_has_state (RetroReftestFile  *self,
                              guint              frame,
                              GError           **error)
{
  return g_key_file_has_key (self->key_file,
                             g_hash_table_lookup (self->frames, &frame),
                             RETRO_REFTEST_FILE_FRAME_STATE_KEY,
                             error);
}

gchar *
retro_reftest_file_get_state (RetroReftestFile  *self,
                              guint              frame,
                              GError           **error)
{
  return g_key_file_get_string (self->key_file,
                                g_hash_table_lookup (self->frames, &frame),
                                RETRO_REFTEST_FILE_FRAME_STATE_KEY,
                                error);
}

gboolean
retro_reftest_file_has_video (RetroReftestFile  *self,
                              guint              frame,
                              GError           **error)
{
  return g_key_file_has_key (self->key_file,
                             g_hash_table_lookup (self->frames, &frame),
                             RETRO_REFTEST_FILE_FRAME_VIDEO_KEY,
                             error);
}

GFile *
retro_reftest_file_get_video (RetroReftestFile  *self,
                              guint              frame,
                              GError           **error)
{
  gchar *key_file_video;
  GFile *video_file;
  GError *tmp_error = NULL;

  key_file_video = g_key_file_get_string (self->key_file,
                                          g_hash_table_lookup (self->frames, &frame),
                                          RETRO_REFTEST_FILE_FRAME_VIDEO_KEY,
                                          &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return NULL;
  }

  video_file = retro_reftest_file_get_sibbling (self, key_file_video);
  g_free (key_file_video);

  return video_file;
}

GHashTable *
retro_reftest_file_get_controller_states (RetroReftestFile  *self,
                                          guint              frame,
                                          GError           **error)
{
  GHashTable *controllers;
  gchar *group;
  gchar **keys, **key_i;
  gchar **inputs, **input_i;
  guint *controller_number;
  gchar *controller_number_string;
  RetroControllerState *state;
  GArray *states;
  GError *tmp_error = NULL;

  group = g_hash_table_lookup (self->frames, &frame);
  keys = g_key_file_get_keys (self->key_file, group, NULL, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return NULL;
  }

  controllers = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, (GDestroyNotify) g_array_unref);

  for (key_i = keys; *key_i != NULL; key_i++) {
    if (!g_str_has_prefix (*key_i, RETRO_REFTEST_FILE_FRAME_CONTROLLER_PREFIX))
      continue;

    controller_number_string = *key_i + strlen (RETRO_REFTEST_FILE_FRAME_CONTROLLER_PREFIX);
    controller_number = g_new (guint, 1);
    *controller_number = retro_reftest_file_str_to_uint (controller_number_string, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_critical ("Invalid controller key [%s]: %s", *key_i, tmp_error->message);
      g_clear_error (&tmp_error);
      g_free (controller_number);

      continue;
    }

    inputs = g_key_file_get_string_list (self->key_file, group, *key_i, NULL, &tmp_error);
    states = g_array_new (TRUE, TRUE, sizeof (RetroControllerState *));
    g_array_set_clear_func (states, (GDestroyNotify) g_pointer_free);
    for (input_i = inputs; *input_i != NULL; input_i++) {
      state = retro_controller_state_from_string (*input_i);
      if (state == NULL) {
        g_critical ("Invalid controller input: %s. Skipping.", *input_i);

        continue;
      }

      g_array_append_val (states, state);
    }
    g_strfreev (inputs);

    g_hash_table_insert (controllers, controller_number, states);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_critical ("%s", tmp_error->message);
      g_clear_error (&tmp_error);
      g_free (controller_number);

      continue;
    }
  }
  g_strfreev (keys);

  return controllers;
}

G_DEFINE_QUARK (retro-reftest-file-error, retro_reftest_file_error)
