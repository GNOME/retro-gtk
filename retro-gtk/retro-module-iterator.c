// This file is part of retro-gtk. License: GPL-3.0+.

/**
 * SECTION:retro-module-iterator
 * @short_description: An object iterating through the available Libretro cores
 * @title: RetroModuleIterator
 * @See_also: #RetroCoreDescriptor, #RetroModuleQuery
 */

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
new_for_subdirectory (const gchar *lookup_path,
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
was_current_directory_visited (RetroModuleIterator *self)
{
  g_autoptr (GFile) current_directory_file = NULL;
  g_autofree gchar *current_directory_path = NULL;
  gboolean result;

  current_directory_file = g_file_new_for_path (self->directories[self->current_directory]);
  current_directory_path = g_file_get_path (current_directory_file);
  result = g_hash_table_contains (self->visited, current_directory_path);

  return result;
}

static void
set_current_directory_as_visited (RetroModuleIterator *self)
{
  g_autoptr (GFile) current_directory_file = NULL;
  g_autofree gchar *current_directory_path = NULL;

  current_directory_file = g_file_new_for_path (self->directories[self->current_directory]);
  current_directory_path = g_file_get_path (current_directory_file);
  g_hash_table_add (self->visited, current_directory_path);
}

static gboolean
next_in_sub_directory (RetroModuleIterator *self)
{
  if (retro_module_iterator_next (self->sub_directory)) {
    g_clear_object (&self->core_descriptor);
    self->core_descriptor = retro_module_iterator_get (self->sub_directory);

    return TRUE;
  }

  g_clear_object (&self->sub_directory);

  return FALSE;
}

static gboolean
iterate_next_in_current_path (RetroModuleIterator  *self,
                              GFile                *directory,
                              GFileInfo            *info,
                              GError              **error)
{
  const gchar *core_descriptor_basename;
  g_autoptr (GFile) core_descriptor_file = NULL;
  g_autofree gchar *core_descriptor_path = NULL;
  RetroCoreDescriptor *core_descriptor;
  GError *tmp_error = NULL;

  g_assert (G_IS_FILE (directory));
  g_assert (G_IS_FILE_INFO (info));

  if (self->recursive &&
      g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY &&
      self->sub_directory == NULL) {
    const gchar *sub_directory_basename;
    g_autoptr (GFile) sub_directory_file = NULL;
    g_autofree gchar *sub_directory_path = NULL;

    sub_directory_basename = g_file_info_get_name (info);
    sub_directory_file = g_file_get_child (directory, sub_directory_basename);
    sub_directory_path = g_file_get_path (sub_directory_file);

    if (g_hash_table_contains (self->visited, sub_directory_path))
      return FALSE;

    self->sub_directory = new_for_subdirectory (sub_directory_path, self->visited);

    return next_in_sub_directory (self);
  }

  core_descriptor_basename = g_file_info_get_name (info);
  if (!g_str_has_suffix (core_descriptor_basename, ".libretro"))
    return FALSE;

  core_descriptor_file = g_file_get_child (directory, core_descriptor_basename);
  core_descriptor_path = g_file_get_path (core_descriptor_file);
  core_descriptor = retro_core_descriptor_new (core_descriptor_path, &tmp_error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_debug ("%s", tmp_error->message);

    g_error_free (tmp_error);

    return FALSE;
  }

  g_clear_object (&self->core_descriptor);
  self->core_descriptor = core_descriptor;

  return TRUE;
}

static gboolean
next_in_current_path (RetroModuleIterator  *self,
                      GError              **error)
{
  g_autoptr (GFile) directory = NULL;
  gboolean found = FALSE;
  GError *tmp_error = NULL;

  if (self->sub_directory != NULL && next_in_sub_directory (self))
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
      g_clear_object (&self->file_enumerator);

      return FALSE;
    }
  }

  if (self->file_enumerator == NULL)
    return FALSE;

  while (TRUE) {
    g_autoptr (GFileInfo) info = NULL;

    info = g_file_enumerator_next_file (self->file_enumerator, NULL, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);

      return FALSE;
    }

    if (info == NULL)
      break;

    found = iterate_next_in_current_path (self, directory, info, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_propagate_error (error, tmp_error);

      return FALSE;
    }

    if (found)
      return TRUE;
  }

  g_clear_object (&self->file_enumerator);

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
  gboolean found_next_in_current_path;
  GError *tmp_error = NULL;

  g_return_val_if_fail (RETRO_IS_MODULE_ITERATOR (self), FALSE);

  while (self->directories[self->current_directory] != NULL) {
    set_current_directory_as_visited (self);

    found_next_in_current_path = next_in_current_path (self, &tmp_error);
    if (G_UNLIKELY (tmp_error != NULL)) {
      g_debug ("%s", tmp_error->message);
      g_clear_error (&tmp_error);
      found_next_in_current_path = FALSE;
    }

    if (found_next_in_current_path)
      return TRUE;

    while (self->directories[self->current_directory] != NULL &&
           was_current_directory_visited (self))
      self->current_directory++;
  }

  g_clear_object (&self->file_enumerator);
  g_clear_object (&self->core_descriptor);
  g_clear_object (&self->sub_directory);

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
