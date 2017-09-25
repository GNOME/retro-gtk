// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-module-query.h"

struct _RetroModuleQuery
{
  GObject parent_instance;
  gboolean recursive;
};

G_DEFINE_TYPE (RetroModuleQuery, retro_module_query, G_TYPE_OBJECT)

#define RETRO_MODULE_QUERY_ENV_PLUGIN_PATH "LIBRETRO_PLUGIN_PATH"

/* Private */

static void
retro_module_query_finalize (GObject *object)
{
  G_OBJECT_CLASS (retro_module_query_parent_class)->finalize (object);
}

static void
retro_module_query_class_init (RetroModuleQueryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_module_query_finalize;
}

static void
retro_module_query_init (RetroModuleQuery *self)
{
}

static gchar **
retro_module_query_get_plugin_lookup_paths ()
{
  gchar **envp;
  const gchar *env_plugin_path;
  gchar *full_plugin_path;
  gchar **result;

  envp = g_get_environ ();
  env_plugin_path = g_environ_getenv (envp, RETRO_MODULE_QUERY_ENV_PLUGIN_PATH);
  if (env_plugin_path == NULL) {
    g_strfreev (envp);

    return g_strsplit (RETRO_PLUGIN_PATH, ":", 0);
  }

  full_plugin_path = g_strconcat (env_plugin_path, ":", RETRO_PLUGIN_PATH, NULL);
  g_strfreev (envp);
  result = g_strsplit (full_plugin_path, ":", 0);
  g_free (full_plugin_path);

  return result;
}

/* Public */

/**
 * retro_module_query_iterator:
 * @self: a #RetroModuleQuery
 *
 * Creates a new #RetroModuleIterator.
 *
 * Returns: (transfer full): a new #RetroModuleIterator
 */
RetroModuleIterator *
retro_module_query_iterator (RetroModuleQuery *self)
{
  RetroModuleIterator *result;
  gchar **paths;

  g_return_val_if_fail (RETRO_IS_MODULE_QUERY (self), NULL);

  paths = retro_module_query_get_plugin_lookup_paths ();
  result = retro_module_iterator_new ((const gchar * const *) paths,
                                      self->recursive);
  g_strfreev (paths);

  return result;
}

/**
 * retro_module_query_new:
 * @recursive: whether to run the query in sub-directories
 *
 * Creates a new #RetroModuleQuery.
 *
 * Returns: (transfer full): a new #RetroModuleQuery
 */
RetroModuleQuery *
retro_module_query_new (gboolean recursive)
{
  RetroModuleQuery *self;

  self = g_object_new (RETRO_TYPE_MODULE_QUERY, NULL);
  self->recursive = recursive;

  return self;
}
