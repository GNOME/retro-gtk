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

#include "retro-reftest-file.h"
#include "retro-test-controller.h"
#include <glib/gstdio.h>

typedef struct {
  guint refs;
  RetroCore *core;
  GArray *controllers;
  gsize controllers_length;
  guint next_frame;
  RetroPixdata *pixdata;
} RetroReftestData;

typedef struct {
  guint refs;
  RetroReftestData *data;
  GHashTable *options;
} RetroReftestOptions;

typedef struct {
  guint refs;
  RetroReftestData *data;
  guint target_frame;
  GHashTable *controller_states;
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
    if (self->controllers != NULL)
      g_array_unref (self->controllers);
    g_free (self);

    return;
  }

  self->refs--;
}

static void
retro_reftest_options_unref (RetroReftestOptions *self) {
  g_return_if_fail (self != NULL);

  if (self->refs == 0) {
    retro_reftest_data_unref (self->data);
    g_hash_table_unref (self->options);
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
    if (self->controller_states != NULL)
      g_hash_table_unref (self->controller_states);
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
retro_reftest_test_options (RetroReftestOptions *options)
{
  RetroCore *core;
  RetroOption *option;
  gchar *options_key, *core_key;
  const gchar **options_values, **core_values;
  GHashTableIter iter;
  gsize i;
  RetroOptionIterator *option_iterator;

  core = options->data->core;

  g_hash_table_iter_init (&iter, options->options);
  while (g_hash_table_iter_next (&iter, (gpointer) &options_key, (gpointer) &options_values)) {
    if (retro_core_has_option (core, options_key))
      continue;

    g_test_fail ();
    g_test_message ("Expected option not found: %s.", options_key);

    return;
  }

  option_iterator = retro_core_iterate_options (core);
  g_hash_table_iter_init (&iter, options->options);
  while (retro_option_iterator_next (option_iterator, (gpointer) &core_key, (gpointer) &option)) {
    core_values = retro_option_get_values (option);
    if (!g_hash_table_contains (options->options, core_key)) {
      g_test_fail ();
      g_test_message ("Unexpected option found: %s.", core_key);
      g_object_unref (option_iterator);

      return;
    }

    options_values = g_hash_table_lookup (options->options, core_key);
    for (i = 0; core_values[i] != NULL && options_values[i] != NULL; i++) {
      if (g_str_equal (core_values[i], options_values[i]))
        continue;

      g_test_fail ();
      g_test_message ("Unexpxected value found for option %s: expected %s, got %s.", core_key, options_values[i], core_values[i]);
      g_object_unref (option_iterator);

      return;
    }

    if (options_values[i] != NULL) {
      g_test_fail ();
      g_test_message ("Expected value not found for option %s: %s.", core_key, options_values[i]);
      g_object_unref (option_iterator);

      return;
    }

    if (core_values[i] != NULL) {
      g_test_fail ();
      g_test_message ("Unexpected value found for option %s: %s.", core_key, core_values[i]);
      g_object_unref (option_iterator);

      return;
    }
  }
  g_object_unref (option_iterator);
}

static void
retro_reftest_test_fast_forward (RetroReftestRun *run)
{
  guint target_frame = run->target_frame;
  guint next_frame = run->data->next_frame;

  g_assert_cmpuint (next_frame, <, target_frame);

  for (; next_frame < target_frame; next_frame++)
    retro_core_iteration (run->data->core);

  run->data->next_frame = next_frame;

  g_assert_cmpuint (target_frame, ==, next_frame);
}

static void
retro_reftest_test_run (RetroReftestRun *run)
{
  GHashTableIter iter;
  guint *controller_i;
  GArray *states;
  gsize state_i;
  RetroControllerState *state;
  RetroTestController *controller;

  guint target_frame = run->target_frame;
  guint next_frame = run->data->next_frame;

  g_assert_cmpuint (next_frame, ==, target_frame);

  if (run->controller_states != NULL) {
    g_hash_table_iter_init (&iter, run->controller_states);
    while (g_hash_table_iter_next (&iter, (gpointer) &controller_i, (gpointer) &states)) {
      if (*controller_i >= run->data->controllers_length)
        continue;

      controller = g_array_index (run->data->controllers, RetroTestController *, *controller_i);
      if (controller == NULL)
        continue;

      retro_test_controller_reset (controller);
      state_i = 0;
      for (state_i = 0;
           (state = g_array_index (states, RetroControllerState *, state_i)) != NULL;
           state_i++) {

        retro_test_controller_set_input_state (controller, state);
      }
    }
  }

  retro_core_iteration (run->data->core);
  next_frame++;

  run->data->next_frame = next_frame;

  g_assert_cmpuint (target_frame + 1, ==, next_frame);
}

static void
retro_reftest_test_state_none (RetroReftestData *data)
{
  if (retro_core_get_can_access_state (data->core))
    g_test_fail ();
}

static void
retro_reftest_test_state_refresh (RetroReftestData *data)
{
  g_autofree gchar *filename = NULL, *tmpname = NULL;
  GError *error = NULL;

  if (!retro_core_get_can_access_state (data->core)) {
    g_test_fail ();

    return;
  }

  tmpname = g_strdup_printf ("retro-reftest-state-%d", g_random_int ());
  filename = g_build_filename (g_get_tmp_dir (), tmpname, NULL);

  retro_core_save_state (data->core, filename, &error);
  g_assert_no_error (error);
  retro_core_load_state (data->core, filename, &error);
  g_assert_no_error (error);

  g_remove (filename);
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
retro_reftest_add_options_test (RetroReftestFile *reftest_file,
                                RetroReftestData *data)
{
  RetroReftestOptions *options;
  gchar *test_path;
  GError *error = NULL;

  options = g_new0 (RetroReftestOptions, 1);
  options->data = retro_reftest_data_ref (data);
  options->options = retro_reftest_file_get_options (reftest_file, &error);
  g_assert_no_error (error);
  test_path = g_strdup_printf ("%s/Options",
                               retro_reftest_file_peek_path (reftest_file));
  g_test_add_data_func_full (test_path,
                             options,
                             (GTestDataFunc) retro_reftest_test_options,
                             (GDestroyNotify) retro_reftest_options_unref);
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
  run->target_frame = frame_number;
  test_path = g_strdup_printf ("%s/%u/FastForward",
                               retro_reftest_file_peek_path (reftest_file),
                               frame_number);
  g_test_add_data_func_full (test_path,
                             run,
                             (GTestDataFunc) retro_reftest_test_fast_forward,
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
  GError *error = NULL;

  run = g_new0 (RetroReftestRun, 1);
  run->data = retro_reftest_data_ref (data);
  run->target_frame = frame_number;
  run->controller_states = retro_reftest_file_get_controller_states (reftest_file, frame_number, &error);
  g_assert_no_error (error);
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
retro_reftest_add_state_test (RetroReftestFile *reftest_file,
                              guint             frame_number,
                              RetroReftestData *data)
{
  gchar *state;
  gchar *test_path;
  GError *error = NULL;

  state = retro_reftest_file_get_state (reftest_file, frame_number, &error);
  g_assert_no_error (error);

  if (g_str_equal (state, "None")) {
    test_path = g_strdup_printf ("%s/%u/State None",
                                 retro_reftest_file_peek_path (reftest_file),
                                 frame_number);
    g_test_add_data_func_full (test_path,
                               retro_reftest_data_ref (data),
                               (GTestDataFunc) retro_reftest_test_state_none,
                               (GDestroyNotify) retro_reftest_data_unref);
    g_free (test_path);
  }
  else if (g_str_equal (state, "Refresh")) {
    test_path = g_strdup_printf ("%s/%u/State Refresh",
                                 retro_reftest_file_peek_path (reftest_file),
                                 frame_number);
    g_test_add_data_func_full (test_path,
                               retro_reftest_data_ref (data),
                               (GTestDataFunc) retro_reftest_test_state_refresh,
                               (GDestroyNotify) retro_reftest_data_unref);
    g_free (test_path);
  }
  else {
    g_critical ("Not a state test: %s.", state);
    g_assert_not_reached ();
  }
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
  gboolean has_test;
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
  data->controllers = retro_reftest_file_get_controllers (reftest_file,
                                                          &data->controllers_length,
                                                          &error);
  if (error != NULL) {
    gchar *path = g_file_get_path (file);
    g_critical ("Invalid test file %s: %s", path, error->message);
    g_free (path);
    retro_reftest_data_unref (data);
    g_object_unref (reftest_file);
    g_clear_error (&error);

    return;
  }
  if (data->controllers != NULL)
    for (gsize i = 0; i < data->controllers_length; i++)
      if (g_array_index (data->controllers, RetroTestController *, i) != NULL)
        retro_core_set_controller (data->core, i,
                                   RETRO_CONTROLLER (g_array_index (data->controllers, RetroTestController *, i)));

  /* Boot */
  retro_reftest_add_boot_test (reftest_file, data);

  /* Options */
  if (retro_reftest_file_has_options (reftest_file))
    retro_reftest_add_options_test (reftest_file, data);

  frames = retro_reftest_file_get_frames (reftest_file);
  current_frame_number = 0;
  for (frame = frames; frame != NULL; frame = frame->next) {
    frame_number = *((guint *) frame->data);

    /* FastForward */
    if (current_frame_number < frame_number)
      retro_reftest_add_fast_forward_test (reftest_file, frame_number, data);

    /* State */
    has_test = retro_reftest_file_has_state (reftest_file, frame_number, &error);
    g_assert_no_error (error);
    if (has_test)
      retro_reftest_add_state_test (reftest_file, frame_number, data);

    /* Run */
    retro_reftest_add_run_test (reftest_file, frame_number, data);

    /* Video */
    has_test = retro_reftest_file_has_video (reftest_file, frame_number, &error);
    g_assert_no_error (error);
    if (has_test)
      retro_reftest_add_video_test (reftest_file, frame_number, data);

    current_frame_number = frame_number + 1;
  }
  g_list_free (frames);
  retro_reftest_data_unref (data);

  g_signal_connect_swapped (data->core, "video-output", (GCallback) retro_reftest_on_video_output, data);
}

int
main (int argc,
      gchar **argv)
{
  GFile *file;
  gint i;

  g_setenv ("GDK_RENDERING", "image", FALSE);

  if (!parse_command_line (&argc, &argv))
    return 1;

  for (i = 1; i < argc; i++) {
    file = g_file_new_for_commandline_arg (argv[i]);
    retro_reftest_setup_for_file (file);
    g_object_unref (file);
  }

  return g_test_run ();
}
