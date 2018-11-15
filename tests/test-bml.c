/* test-bml.c
 *
 * Copyright (C) 2018 Adrien Plazas <kekun.plazas@laposte.net>
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
 */

#include "../retro-gtk/retro-bml-private.h"

static void
test_parse (void)
{
  g_autoptr (RetroBml) bml = NULL;
  g_autoptr (GFile) file = NULL;
  GNode *root, *node;
  gchar *name, *value;
  GHashTable *attributes;
  GError *error = NULL;

  bml = retro_bml_new ();
  file = g_file_new_for_uri ("resource:///org/gnome/Retro/Tests/RetroBml/test.bml");
  retro_bml_parse_file (bml, file, &error);
  g_assert_no_error (error);

  root = retro_bml_get_root (bml);
  g_assert_nonnull (root);
  g_assert_cmpuint (g_node_n_children (root), ==, 4);

  node = g_node_nth_child (root, 0);
  g_assert_nonnull (node);
  g_assert_cmpuint (g_node_n_children (node), ==, 2);
  name = retro_bml_node_get_name (node);
  g_assert_cmpstr (name, ==, "namespace");
  attributes = retro_bml_node_get_attributes (node);
  g_assert_nonnull (attributes);
  g_assert_cmpuint (g_hash_table_size (attributes), ==, 1);
  g_assert_cmpstr (g_hash_table_lookup (attributes, "name"), ==, "Depth");
  value = retro_bml_node_get_value (node);
  g_assert_cmpstr (value, ==, "");

  node = g_node_nth_child (root, 0);
  g_assert_nonnull (node);
  node = g_node_nth_child (node, 0);
  g_assert_nonnull (node);
  g_assert_cmpuint (g_node_n_children (node), ==, 1);
  name = retro_bml_node_get_name (node);
  g_assert_cmpstr (name, ==, "namespace");
  attributes = retro_bml_node_get_attributes (node);
  g_assert_nonnull (attributes);
  g_assert_cmpuint (g_hash_table_size (attributes), ==, 1);
  g_assert_cmpstr (g_hash_table_lookup (attributes, "name"), ==, "Test1");
  value = retro_bml_node_get_value (node);
  g_assert_cmpstr (value, ==, "");

  node = g_node_nth_child (root, 0);
  g_assert_nonnull (node);
  node = g_node_nth_child (node, 0);
  g_assert_nonnull (node);
  node = g_node_nth_child (node, 0);
  g_assert_nonnull (node);
  g_assert_cmpuint (g_node_n_children (node), ==, 0);
  name = retro_bml_node_get_name (node);
  g_assert_cmpstr (name, ==, "binary");
  attributes = retro_bml_node_get_attributes (node);
  g_assert_nonnull (attributes);
  g_assert_cmpuint (g_hash_table_size (attributes), ==, 2);
  g_assert_cmpstr (g_hash_table_lookup (attributes, "name"), ==, "testfile1");
  g_assert_cmpstr (g_hash_table_lookup (attributes, "file"), ==, "test/testfile1.test");
  value = retro_bml_node_get_value (node);
  g_assert_cmpstr (value, ==, "");

  node = g_node_nth_child (root, 0);
  g_assert_nonnull (node);
  node = g_node_nth_child (node, 1);
  g_assert_nonnull (node);
  g_assert_cmpuint (g_node_n_children (node), ==, 2);
  name = retro_bml_node_get_name (node);
  g_assert_cmpstr (name, ==, "namespace");
  attributes = retro_bml_node_get_attributes (node);
  g_assert_nonnull (attributes);
  g_assert_cmpuint (g_hash_table_size (attributes), ==, 1);
  g_assert_cmpstr (g_hash_table_lookup (attributes, "name"), ==, "Test2");
  value = retro_bml_node_get_value (node);
  g_assert_cmpstr (value, ==, "");

  node = g_node_nth_child (root, 0);
  g_assert_nonnull (node);
  node = g_node_nth_child (node, 1);
  g_assert_nonnull (node);
  node = g_node_nth_child (node, 0);
  g_assert_nonnull (node);
  g_assert_cmpuint (g_node_n_children (node), ==, 0);
  name = retro_bml_node_get_name (node);
  g_assert_cmpstr (name, ==, "binary");
  attributes = retro_bml_node_get_attributes (node);
  g_assert_nonnull (attributes);
  g_assert_cmpuint (g_hash_table_size (attributes), ==, 2);
  g_assert_cmpstr (g_hash_table_lookup (attributes, "name"), ==, "testfile2a");
  g_assert_cmpstr (g_hash_table_lookup (attributes, "file"), ==, "test/testfile2a.test");
  value = retro_bml_node_get_value (node);
  g_assert_cmpstr (value, ==, "");

  node = g_node_nth_child (root, 0);
  g_assert_nonnull (node);
  node = g_node_nth_child (node, 1);
  g_assert_nonnull (node);
  node = g_node_nth_child (node, 1);
  g_assert_nonnull (node);
  g_assert_cmpuint (g_node_n_children (node), ==, 0);
  name = retro_bml_node_get_name (node);
  g_assert_cmpstr (name, ==, "binary");
  attributes = retro_bml_node_get_attributes (node);
  g_assert_nonnull (attributes);
  g_assert_cmpuint (g_hash_table_size (attributes), ==, 2);
  g_assert_cmpstr (g_hash_table_lookup (attributes, "name"), ==, "testfile2b");
  g_assert_cmpstr (g_hash_table_lookup (attributes, "file"), ==, "test/testfile2b.test");
  value = retro_bml_node_get_value (node);
  g_assert_cmpstr (value, ==, "");

  node = g_node_nth_child (root, 1);
  g_assert_nonnull (node);
  g_assert_cmpuint (g_node_n_children (node), ==, 0);
  name = retro_bml_node_get_name (node);
  g_assert_cmpstr (name, ==, "attributes");
  attributes = retro_bml_node_get_attributes (node);
  g_assert_nonnull (attributes);
  g_assert_cmpuint (g_hash_table_size (attributes), ==, 3);
  g_assert_cmpstr (g_hash_table_lookup (attributes, "simple"), ==, "simplevalue");
  g_assert_cmpstr (g_hash_table_lookup (attributes, "quoted"), ==, "I am quoted");
  g_assert_cmpstr (g_hash_table_lookup (attributes, "lineend"), ==, "This is a line end attribute");
  value = retro_bml_node_get_value (node);
  g_assert_cmpstr (value, ==, "");

  node = g_node_nth_child (root, 2);
  g_assert_nonnull (node);
  g_assert_cmpuint (g_node_n_children (node), ==, 0);
  name = retro_bml_node_get_name (node);
  g_assert_cmpstr (name, ==, "cartridge");
  attributes = retro_bml_node_get_attributes (node);
  g_assert_nonnull (attributes);
  g_assert_cmpuint (g_hash_table_size (attributes), ==, 1);
  g_assert_cmpstr (g_hash_table_lookup (attributes, "sha256"), ==, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef");
  value = retro_bml_node_get_value (node);
  g_assert_cmpstr (value, ==, "This is a multiline value.\n");

  node = g_node_nth_child (root, 3);
  g_assert_nonnull (node);
  g_assert_cmpuint (g_node_n_children (node), ==, 0);
  name = retro_bml_node_get_name (node);
  g_assert_cmpstr (name, ==, "cartridge");
  attributes = retro_bml_node_get_attributes (node);
  g_assert_nonnull (attributes);
  g_assert_cmpuint (g_hash_table_size (attributes), ==, 1);
  g_assert_cmpstr (g_hash_table_lookup (attributes, "sha256"), ==, "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210");
  value = retro_bml_node_get_value (node);
  g_assert_cmpstr (value, ==, "This multiline value\nactually is multiline.\n");
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func("/RetroBml/parse", test_parse);

  return g_test_run();
}
