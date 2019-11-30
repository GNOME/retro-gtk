// This file is part of retro-gtk. License: GPL-3.0+.

#include <retro-gtk.h>

#define RETRO_TYPE_DEMO_APPLICATION (retro_demo_application_get_type())

G_DECLARE_FINAL_TYPE (RetroDemoApplication, retro_demo_application, RETRO, DEMO_APPLICATION, GtkApplication)

struct _RetroDemoApplication
{
  GtkApplication parent_instance;

  RetroCore *core;
};

G_DEFINE_TYPE (RetroDemoApplication, retro_demo_application, GTK_TYPE_APPLICATION)

static void
retro_demo_open (GApplication  *application,
                 GFile        **files,
                 gint           n_files,
                 const gchar   *hint)
{
  RetroDemoApplication *self;
  char *module_path;
  GError *error = NULL;

  self = RETRO_DEMO_APPLICATION (application);

  if (n_files < 1)
    return;

  if (!g_file_query_exists (files[0], NULL))
    return;

  module_path = g_file_get_path (files[0]);
  self->core = retro_core_new (module_path);
  g_free (module_path);

  if (self->core == NULL)
    return;

  if (n_files > 1) {
    gchar **medias;
    gint i;

    medias = g_new0 (gchar *, n_files);
    for (i = 1; i < n_files; i++)
      medias[i - 1] = g_file_get_uri (files[i]);
    retro_core_set_medias (self->core, (const gchar *const *) medias);
    g_strfreev (medias);
  }

  retro_core_boot (self->core, &error);
  if (error != NULL) {
    g_debug ("Couldn't initialize the Libretro core: %s", error->message);
    g_error_free (error);

    return;
  }

  g_application_activate (application);
}

static RetroDemoApplication *
retro_demo_application_new (void)
{
  return g_object_new (RETRO_TYPE_DEMO_APPLICATION, NULL);
}

static void
retro_demo_application_finalize (GObject *object)
{
  RetroDemoApplication *self = RETRO_DEMO_APPLICATION (object);

  if (self->core != NULL)
    g_object_unref (self->core);

  G_OBJECT_CLASS (retro_demo_application_parent_class)->finalize (object);
}

static void
retro_demo_activate (GApplication *application)
{
  RetroDemoApplication *self;
  GtkWidget *window;
  RetroCoreView *view;
  g_autoptr (RetroScreen) screen;
  GdkRectangle view_size = (GdkRectangle) { 140, 60, 40, 120 };

  self = RETRO_DEMO_APPLICATION (application);

  g_signal_connect (self->core, "log", (GCallback) retro_g_log, NULL);

  screen = retro_screen_new ();
  retro_screen_set_view (screen, &view_size);

  view = retro_core_view_new ();
  retro_core_view_set_core (view, self->core);
  retro_core_view_set_screen (view, screen);
  /* retro_core_view_set_filter (view, RETRO_VIDEO_FILTER_CRT); */
  retro_core_view_set_as_default_controller (view, self->core);

  retro_core_set_keyboard (self->core, GTK_WIDGET (view));

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), 640, 480);
  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (view));

  gtk_widget_show_all (GTK_WIDGET (window));
  gtk_application_add_window (GTK_APPLICATION (application),
                              GTK_WINDOW (window));

  retro_core_run (self->core);
}

static void
retro_demo_application_class_init (RetroDemoApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  object_class->finalize = retro_demo_application_finalize;

  application_class->activate = retro_demo_activate;
  application_class->open = retro_demo_open;
}

static void
retro_demo_application_init (RetroDemoApplication *self)
{
  g_application_set_application_id (G_APPLICATION (self), "org.gnome.Retro.Demo");
  g_application_set_flags (G_APPLICATION (self), G_APPLICATION_HANDLES_OPEN);
}

gint
main (gint   argc,
      gchar *argv[])
{
  RetroDemoApplication *app;
  int status;

  g_set_prgname ("retro-demo");
  g_set_application_name ("org.gnome.Retro.Demo");

  app = retro_demo_application_new();
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
