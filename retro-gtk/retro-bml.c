// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-bml-private.h"

/* This parses the BML markup language from Higan. It is used by the Higan
 * shader format that is supported by retro-gtk.
 */

struct _RetroBml
{
  GObject parent_instance;
  GNode *root;
};

typedef struct
{
  guint depth;
  gchar *name;
  gchar *value;
  GHashTable *attributes;
} Data;

G_DEFINE_TYPE (RetroBml, retro_bml, G_TYPE_OBJECT)

static void
free_data (Data *data)
{
  g_free (data->name);
  g_free (data->value);
  if (data->attributes)
    g_hash_table_unref (data->attributes);

  g_free (data);
}

G_DEFINE_AUTOPTR_CLEANUP_FUNC (Data, free_data);

static gboolean
free_node_data (GNode    *node,
                gpointer  user_data)
{
  free_data (node->data);

  return FALSE;
}

RetroBml *
retro_bml_new (void)
{
  return g_object_new (RETRO_TYPE_BML, NULL);
}

static void
retro_bml_finalize (GObject *object)
{
  RetroBml *self = (RetroBml *)object;

  g_node_traverse (self->root, G_IN_ORDER, G_TRAVERSE_ALL, -1, free_node_data, NULL);
  g_node_destroy (self->root);

  G_OBJECT_CLASS (retro_bml_parent_class)->finalize (object);
}

static void
retro_bml_class_init (RetroBmlClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_bml_finalize;
}

static void
retro_bml_init (RetroBml *self)
{
  self->root = g_node_new (g_new0 (Data, 1));
}

static guint
count_whitespaces (gchar *line)
{
  gchar *p;

  for (p = line; g_ascii_isspace (*p); p++);

  return p - line;
}

static gboolean
is_valid (char c)
{
  return g_ascii_isalpha (c) || g_ascii_isdigit (c) || c == '-' || c == '.';
}

static guint
count_name (gchar *line)
{
  gchar *p;

  for (p = line; is_valid (*p); p++);

  return p - line;
}

static guint
parse_whitespaces (gchar *start, gchar **end)
{
  guint length = count_whitespaces (start);

  if (end)
    *end = start + length;

  return length;
}

static gchar *
parse_name (gchar   *start,
            gchar  **end,
            GError **error)
{
  guint length = count_name (start);

  if (end)
    *end = start + length;

  if (length == 0) {
    g_set_error (error,
                 RETRO_TYPE_BML,
                 RETRO_BML_ERROR_NOT_NAME,
                 "Expected a name, got %s.",
                 start);

    return NULL;
  }

  return g_strndup (start, length);
}

static gchar *
parse_value (gchar   *start,
             gchar  **end,
             GError **error)
{
  guint length = 0;

  if(start[0] == '=' && start[1] == '\"') {
    start += 2;
    /* Parse quoted values. */
    for (length = 0; start[length] && start[length] != '\"'; length++);
    if(start[length] != '\"') {
      g_set_error (error,
                   RETRO_TYPE_BML,
                   RETRO_BML_ERROR_NOT_QUOTED_VALUE,
                   "Expected a quoted value, got %s: closing quote not found.",
                   start);

      return NULL;
    }

    if (end)
      *end = start + length + 1;
  }
  else if(start[0] == '=') {
    start++;
    /* Parse unquoted values */
    for (length = 0; start[length] && start[length] != '\"' && start[length] != ' '; length++);
    if(start[length] == '\"') {
      g_set_error (error,
                   RETRO_TYPE_BML,
                   RETRO_BML_ERROR_NOT_VALUE,
                   "Expected a value, got %s: illegal character '%c'.",
                   start, start[length]);

      return NULL;
    }

    if (end)
      *end = start + length;
  }
  else if(start[0] == ':') {
    start++;
    for (length = 0; start[length]; length++);

    if (end)
      *end = start + length;
  }

  return g_strndup (start, length);
}

/* Attributes are name-value pairs following the node's name on the same line.
 * They can take the following forms:
 * - name=value
 * - name="long value"
 * - name:value to the line end
 */
static GHashTable *
parse_attributes (gchar   *start,
                  gchar  **end,
                  GError **error)
{
  g_autoptr (GHashTable) attributes =
    g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
  g_autoptr (GError) tmp_error = NULL;

  while(start[0]) {
    g_autofree gchar *name = NULL;
    g_autofree gchar *value = NULL;

    if(start[0] != ' ') {
      /* Cheating a bit as what we expect isn't a name per se, but the spaces
       * after the names didn't get parsed so if there are no spaces, it means
       * there was an illegal character in the name.
       */
      g_set_error (error,
                   RETRO_TYPE_BML,
                   RETRO_BML_ERROR_NOT_NAME,
                   "Expected a name, got the illegal character '%c'.",
                   *start);

      return NULL;
    }

    while (start[0] == ' ')
      start++;

    if(start[0] == '/' && start[1] == '/')
      break;

    name = parse_name (start, &start, &tmp_error);
    if (tmp_error != NULL) {
      g_propagate_error (error, g_steal_pointer (&tmp_error));

      return NULL;
    }

    value = parse_value (start, &start, &tmp_error);
    if (tmp_error != NULL) {
      g_propagate_error (error, g_steal_pointer (&tmp_error));

      return NULL;
    }

    g_strchomp (value);

    g_hash_table_insert (attributes,
                         g_steal_pointer (&name),
                         g_steal_pointer (&value));
  }

  return g_steal_pointer (&attributes);
}

static void
parse_stream (RetroBml      *self,
              GInputStream  *stream,
              GError       **error)
{
  g_autoptr (GDataInputStream) data_stream = g_data_input_stream_new (stream);
  gsize length;
  GNode *parent_node;
  g_autoptr (GError) tmp_error = NULL;

  parent_node = self->root;

  while (TRUE) {
    g_autofree gchar *line = NULL;
    gchar *start;
    g_autoptr (Data) data = g_new0 (Data, 1);
    GNode *current_node;

    line = g_data_input_stream_read_line (data_stream, &length, NULL, &tmp_error);
    if (tmp_error != NULL) {
      g_propagate_error (error, g_steal_pointer (&tmp_error));

      return;
    }

    if (line == NULL)
      break;

    g_strchomp (line);
    if (line[0] == '\0')
      continue;

    start = line;

    data->depth = parse_whitespaces (start, &start);
    if (line[data->depth] == '/' && line[data->depth + 1] == '/')
      continue;

    while (data->depth + 1 <= ((Data *) parent_node->data)->depth)
      parent_node = parent_node->parent;

    /* Parse multi-line values starting with ':'. */
    if (start[0] == ':') {
      data->value = g_strdup_printf ("%s%s\n",
                                     ((Data *) parent_node->data)->value,
                                     start + 1);
      ((Data *) parent_node->data)->value = g_steal_pointer (&data->value);

      continue;
    }

    data->name = parse_name (start, &start, &tmp_error);
    if (tmp_error != NULL) {
      g_propagate_error (error, g_steal_pointer (&tmp_error));

      return;
    }

    data->value = parse_value (start, &start, &tmp_error);
    if (tmp_error != NULL) {
      g_propagate_error (error, g_steal_pointer (&tmp_error));

      return;
    }

    data->attributes = parse_attributes (start, &start, &tmp_error);
    if (tmp_error != NULL) {
      g_propagate_error (error, g_steal_pointer (&tmp_error));

      return;
    }

    /* Ensure all nodes are children of the RetroBml_private_offset. */
    data->depth++;

    current_node = g_node_new (g_steal_pointer (&data));
    g_node_append (parent_node, current_node);
    parent_node = current_node;
  }
}

void
retro_bml_parse_file (RetroBml  *self,
                      GFile     *file,
                      GError   **error)
{
  g_autoptr (GFileInputStream) stream = NULL;
  g_autoptr (GError) tmp_error = NULL;
  
  stream = g_file_read (file, NULL, error);
  if (G_UNLIKELY (tmp_error != NULL)) {
    g_propagate_error (error, g_steal_pointer (&tmp_error));

    return;
  }

  parse_stream (self, G_INPUT_STREAM (stream), error);
}

GNode *
retro_bml_get_root (RetroBml  *self)
{
  return self->root;
}

gchar *
retro_bml_node_get_name (GNode *node)
{
  Data *data = node->data;

  return data->name;
}

gchar *
retro_bml_node_get_value (GNode *node)
{
  Data *data = node->data;

  return data->value;
}

GHashTable *
retro_bml_node_get_attributes (GNode *node)
{
  Data *data = node->data;

  return data->attributes;
}

G_DEFINE_QUARK (retro-bml-error, retro_bml_error)
