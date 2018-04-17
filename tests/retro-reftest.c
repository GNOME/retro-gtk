/* retro-reftest.c
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

#include <string.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#define RETRO_GTK_USE_UNSTABLE_API
#include <retro-gtk/retro-gtk.h>

typedef struct {
  RetroCore *core;
  RetroPixdata *pixdata;
} RetroReftest;

static gboolean arg_generate = FALSE;
static gint arg_skip = 0;
static gchar *arg_video_file = NULL;
static gchar *arg_core_file = NULL;
static gchar **arg_media_files = NULL;
static gint arg_media_files_count = 0;

static const GOptionEntry test_args[] = {
  { "generate", 'g', 0, G_OPTION_ARG_NONE, &arg_generate,
    "Generate reference files instead of testing them", NULL },
  { "skip", 's', 0, G_OPTION_ARG_INT, &arg_skip,
    "The number of frames to skip before running the test", "FRAMES" },
  { "video", 'v', 0, G_OPTION_ARG_FILENAME, &arg_video_file,
    "The reference video output", "FILE" },
  { NULL }
};

static gboolean
parse_command_line (int    *argc,
                    char ***argv)
{
  GError *error = NULL;
  GOptionContext *context;

  context = g_option_context_new ("CORE [MEDIAS…] - Run retro-gtk reftests");
  g_option_context_add_main_entries (context, test_args, NULL);
  g_option_context_set_ignore_unknown_options (context, TRUE);

  if (!g_option_context_parse (context, argc, argv, &error)) {
    g_print ("Option parsing failed: %s\n", error->message);

    return FALSE;
  }

  if (*argc < 2) {
    g_printerr ("Mandatory argument CORE missing.\n");

    return FALSE;
  }

  arg_core_file = (*argv)[1];
  if (*argc > 2) {
    arg_media_files = &(*argv)[2];
    arg_media_files_count = *argc - 2;
  }

  gtk_test_init (argc, argv);

  if (arg_skip < 0) {
    g_printerr ("Invalid argument passed to --skip argument. It must be a positive integer.\n");
    arg_skip = 0;
  }

  g_test_set_nonfatal_assertions ();

  return TRUE;
}

/* This is a copy of pixdata_equal() from tests/test-common.c from gdk-pixbuf.
 *
 * Copyright (C) 2014 Red Hat, Inc.
 * Author: Matthias Clasen
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
static gboolean
pixdata_equal (GdkPixbuf  *test,
               GdkPixbuf  *ref,
               GError    **error)
{
  if (gdk_pixbuf_get_colorspace (test) != gdk_pixbuf_get_colorspace (ref)) {
    g_set_error (error, GDK_PIXBUF_ERROR, 0, "Image colorspace is %d but should be %d",
                 gdk_pixbuf_get_colorspace (test), gdk_pixbuf_get_colorspace (ref));

   return FALSE;
  }

  if (gdk_pixbuf_get_n_channels (test) != gdk_pixbuf_get_n_channels (ref)) {
    g_set_error (error, GDK_PIXBUF_ERROR, 0,
                 "has %u channels but should have %u",
                 gdk_pixbuf_get_n_channels (test), gdk_pixbuf_get_n_channels (ref));

   return FALSE;
  }

  if (gdk_pixbuf_get_bits_per_sample (test) != gdk_pixbuf_get_bits_per_sample (ref)) {
    g_set_error (error, GDK_PIXBUF_ERROR, 0,
                 "Image is %u bits per sample but should be %u bits per sample",
                 gdk_pixbuf_get_bits_per_sample (test), gdk_pixbuf_get_bits_per_sample (ref));

    return FALSE;
  }

  if (gdk_pixbuf_get_width (test) != gdk_pixbuf_get_width (ref) ||
      gdk_pixbuf_get_height (test) != gdk_pixbuf_get_height (ref)) {
    g_set_error (error, GDK_PIXBUF_ERROR, 0,
                 "Image size is %dx%d but should be %dx%d",
                 gdk_pixbuf_get_width (test), gdk_pixbuf_get_height (test),
                 gdk_pixbuf_get_width (ref), gdk_pixbuf_get_height (ref));

    return FALSE;
  }

  if (gdk_pixbuf_get_rowstride (test) != gdk_pixbuf_get_rowstride (ref)) {
    g_set_error (error, GDK_PIXBUF_ERROR, 0,
                 "Image rowstrides is %u bytes but should be %u bytes",
                 gdk_pixbuf_get_rowstride (test), gdk_pixbuf_get_rowstride (ref));

    return FALSE;
  }

  if (memcmp (gdk_pixbuf_get_pixels (test),
              gdk_pixbuf_get_pixels (ref),
              gdk_pixbuf_get_byte_length (test)) != 0) {
    gint x, y, width, height, n_channels, rowstride;
    const guchar *test_pixels, *ref_pixels;

    rowstride = gdk_pixbuf_get_rowstride (test);
    n_channels = gdk_pixbuf_get_n_channels (test);
    width = gdk_pixbuf_get_width (test);
    height = gdk_pixbuf_get_height (test);
    test_pixels = gdk_pixbuf_get_pixels (test);
    ref_pixels = gdk_pixbuf_get_pixels (ref);

    g_assert_cmpint (width, >=, 0);
    g_assert_cmpint (height, >=, 0);
    g_assert_cmpint (n_channels, >=, 0);

    for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
        if (memcmp (&test_pixels[x * n_channels], &ref_pixels[x * n_channels], n_channels) != 0) {
          if (n_channels == 4) {
            g_set_error (error, GDK_PIXBUF_ERROR, 0, "Image data at %ux%u is #%02X%02X%02X%02X, but should be #%02X%02X%02X%02X",
                         x, y,
                         test_pixels[x * n_channels + 0], test_pixels[x * n_channels + 1], test_pixels[x * n_channels + 2], test_pixels[x * n_channels + 3],
                         ref_pixels[x * n_channels + 0], ref_pixels[x * n_channels + 1], ref_pixels[x * n_channels + 2], ref_pixels[x * n_channels + 3]);
          }
          else if (n_channels == 3) {
            g_set_error (error, GDK_PIXBUF_ERROR, 0, "Image data at %ux%u is #%02X%02X%02X, but should be #%02X%02X%02X",
                         x, y,
                         test_pixels[x * n_channels + 0], test_pixels[x * n_channels + 1], test_pixels[x * n_channels + 2],
                         ref_pixels[x * n_channels + 0], ref_pixels[x * n_channels + 1], ref_pixels[x * n_channels + 2]);
          }
          else
            g_set_error (error, GDK_PIXBUF_ERROR, 0, "Image data differ at %ux%u", x, y);

          return FALSE;
        }
      }
      test_pixels += rowstride;
      ref_pixels += rowstride;
    }
  }

  return TRUE;
}

static void
retro_reftest_skip_frames (RetroReftest *reftest)
{
  for (gint i = 0; i < arg_skip; i++)
    retro_core_run (reftest->core);
}

static void
retro_reftest_on_video_output (RetroReftest *reftest,
                               RetroPixdata *pixdata)
{
  if (pixdata == NULL)
    return;

  if (reftest->pixdata != NULL)
    retro_pixdata_free (reftest->pixdata);
  reftest->pixdata = retro_pixdata_copy (pixdata);
}

static RetroReftest *
retro_reftest_setup (GFile *file,
                     const gchar * const *media_uris)
{
  RetroReftest *reftest = g_new0 (RetroReftest, 1);
  gchar *core_filename;
  GError *error = NULL;

  g_assert_nonnull (file);
  g_assert (g_file_query_exists (file, NULL));

  core_filename = g_file_get_path (file);
  reftest->core = retro_core_new (core_filename);
  g_free (core_filename);
  g_assert (reftest->core != NULL);

  retro_core_set_medias (reftest->core, media_uris);
  retro_core_boot (reftest->core, &error);
  g_assert_no_error (error);

  retro_reftest_skip_frames (reftest);

  g_signal_connect_swapped (reftest->core, "video-output", (GCallback) retro_reftest_on_video_output, reftest);

  retro_core_run (reftest->core);

  return reftest;
}

static void
retro_reftest_teardown (RetroReftest *reftest)
{
  g_object_unref (reftest->core);
  if (reftest->pixdata != NULL)
    retro_pixdata_free (reftest->pixdata);
  g_free (reftest);
}

static void
retro_reftest_test_video (RetroReftest *reftest)
{
  GdkPixbuf *screenshot, *reference_screenshot;
  GError *error = NULL;

  screenshot = retro_pixdata_to_pixbuf (reftest->pixdata);
  g_assert_nonnull (screenshot);

  if (arg_generate) {
    /* See http://www.libpng.org/pub/png/spec/iso/index-object.html#11textinfo for
     * description of used keys.
     */
    gdk_pixbuf_save (screenshot,
                     arg_video_file,
                     "png",
                     &error,
                     "tEXt::Software", g_get_prgname (),
                     NULL);
    g_assert_no_error (error);

    g_object_unref (screenshot);

    return;
  }

  reference_screenshot = gdk_pixbuf_new_from_file (arg_video_file, &error);
  g_assert_no_error (error);

  pixdata_equal (screenshot, reference_screenshot, &error);
  if (error != NULL) {
    g_test_message ("%s", error->message);
    g_clear_error (&error);
    g_object_unref (screenshot);
    g_object_unref (reference_screenshot);

    g_test_fail ();

    return;
  }

  g_object_unref (screenshot);
  g_object_unref (reference_screenshot);
}

int
main (int argc,
      gchar **argv)
{
  GFile *core_file;
  gchar **media_uris = NULL;
  RetroReftest *reftest;
  int result;

  g_setenv ("GDK_RENDERING", "image", FALSE);

  if (!parse_command_line (&argc, &argv))
    return 1;

  core_file = g_file_new_for_commandline_arg (arg_core_file);

  g_assert (g_file_query_file_type (core_file, 0, NULL) == G_FILE_TYPE_REGULAR);

  if (arg_media_files_count > 0) {
    media_uris = g_new0 (gchar *, arg_media_files_count + 1);
    for (int i = 0; i < arg_media_files_count; i++) {
      GFile *media_file = g_file_new_for_commandline_arg (arg_media_files[i]);

      g_assert (g_file_query_file_type (media_file, 0, NULL) == G_FILE_TYPE_REGULAR);

      media_uris[i] = g_file_get_uri (media_file);
    }
  }
  reftest = retro_reftest_setup (core_file, (const gchar* const *) media_uris);
  g_object_unref (core_file);
  g_strfreev (media_uris);

  if (arg_video_file != NULL)
    g_test_add_data_func ("/video",
                          reftest,
                          (GTestDataFunc) retro_reftest_test_video);

  result = g_test_run ();

  retro_reftest_teardown (reftest);

  return result;
}
