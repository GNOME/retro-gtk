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

#include <X11/extensions/XTest.h>
#include <X11/keysym.h>

#include "retro-reftest-file.h"

typedef struct {
  guint refs;
  RetroCore *core;
  guint next_frame;
  RetroPixdata *pixdata;
} RetroReftestData;

typedef struct {
  guint refs;
  RetroReftestData *data;
  guint target_frame;
} RetroReftestRun;

typedef struct {
  guint refs;
  RetroReftestData *data;
  GFile *video_file;
} RetroReftestVideo;

static gboolean arg_generate = FALSE;
static gchar **arg_media_files = NULL;
static gint arg_media_files_count = 0;

static const GOptionEntry test_args[] = {
  { "generate", 'g', 0, G_OPTION_ARG_NONE, &arg_generate,
    "Generate reference files instead of testing them", NULL },
  { NULL }
};

static RetroReftestData *
retro_reftest_data_ref (RetroReftestData *self) {
  g_return_val_if_fail (self != NULL, NULL);

  self->refs++;

  return self;
}

static void
retro_reftest_data_unref (RetroReftestData *self) {
  g_return_if_fail (self != NULL);

  if (self->refs == 0) {
    g_object_unref (self->core);
    retro_pixdata_free (self->pixdata);
    g_free (self);

    return;
  }

  self->refs--;
}

static void
retro_reftest_run_unref (RetroReftestRun *self) {
  g_return_if_fail (self != NULL);

  if (self->refs == 0) {
    retro_reftest_data_unref (self->data);
    g_free (self);

    return;
  }

  self->refs--;
}

static void
retro_reftest_video_unref (RetroReftestVideo *self) {
  g_return_if_fail (self != NULL);

  if (self->refs == 0) {
    retro_reftest_data_unref (self->data);
    g_object_unref (self->video_file);
    g_free (self);

    return;
  }

  self->refs--;
}

static gboolean
parse_command_line (int    *argc,
                    char ***argv)
{
  GError *error = NULL;
  GOptionContext *context;

  context = g_option_context_new ("TESTS… - Run retro-gtk reftests");
  g_option_context_add_main_entries (context, test_args, NULL);
  g_option_context_set_ignore_unknown_options (context, TRUE);

  if (!g_option_context_parse (context, argc, argv, &error)) {
    g_print ("Option parsing failed: %s\n", error->message);

    return FALSE;
  }

  gtk_test_init (argc, argv);

  if (*argc > 1) {
    arg_media_files = &(*argv)[1];
    arg_media_files_count = *argc - 1;
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
retro_reftest_on_video_output (RetroReftestData *data,
                               RetroPixdata     *pixdata)
{
  if (pixdata == NULL)
    return;

  if (data->pixdata != NULL)
    retro_pixdata_free (data->pixdata);
  data->pixdata = retro_pixdata_copy (pixdata);
}

static void
retro_reftest_test_boot (RetroReftestData *data)
{
  GError *error = NULL;

  retro_core_boot (data->core, &error);
  g_assert_no_error (error);
}

static void
retro_reftest_test_run (RetroReftestRun *run)
{
  guint target_frame = run->target_frame;
  guint next_frame = run->data->next_frame;

  g_assert_cmpuint (next_frame, <=, target_frame);

  for (; next_frame < target_frame + 1; next_frame++)
    retro_core_run (run->data->core);

  run->target_frame = target_frame;
  run->data->next_frame = next_frame;

  g_assert_cmpuint (target_frame + 1, ==, next_frame);
}

static void
retro_reftest_test_state_refresh (RetroReftestData *data)
{
  GBytes *state;
  GError *error = NULL;

  if (!retro_core_get_can_access_state (data->core)) {
    g_test_fail ();

    return;
  }

  state = retro_core_get_state (data->core, &error);
  g_assert_no_error (error);
  retro_core_set_state (data->core, state, &error);
  g_assert_no_error (error);

  g_bytes_unref (state);
}

static void
retro_reftest_test_video (RetroReftestVideo *video)
{
  GdkPixbuf *screenshot, *reference_screenshot;
  gchar *path;
  GError *error = NULL;

  screenshot = retro_pixdata_to_pixbuf (video->data->pixdata);
  g_assert_nonnull (screenshot);

  path = g_file_get_path (video->video_file);

  if (arg_generate) {
    /* See http://www.libpng.org/pub/png/spec/iso/index-object.html#11textinfo for
     * description of used keys.
     */
    gdk_pixbuf_save (screenshot,
                     path,
                     "png",
                     &error,
                     "tEXt::Software", g_get_prgname (),
                     NULL);
    g_assert_no_error (error);

    g_free (path);
    g_object_unref (screenshot);

    return;
  }

  reference_screenshot = gdk_pixbuf_new_from_file (path, &error);
  g_assert_no_error (error);
  g_free (path);

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

static void
retro_reftest_add_boot_test (RetroReftestFile *reftest_file,
                             RetroReftestData *data)
{
  gchar *test_path;

  test_path = g_strdup_printf ("%s/Boot",
                               retro_reftest_file_peek_path (reftest_file));
  g_test_add_data_func_full (test_path,
                             retro_reftest_data_ref (data),
                             (GTestDataFunc) retro_reftest_test_boot,
                             (GDestroyNotify) retro_reftest_data_unref);
  g_free (test_path);
}

static void
retro_reftest_add_fast_forward_test (RetroReftestFile *reftest_file,
                                     guint             frame_number,
                                     RetroReftestData *data)
{
  RetroReftestRun *run;
  gchar *test_path;

  run = g_new0 (RetroReftestRun, 1);
  run->data = retro_reftest_data_ref (data);
  run->target_frame = frame_number - 1;
  test_path = g_strdup_printf ("%s/%u/FastForward",
                               retro_reftest_file_peek_path (reftest_file),
                               frame_number);
  g_test_add_data_func_full (test_path,
                             run,
                             (GTestDataFunc) retro_reftest_test_run,
                             (GDestroyNotify) retro_reftest_run_unref);
  g_free (test_path);
}

static void
retro_reftest_add_run_test (RetroReftestFile *reftest_file,
                            guint             frame_number,
                            RetroReftestData *data)
{
  RetroReftestRun *run;
  gchar *test_path;

  run = g_new0 (RetroReftestRun, 1);
  run->data = retro_reftest_data_ref (data);
  run->target_frame = frame_number;
  test_path = g_strdup_printf ("%s/%u/Run",
                               retro_reftest_file_peek_path (reftest_file),
                               frame_number);
  g_test_add_data_func_full (test_path,
                             run,
                             (GTestDataFunc) retro_reftest_test_run,
                             (GDestroyNotify) retro_reftest_run_unref);
  g_free (test_path);
}

static void
retro_reftest_add_state_refresh_test (RetroReftestFile *reftest_file,
                                      guint             frame_number,
                                      RetroReftestData *data)
{
  gchar *test_path;

  test_path = g_strdup_printf ("%s/%u/StateRefresh",
                               retro_reftest_file_peek_path (reftest_file),
                               frame_number);
  g_test_add_data_func_full (test_path,
                             retro_reftest_data_ref (data),
                             (GTestDataFunc) retro_reftest_test_state_refresh,
                             (GDestroyNotify) retro_reftest_data_unref);
  g_free (test_path);
}

static void
retro_reftest_add_video_test (RetroReftestFile *reftest_file,
                              guint             frame_number,
                              RetroReftestData *data)
{
  RetroReftestVideo *video;
  gchar *test_path;
  GError *error = NULL;

  video = g_new0 (RetroReftestVideo, 1);
  video->data = retro_reftest_data_ref (data);
  video->video_file = retro_reftest_file_get_video (reftest_file, frame_number, &error);
  g_assert_no_error (error);
  test_path = g_strdup_printf ("%s/%u/Video",
                               retro_reftest_file_peek_path (reftest_file),
                               frame_number);
  g_test_add_data_func_full (test_path,
                             video,
                             (GTestDataFunc) retro_reftest_test_video,
                             (GDestroyNotify) retro_reftest_video_unref);
  g_free (test_path);
}

static void
retro_reftest_setup_for_file (GFile *file)
{
  RetroReftestFile *reftest_file;
  GList *frames, *frame;
  guint current_frame_number, frame_number;
  gchar **tests;
  gsize tests_length, tests_i;
  RetroReftestData *data;
  GError *error = NULL;

  reftest_file = retro_reftest_file_new (file);
  data = g_new0 (RetroReftestData, 1);
  data->core = retro_reftest_file_get_core (reftest_file, &error);
  if (error != NULL) {
    gchar *path = g_file_get_path (file);
    g_critical ("Invalid test file %s: %s", path, error->message);
    g_free (path);
    retro_reftest_data_unref (data);
    g_object_unref (reftest_file);
    g_clear_error (&error);

    return;
  }

  retro_reftest_add_boot_test (reftest_file, data);

  frames = retro_reftest_file_get_frames (reftest_file);
  current_frame_number = 0;
  for (frame = frames; frame != NULL; frame = frame->next) {
    frame_number = *((guint *) frame->data);

    if (current_frame_number < frame_number) {
      retro_reftest_add_fast_forward_test (reftest_file, frame_number, data);
    }
    current_frame_number = frame_number + 1;

    tests_length = 0;
    tests = retro_reftest_file_get_tests (reftest_file,
                                          frame_number,
                                          &tests_length,
                                          &error);
    g_assert_no_error (error);
    for (tests_i = 0; tests != NULL && tests[tests_i] != NULL; tests_i++) {
      if (g_str_equal (tests[tests_i], "Run"))
        retro_reftest_add_run_test (reftest_file, frame_number, data);
      else if (g_str_equal (tests[tests_i], "StateRefresh"))
        retro_reftest_add_state_refresh_test (reftest_file, frame_number, data);
      else if (g_str_equal (tests[tests_i], "Video"))
        retro_reftest_add_video_test (reftest_file, frame_number, data);
    }
  }
  g_list_free (frames);
  retro_reftest_data_unref (data);

  g_signal_connect_swapped (data->core, "video-output", (GCallback) retro_reftest_on_video_output, data);
}

int
main (int argc,
      gchar **argv)
{
  Display *display = NULL;
  int event_base, error_base, major_version, minor_version;
  GFile *file;
  gint i, result;

  g_setenv ("GDK_RENDERING", "image", FALSE);


  display = XOpenDisplay (NULL);

  if (display == NULL) {
    g_critical ("Cannot open display.");

    return 1;
  }

  if (!XTestQueryExtension (display, &event_base, &error_base, &major_version, &minor_version)) {
    g_critical ("No XTest extension.");

    return 1;
  }

  if (!parse_command_line (&argc, &argv))
    return 1;

  for (i = 1; i < argc; i++) {
    file = g_file_new_for_commandline_arg (argv[i]);
    retro_reftest_setup_for_file (file);
    g_object_unref (file);
  }

  result = g_test_run ();

  XCloseDisplay (display);

  return result;
}
