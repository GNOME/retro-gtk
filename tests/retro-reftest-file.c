#include "retro-reftest-file.h"

#include <errno.h>

struct _RetroReftestFile
{
  GObject parent_instance;

  GFile *file;
  GKeyFile *key_file;
  gchar *path;
  GHashTable *frames;
};

G_DEFINE_TYPE (RetroReftestFile, retro_reftest_file, G_TYPE_OBJECT)

#define RETRO_REFTEST_FILE_RETRO_REFTEST_GROUP "Retro Reftest"
#define RETRO_REFTEST_FILE_RETRO_PATH_KEY "Path"
#define RETRO_REFTEST_FILE_RETRO_CORE_KEY "Core"
#define RETRO_REFTEST_FILE_RETRO_MEDIAS_KEY "Medias"

#define RETRO_REFTEST_FILE_RETRO_FRAME_GROUP_PREFIX "Frame "
#define RETRO_REFTEST_FILE_RETRO_FRAME_GROUP_PREFIX_LENGTH 6
#define RETRO_REFTEST_FILE_RETRO_FRAME_TESTS_KEY "Tests"
#define RETRO_REFTEST_FILE_RETRO_FRAME_VIDEO_KEY "Video"

enum {
  PROP_0,
  PROP_FILE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

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
                                RETRO_REFTEST_FILE_RETRO_PATH_KEY,
                                &error);
  g_assert_no_error (error);

  return self->path;
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
                                         RETRO_REFTEST_FILE_RETRO_CORE_KEY,
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
                                                RETRO_REFTEST_FILE_RETRO_MEDIAS_KEY,
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

GList *
retro_reftest_file_get_frames (RetroReftestFile *self)
{
  gsize i, groups_length = 0;
  gchar **groups;
  gchar *frame_number_string, *frame_number_string_end;
  guint64 frame_number_long;
  guint frame_number;
  guint *key;

  if (self->frames != NULL) {
    return g_list_sort (g_hash_table_get_keys (self->frames),
                        (GCompareFunc) uint_compare);
  }

  self->frames = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);

  groups = g_key_file_get_groups (self->key_file, &groups_length);

  for (i = 0; i < groups_length; i++) {
    if (!g_str_has_prefix (groups[i], RETRO_REFTEST_FILE_RETRO_FRAME_GROUP_PREFIX))
      continue;

    frame_number_string = groups[i] + RETRO_REFTEST_FILE_RETRO_FRAME_GROUP_PREFIX_LENGTH;
    if (!g_ascii_isdigit (frame_number_string[0])) {
      g_critical ("Invalid frame group [%s]: %s isn't a valid frame number.", groups[i], frame_number_string);

      continue;
    }

    errno = 0;
    frame_number_long = g_ascii_strtoull (frame_number_string,
                                          &frame_number_string_end,
                                          10);
    if (errno != 0) {
      g_critical ("Invalid frame group [%s]: %s", groups[i], strerror (errno));

      continue;
    }

    if (frame_number_string_end[0] != '\0') {
      g_critical ("Invalid frame group [%s]: %s isn't a valid frame number.", groups[i], frame_number_string);

      continue;
    }

    if (frame_number_long >= G_MAXUINT) {
      g_critical ("Invalid frame number %lu, the maximum allowed frame number is %u.", frame_number_long, G_MAXUINT - 1);

      continue;
    }

    frame_number = (guint) frame_number_long;

    if (g_hash_table_contains (self->frames, &frame_number)) {
      g_critical ("Can't use [%s], frame %u is already implemented by [%s].",
                  groups[i],
                  frame_number,
                  g_hash_table_lookup (self->frames, &frame_number));

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

gchar **
retro_reftest_file_get_tests (RetroReftestFile  *self,
                              guint              frame,
                              gsize             *length,
                              GError           **error)
{
  return g_key_file_get_string_list (self->key_file,
                                     g_hash_table_lookup (self->frames, &frame),
                                     RETRO_REFTEST_FILE_RETRO_FRAME_TESTS_KEY,
                                     length,
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
                                          RETRO_REFTEST_FILE_RETRO_FRAME_VIDEO_KEY,
                                          error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, tmp_error);

    return NULL;
  }

  video_file = retro_reftest_file_get_sibbling (self, key_file_video);
  g_free (key_file_video);

  return video_file;
}
