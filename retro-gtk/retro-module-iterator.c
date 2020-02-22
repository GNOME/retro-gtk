// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-module-iterator.h"

struct _RetroModuleIterator
{
  GObject parent_instance;
  gchar **directories;
  gboolean recursive;
  gint current_directory;
  GFileEnumerator *file_enumerator;
  RetroCoreDescriptor *core_descriptor;
  RetroModuleIterator *sub_directory;
  GHashTable *visited;
};

G_DEFINE_TYPE (RetroModuleIterator, retro_module_iterator, G_TYPE_OBJECT)

/* Private */

static void
retro_module_iterator_finalize (GObject *object)
{
  RetroModuleIterator *self = RETRO_MODULE_ITERATOR (object);

  g_strfreev (self->directories);
  g_clear_object (&self->file_enumerator);
  g_clear_object (&self->core_descriptor);
  g_clear_object (&self->sub_directory);
  g_hash_table_unref (self->visited);

  G_OBJECT_CLASS (retro_module_iterator_parent_class)->finalize (object);
}

static void
retro_module_iterator_class_init (RetroModuleIteratorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_module_iterator_finalize;
}

static void
retro_module_iterator_init (RetroModuleIterator *self)
{
}

static RetroModuleIterator *
retro_module_iterator_new_for_subdirectory (const gchar *lookup_path,
                                            GHashTable  *visited_paths)
{
  RetroModuleIterator *self;

  g_assert (lookup_path != NULL);
  g_assert (visited_paths != NULL);

  self = g_object_new (RETRO_TYPE_MODULE_ITERATOR, NULL);
  self->directories = g_new0 (gchar *, 2);
  self->directories[0] = g_strdup (lookup_path);
  self->recursive = TRUE;
  self->visited = g_hash_table_ref (visited_paths);

  return self;
}

static gboolean
retro_module_iterator_was_current_directory_visited (RetroModuleIterator *self)
{
  GFile *current_directory_file;
  gchar *current_directory_path;
  gboolean result;

  current_directory_file = g_file_new_for_path (self->directories[self->current_directory]);
  current_directory_path = g_file_get_path (current_directory_file);
  g_object_unref (current_directory_file);
  result = g_hash_table_contains (self->visited, current_directory_path);
  g_free (current_directory_path);

  return result;
}

static void
retro_module_iterator_set_current_directory_as_visited (RetroModuleIterator *self)
{
  GFile *current_directory_file;
  gchar *current_directory_path;

  current_directory_file = g_file_new_for_path (self->directories[self->current_directory]);
  current_directory_path = g_file_get_path (current_directory_file);
  g_object_unref (current_directory_file);
  g_hash_table_add (self->visited, current_directory_path);
}

static gboolean
retro_module_iterator_next_in_sub_directory (RetroModuleIterator *self)
{
  if (retro_module_iterator_next (self->sub_directory)) {
    if (self->core_descriptor != NULL)
      g_object_unref (self->core_descriptor);

    self->core_descriptor = retro_module_iterator_get (self->sub_directory);

    return TRUE;
  }

  if (self->sub_directory != NULL) {
    g_object_unref (self->sub_directory);
    self->sub_directory = NULL;
  }

  return FALSE;
}

static gboolean
retro_module_iterator_iterate_next_in_current_path (RetroModuleIterator  *self,
                                                    GFile                *directory,
                                                    GFileInfo            *info,
                                                    GError              **error)
{
  const gchar *sub_directory_basename;
  GFile *sub_directory_file;
  gchar *sub_directory_path;
  const gchar *core_descriptor_basename;
  GFile *core_descriptor_file;
  gchar *core_descriptor_path;
  RetroCoreDescriptor *core_descriptor;
  GError *tmp_error = NULL;

  g_assert (G_IS_FILE (directory));
  g_assert (G_IS_FILE_INFO (info));

  if (self->recursive &&
      g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY &&
      self->sub_directory == NULL) {
    sub_directory_basename = g_file_info_get_name (info);
    sub_directory_file = g_file_get_child (directory, sub_directory_basename);
    sub_directory_path = g_file_get_path (sub_directory_file);
      g_object_unref (sub_directory_file);

    if (g_hash_table_contains (self->visited, sub_directory_path)) {
      g_free (sub_directory_path);

      return FALSE;
    }

    self->sub_directory = retro_module_iterator_new_for_subdirectory (sub_directory_path, self->visited);
    g_free (sub_directory_path);

    return retro_module_iterator_next_in_sub_directory (self);
  }

  core_descriptor_basename = g_file_info_get_name (info);
  if (!g_str_has_suffix (core_descriptor_basename, ".libretro"))
    return FALSE;

  core_descriptor_file = g_file_get_child (directory, core_descriptor_basename);
  core_descriptor_path = g_file_get_path (core_descriptor_file);
  g_object_unref (core_descriptor_file);
  core_descriptor = retro_core_descriptor_new (core_descriptor_path, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_debug ("%s", tmp_error->message);

    g_error_free (tmp_error);
    g_free (core_descriptor_path);

    return FALSE;
  }

  g_free (core_descriptor_path);
  g_clear_object (&self->core_descriptor);
  self->core_descriptor = core_descriptor;

  return TRUE;
}

static gboolean
retro_module_iterator_next_in_current_path (RetroModuleIterator  *self,
                                            GError              **error)
{
  GFile *directory = NULL;
  GFileInfo *info = NULL;
  gboolean found = FALSE;

  GError *tmp_error = NULL;

  if (self->sub_directory != NULL &&
      retro_module_iterator_next_in_sub_directory (self))
    return TRUE;

  directory = g_file_new_for_path (self->directories[self->current_directory]);

  if (self->file_enumerator == NULL) {
    self->file_enumerator =
      g_file_enumerate_children (directory,
                                 "",
                                 G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                 NULL,
                                 &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);
      g_object_unref (directory);
      g_clear_object (&self->file_enumerator);

      return FALSE;
    }
  }

  if (self->file_enumerator == NULL) {
    g_object_unref (directory);

    return FALSE;
  }

  while (TRUE) {
    if (info != NULL)
      g_object_unref (info);

    info = g_file_enumerator_next_file (self->file_enumerator, NULL, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);
      g_clear_object (&info);
      g_object_unref (directory);

      return FALSE;
    }

    if (info == NULL)
      break;

    found = retro_module_iterator_iterate_next_in_current_path (self, directory, info, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);
      g_object_unref (info);
      g_object_unref (directory);

      return FALSE;
    }

    if (found) {
      g_object_unref (info);
      g_object_unref (directory);

      return TRUE;
    }
  }

  g_clear_object (&self->file_enumerator);
  g_object_unref (directory);

  return FALSE;
}

/* Public */

/**
 * retro_module_iterator_get:
 * @self: a #RetroModuleIterator
 *
 * Gets the last #RetroCoreDescriptor fetched by retro_module_iterator_next(),
 * or %NULL if the end was reached.
 *
 * Returns: (nullable) (transfer full): a #RetroCoreDescriptor, or %NULL
 */
RetroCoreDescriptor *
retro_module_iterator_get (RetroModuleIterator *self)
{
  g_return_val_if_fail (RETRO_IS_MODULE_ITERATOR (self), NULL);

  return self->core_descriptor != NULL ?
    g_object_ref (self->core_descriptor) :
    NULL;
}

/**
 * retro_module_iterator_next:
 * @self: a #RetroModuleIterator
 *
 * Fetch the next #RetroModuleIterator.
 *
 * Returns: %FALSE if it reached the end, %TRUE otherwise
 */
gboolean
retro_module_iterator_next (RetroModuleIterator *self)
{
  gboolean next_in_current_path;
  GError *tmp_error = NULL;

  g_return_val_if_fail (RETRO_IS_MODULE_ITERATOR (self), FALSE);

  while (self->directories[self->current_directory] != NULL) {
    retro_module_iterator_set_current_directory_as_visited (self);

    next_in_current_path = retro_module_iterator_next_in_current_path (self, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_debug ("%s", tmp_error->message);
      g_clear_error (&tmp_error);
      next_in_current_path = FALSE;
    }

    if (next_in_current_path)
      return TRUE;

    while (self->directories[self->current_directory] != NULL &&
           retro_module_iterator_was_current_directory_visited (self))
      self->current_directory++;
  }

  g_clear_object (&self->file_enumerator);
  g_clear_object (&self->core_descriptor);
  if (self->sub_directory != NULL) {
    g_object_unref (self->sub_directory);
    self->sub_directory = NULL;
  }

  return FALSE;
}

/**
 * retro_module_iterator_new:
 * @lookup_paths: (array zero-terminated=1): paths where to look for Libretro
 * cores
 * @recursive: whether to run the query in sub-directories
 *
 * Creates a new #RetroModuleIterator.
 *
 * Returns: (transfer full): a new #RetroModuleIterator
 */
RetroModuleIterator *
retro_module_iterator_new (const gchar * const *lookup_paths,
                           gboolean             recursive)
{
  RetroModuleIterator *self;

  g_return_val_if_fail (lookup_paths != NULL, NULL);

  self = g_object_new (RETRO_TYPE_MODULE_ITERATOR, NULL);
  self->directories = g_strdupv ((gchar **) lookup_paths);
  self->recursive = recursive;
  self->visited = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_free);

  return self;
}
