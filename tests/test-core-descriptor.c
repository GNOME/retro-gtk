/* test-core-descriptor.c
 *
 * Copyright (C) 2017 Adrien Plazas <kekun.plazas@laposte.net>
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

#include "../retro-gtk/retro-core-descriptor.h"

#define NESTOPIA_DESCRIPTOR_URI "resource:///org/gnome/Retro/TestCoreDescriptor/nestopia.libretro"
#define NES_PLATFORM "NintendoEntertainmentSystem"
#define FDS_PLATFORM "FamicomDiskSystem"
#define NES_MIME_TYPE "application/x-nes-rom"
#define FDS_MIME_TYPE "application/x-fds-disk"

static const gchar * const NES_MIME_TYPES[] = { NES_MIME_TYPE, NULL };
static const gchar * const FDS_MIME_TYPES[] = { FDS_MIME_TYPE, NULL };

static void
test_empty (void)
{
  RetroCoreDescriptor *descriptor;
  GError *error = NULL;

  descriptor = retro_core_descriptor_new_for_uri ("", &error);
  g_assert_nonnull (error);
  g_error_free (error);

  g_assert_null (descriptor);
}

static void
test_has_platform (void)
{
  RetroCoreDescriptor *descriptor;
  GError *error = NULL;

  descriptor = retro_core_descriptor_new_for_uri (NESTOPIA_DESCRIPTOR_URI,
                                                  &error);
  g_assert_no_error (error);
  g_assert_nonnull (descriptor);
  g_assert_true (RETRO_IS_CORE_DESCRIPTOR (descriptor));

  g_assert_true (retro_core_descriptor_has_platform (descriptor, NES_PLATFORM));
  g_assert_true (retro_core_descriptor_has_platform (descriptor, FDS_PLATFORM));
  g_assert_false (retro_core_descriptor_has_platform (descriptor, "dummy"));

  g_object_unref (descriptor);
}

static void
test_get_name (void)
{
  RetroCoreDescriptor *descriptor;
  gchar *name;
  GError *error = NULL;

  descriptor = retro_core_descriptor_new_for_uri (NESTOPIA_DESCRIPTOR_URI,
                                                  &error);
  g_assert_no_error (error);
  g_assert_nonnull (descriptor);
  g_assert_true (RETRO_IS_CORE_DESCRIPTOR (descriptor));

  name = retro_core_descriptor_get_name (descriptor, &error);
  g_assert_no_error (error);
  g_assert_cmpstr (name, ==, "Nestopia UE");
  g_free (name);

  g_object_unref (descriptor);
}

static void
test_get_mime_type (void)
{
  RetroCoreDescriptor *descriptor;
  gchar **mime_type;
  gsize length = 0;
  GError *error = NULL;

  descriptor = retro_core_descriptor_new_for_uri (NESTOPIA_DESCRIPTOR_URI,
                                                  &error);
  g_assert_no_error (error);
  g_assert_nonnull (descriptor);
  g_assert_true (RETRO_IS_CORE_DESCRIPTOR (descriptor));

  mime_type = retro_core_descriptor_get_mime_type (descriptor, NES_PLATFORM, &length, &error);
  g_assert_no_error (error);
  g_assert_cmpuint (length, ==, 1);
  g_assert_nonnull (mime_type);
  g_assert_cmpstr (mime_type[0], ==, NES_MIME_TYPE);
  g_assert_null (mime_type[1]);
  g_strfreev (mime_type);

  mime_type = retro_core_descriptor_get_mime_type (descriptor, FDS_PLATFORM, &length, &error);
  g_assert_no_error (error);
  g_assert_cmpuint (length, ==, 1);
  g_assert_nonnull (mime_type);
  g_assert_cmpstr (mime_type[0], ==, FDS_MIME_TYPE);
  g_assert_null (mime_type[1]);
  g_strfreev (mime_type);

  g_object_unref (descriptor);
}

static void
test_get_platform_supports_mime_types (void)
{
  RetroCoreDescriptor *descriptor;
  gboolean supports;
  GError *error = NULL;

  descriptor = retro_core_descriptor_new_for_uri (NESTOPIA_DESCRIPTOR_URI,
                                                  &error);
  g_assert_no_error (error);
  g_assert_nonnull (descriptor);
  g_assert_true (RETRO_IS_CORE_DESCRIPTOR (descriptor));

  supports = retro_core_descriptor_get_platform_supports_mime_types (descriptor, NES_PLATFORM, NES_MIME_TYPES, &error);
  g_assert_no_error (error);
  g_assert_true (supports);

  supports = retro_core_descriptor_get_platform_supports_mime_types (descriptor, FDS_PLATFORM, FDS_MIME_TYPES, &error);
  g_assert_no_error (error);
  g_assert_true (supports);

  g_object_unref (descriptor);
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/RetroCoreDescriptor/test_empty", test_empty);
  g_test_add_func ("/RetroCoreDescriptor/test_has_platform", test_has_platform);
  g_test_add_func ("/RetroCoreDescriptor/test_get_name", test_get_name);
  g_test_add_func ("/RetroCoreDescriptor/test_get_mime_type", test_get_mime_type);
  g_test_add_func ("/RetroCoreDescriptor/test_get_platform_supports_mime_types", test_get_platform_supports_mime_types);

  return g_test_run();
}
