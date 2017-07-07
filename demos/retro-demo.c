// This file is part of retro-gtk. License: GPLv3

#include <retro-gtk/retro-gtk.h>

#define RETRO_TYPE_DEMO_APPLICATION (retro_demo_application_get_type())

G_DECLARE_FINAL_TYPE (RetroDemoApplication, retro_demo_application, RETRO, DEMO_APPLICATION, GtkApplication)

struct _RetroDemoApplication
{
  GtkApplication parent_instance;

  GtkApplicationWindow *window;

  RetroCore *core;
  RetroMainLoop *loop;
  RetroCoreView *view;
  RetroPaPlayer *pa_player;
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

  g_signal_emit_by_name (self->core, "init");

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

  if (self->loop != NULL)
    g_object_unref (self->loop);

  if (self->pa_player != NULL)
    g_object_unref (self->pa_player);

  G_OBJECT_CLASS (retro_demo_application_parent_class)->finalize (object);
}


static void
retro_demo_activate (GApplication *application)
{
  RetroDemoApplication *self;
  GtkWidget *window;

  self = RETRO_DEMO_APPLICATION (application);

  g_signal_connect (self->core, "log", (GCallback) retro_g_log, NULL);

  self->view = retro_core_view_new ();
  retro_core_view_set_core (self->view, self->core);

  self->pa_player = retro_pa_player_new ();
  retro_pa_player_set_core (self->pa_player, self->core);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), 640, 480);
  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (self->view));

  gtk_widget_show_all (GTK_WIDGET (window));
  gtk_application_add_window (GTK_APPLICATION (application), GTK_WINDOW (window));

  self->loop = retro_main_loop_new (self->core);
  retro_main_loop_start (self->loop);
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
