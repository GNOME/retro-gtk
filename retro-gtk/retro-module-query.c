// This file is part of retro-gtk. License: GPL-3.0+.

/**
 * SECTION:retro-module-query
 * @short_description: An object to query the available Libretro cores
 * @title: RetroModuleQuery
 * @See_also: #RetroCoreDescriptor, #RetroModuleIterator
 */

#include "retro-module-query.h"

#include "../retro-gtk-config.h"

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
get_plugin_lookup_paths (void)
{
  g_auto (GStrv) envp = NULL;
  const gchar *env_plugin_path;
  g_autofree gchar *full_plugin_path = NULL;

  envp = g_get_environ ();
  env_plugin_path = g_environ_getenv (envp, RETRO_MODULE_QUERY_ENV_PLUGIN_PATH);
  if (env_plugin_path == NULL)
    return g_strsplit (RETRO_PLUGIN_PATH, ":", 0);

  full_plugin_path = g_strconcat (env_plugin_path, ":", RETRO_PLUGIN_PATH, NULL);

  return g_strsplit (full_plugin_path, ":", 0);
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
  g_auto (GStrv) paths = NULL;

  g_return_val_if_fail (RETRO_IS_MODULE_QUERY (self), NULL);

  paths = get_plugin_lookup_paths ();

  return retro_module_iterator_new ((const gchar * const *) paths,
                                    self->recursive);
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
