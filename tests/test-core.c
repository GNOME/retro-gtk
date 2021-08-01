/* test-core.c
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

#include <retro-gtk.h>
#include <glib/gstdio.h>

static gchar *arg_core_filename = NULL;
static gchar *tmp_filename = NULL;

static void
test_setup (RetroCore     **core_pointer,
            gconstpointer   core_filename)
{
  *core_pointer = retro_core_new ((const gchar *) core_filename);
}

static void
test_teardown (RetroCore     **core_pointer,
               gconstpointer   data)
{
  g_object_unref (*core_pointer);
}

static gsize
get_file_size (const gchar *filename)
{
  GStatBuf buf;

  g_stat (filename, &buf);

  return buf.st_size;
}

static void
tmp_file_test_setup (RetroCore     **core_pointer,
                     gconstpointer   core_filename)
{
  gint handle;
  g_autoptr (GError) error = NULL;

  g_assert_null (tmp_filename);

  handle = g_file_open_tmp ("retro-test-core-XXXXXX", &tmp_filename, &error);
  g_assert_no_error (error);

  g_close (handle, &error);
  g_assert_no_error (error);

  g_file_set_contents (tmp_filename, NULL, 0, &error);
  g_assert_no_error (error);
  g_assert_cmpuint (get_file_size (tmp_filename), ==, 0);

  test_setup (core_pointer, core_filename);
}

static void
tmp_file_test_teardown (RetroCore     **core_pointer,
                        gconstpointer   data)
{
  test_teardown (core_pointer, data);

  g_assert_nonnull (tmp_filename);
  g_remove (tmp_filename);

  tmp_filename = NULL;
}

static void
test_boot (RetroCore     **core_pointer,
           gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);
}

static void
test_get_api_version (RetroCore     **core_pointer,
                      gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  g_assert_cmpuint (retro_core_get_api_version (core), ==, 1);
}

static void
test_get_filename (RetroCore     **core_pointer,
                   gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  g_assert_cmpstr (retro_core_get_filename (core), ==, arg_core_filename);
}

static void
test_get_game_loaded (RetroCore     **core_pointer,
                      gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  g_assert_true (retro_core_get_game_loaded (core));
}

static void
test_get_support_no_game (RetroCore     **core_pointer,
                          gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  g_assert_false (retro_core_get_support_no_game (core));
}

static void
test_get_frames_per_second (RetroCore     **core_pointer,
                            gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  g_assert_cmpfloat (retro_core_get_frames_per_second (core), ==, 60.0);
}

static void
test_get_can_access_state (RetroCore     **core_pointer,
                           gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  g_assert_false (retro_core_get_can_access_state (core));
}

static void
test_save_state (RetroCore     **core_pointer,
                 gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  retro_core_save_state (core, tmp_filename, &error);

  /* g_assert_error() should be used but the expected error domain is private. */
  g_assert_nonnull (error);
  g_clear_error (&error);

  g_test_expect_message (RETRO_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "retro_core_save_state: assertion 'filename != NULL' failed");
  retro_core_save_state (core, NULL, &error);
  g_test_assert_expected_messages ();
  g_assert_no_error (error);
}

static void
test_load_state (RetroCore     **core_pointer,
                 gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  retro_core_load_state (core, tmp_filename, &error);

  /* g_assert_error() should be used but the expected error domain is private. */
  g_assert_nonnull (error);
  g_clear_error (&error);

  g_test_expect_message (RETRO_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "retro_core_load_state: assertion 'filename != NULL' failed");
  retro_core_load_state (core, NULL, &error);
  g_test_assert_expected_messages ();
  g_assert_no_error (error);
}

static void
test_get_memory_size (RetroCore     **core_pointer,
                      gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  g_assert_cmpuint (retro_core_get_memory_size (core, RETRO_MEMORY_TYPE_SAVE_RAM), ==, 0);
  g_assert_cmpuint (retro_core_get_memory_size (core, RETRO_MEMORY_TYPE_RTC), ==, 0);
  g_assert_cmpuint (retro_core_get_memory_size (core, RETRO_MEMORY_TYPE_SYSTEM_RAM), ==, 0);
  g_assert_cmpuint (retro_core_get_memory_size (core, RETRO_MEMORY_TYPE_VIDEO_RAM), ==, 0);
}

static void
test_save_valid_memory_type (RetroCore       *core,
                             RetroMemoryType  type)
{
  gsize size;
  GError *error = NULL;

  retro_core_save_memory (core, type, tmp_filename, &error);
  size = get_file_size (tmp_filename);
  g_assert_no_error (error);
  g_assert_cmpuint (size, ==, 0);
}

static void
test_save_memory (RetroCore     **core_pointer,
                  gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  test_save_valid_memory_type (core, RETRO_MEMORY_TYPE_SAVE_RAM);
  test_save_valid_memory_type (core, RETRO_MEMORY_TYPE_RTC);
  test_save_valid_memory_type (core, RETRO_MEMORY_TYPE_SYSTEM_RAM);
  test_save_valid_memory_type (core, RETRO_MEMORY_TYPE_VIDEO_RAM);

  g_test_expect_message (RETRO_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "retro_core_save_memory: assertion 'filename != NULL' failed");
  retro_core_save_memory (core, RETRO_MEMORY_TYPE_SAVE_RAM, NULL, &error);
  g_test_assert_expected_messages ();
  g_assert_no_error (error);
}

static void
test_load_valid_memory_type (RetroCore       *core,
                             RetroMemoryType  type)
{
  GError *error = NULL;

  retro_core_load_memory (core, type, tmp_filename, &error);
  /* g_assert_error() should be used but the expected error domain is private. */
  g_assert_nonnull (error);
  g_clear_error (&error);
}

static void
test_load_memory (RetroCore     **core_pointer,
                  gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  test_load_valid_memory_type (core, RETRO_MEMORY_TYPE_SAVE_RAM);
  test_load_valid_memory_type (core, RETRO_MEMORY_TYPE_RTC);
  test_load_valid_memory_type (core, RETRO_MEMORY_TYPE_SYSTEM_RAM);
  test_load_valid_memory_type (core, RETRO_MEMORY_TYPE_VIDEO_RAM);

  g_test_expect_message (RETRO_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "retro_core_load_memory: assertion 'filename != NULL' failed");
  retro_core_load_memory (core, RETRO_MEMORY_TYPE_SAVE_RAM, NULL, &error);
  g_test_assert_expected_messages ();
  g_assert_no_error (error);
}

static void
test_has_option (RetroCore     **core_pointer,
                 gconstpointer   data)
{
  RetroCore *core = *core_pointer;
  GError *error = NULL;

  retro_core_boot (core, &error);
  g_assert_no_error (error);

  g_assert_false (retro_core_has_option (core, "non-existent-option"));
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  if (argc < 2) {
    g_error ("Please pass the location of the retro-dummy as an argument.");

    return 1;
  }
  else if (argc > 2) {
    g_error ("Too many arguments, please pass only the location of the retro-dummy as an argument.");

    return 1;
  }

  arg_core_filename = argv[1];

  g_test_add ("/RetroCore/boot", RetroCore *, arg_core_filename, test_setup, test_boot, test_teardown);
  g_test_add ("/RetroCore/get_api_version", RetroCore *, arg_core_filename, test_setup, test_get_api_version, test_teardown);
  g_test_add ("/RetroCore/get_filename", RetroCore *, arg_core_filename, test_setup, test_get_filename, test_teardown);
  g_test_add ("/RetroCore/get_game_loaded", RetroCore *, arg_core_filename, test_setup, test_get_game_loaded, test_teardown);
  g_test_add ("/RetroCore/get_support_no_game", RetroCore *, arg_core_filename, test_setup, test_get_support_no_game, test_teardown);
  g_test_add ("/RetroCore/get_frames_per_second", RetroCore *, arg_core_filename, test_setup, test_get_frames_per_second, test_teardown);
  g_test_add ("/RetroCore/get_can_access_state", RetroCore *, arg_core_filename, test_setup, test_get_can_access_state, test_teardown);
  g_test_add ("/RetroCore/save_state", RetroCore *, arg_core_filename, tmp_file_test_setup, test_save_state, tmp_file_test_teardown);
  g_test_add ("/RetroCore/load_state", RetroCore *, arg_core_filename, tmp_file_test_setup, test_load_state, tmp_file_test_teardown);
  g_test_add ("/RetroCore/get_memory_size", RetroCore *, arg_core_filename, test_setup, test_get_memory_size, test_teardown);
  g_test_add ("/RetroCore/save_memory", RetroCore *, arg_core_filename, tmp_file_test_setup, test_save_memory, tmp_file_test_teardown);
  g_test_add ("/RetroCore/load_memory", RetroCore *, arg_core_filename, tmp_file_test_setup, test_load_memory, tmp_file_test_teardown);
  g_test_add ("/RetroCore/has_option", RetroCore *, arg_core_filename, test_setup, test_has_option, test_teardown);

  return g_test_run();
}
