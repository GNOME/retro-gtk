// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-main-loop.h"

struct _RetroMainLoop
{
  GObject parent_instance;
  RetroCore *core;
  gdouble speed_rate;
  glong loop;
  gulong core_fps_id;
};

G_DEFINE_TYPE (RetroMainLoop, retro_main_loop, G_TYPE_OBJECT)

enum  {
  PROP_CORE = 1,
  PROP_SPEED_RATE,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

/* Private */

static void
retro_main_loop_finalize (GObject *object)
{
  RetroMainLoop *self = RETRO_MAIN_LOOP (object);

  retro_main_loop_stop (self);
  g_clear_object (&self->core);

  G_OBJECT_CLASS (retro_main_loop_parent_class)->finalize (object);
}

static void
retro_main_loop_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  RetroMainLoop *self = RETRO_MAIN_LOOP (object);

  switch (prop_id) {
  case PROP_CORE:
    g_value_set_object (value, retro_main_loop_get_core (self));

    break;
  case PROP_SPEED_RATE:
    g_value_set_double (value, retro_main_loop_get_speed_rate (self));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_main_loop_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  RetroMainLoop *self = RETRO_MAIN_LOOP (object);

  switch (prop_id) {
  case PROP_CORE:
    retro_main_loop_set_core (self, g_value_get_object (value));

    break;
  case PROP_SPEED_RATE:
    retro_main_loop_set_speed_rate (self, g_value_get_double (value));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_main_loop_class_init (RetroMainLoopClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_main_loop_finalize;
  object_class->get_property = retro_main_loop_get_property;
  object_class->set_property = retro_main_loop_set_property;

  properties[PROP_CORE] =
    g_param_spec_object ("core",
                         "Core",
                         "The core",
                         RETRO_TYPE_CORE,
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB |
                         G_PARAM_CONSTRUCT);
  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   PROP_CORE,
                                   properties[PROP_CORE]);

  properties[PROP_SPEED_RATE] =
    g_param_spec_double ("speed-rate",
                         "Speed rate",
                         "The speed ratio at wich the core will run",
                         -G_MAXDOUBLE, G_MAXDOUBLE, 1.0,
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB |
                         G_PARAM_CONSTRUCT);
  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   PROP_SPEED_RATE,
                                   properties[PROP_SPEED_RATE]);
}

static void
retro_main_loop_on_notification (GObject    *src,
                                 GParamSpec *param,
                                 gpointer    user_data)
{
  RetroMainLoop *self = RETRO_MAIN_LOOP (user_data);
  const gchar *param_name;

  g_return_if_fail (self != NULL);
  g_return_if_fail (src != NULL);
  g_return_if_fail (param != NULL);

  if (self->loop < 0)
    return;

  param_name = g_param_spec_get_name (param);
  if (g_strcmp0 (param_name, "speed-rate") != 0)
    return;

  retro_main_loop_stop (self);
  retro_main_loop_start (self);
}

static void
retro_main_loop_on_frames_per_second_changed (GObject    *sender,
                                              GParamSpec *pspec,
                                              gpointer    user_data)
{
  RetroMainLoop *self = RETRO_MAIN_LOOP (user_data);

  g_return_if_fail (self != NULL);

  if (self->loop < 0)
    return;

  retro_main_loop_stop (self);
  retro_main_loop_start (self);
}

static void
retro_main_loop_init (RetroMainLoop *self)
{
  self->loop = -1;

  g_signal_connect_object (G_OBJECT (self),
                           "notify",
                           (GCallback) retro_main_loop_on_notification,
                           self,
                           0);
}

static gboolean
retro_main_loop_run (RetroMainLoop *self)
{
  g_return_val_if_fail (self != NULL, FALSE);

  if (self->core == NULL || self->loop < 0)
    return FALSE;

  retro_core_run (self->core);

  return TRUE;
}

/* Public */

/**
 * retro_main_loop_get_speed_rate:
 * @self: a #RetroMainLoop
 *
 * Gets the speed rate at which to run the core.
 *
 * Returns: the speed rate
 */
gdouble
retro_main_loop_get_speed_rate (RetroMainLoop *self)
{
  g_return_val_if_fail (self != NULL, 1.0);

  return self->speed_rate;
}

/**
 * retro_main_loop_get_core:
 * @self: a #RetroMainLoop
 *
 * Gets the core handled by @self.
 *
 * Returns: (transfer none): a #RetroCore
 */
RetroCore *
retro_main_loop_get_core (RetroMainLoop *self)
{
  g_return_val_if_fail (self != NULL, NULL);

  return self->core;
}

/**
 * retro_main_loop_set_core:
 * @self: a #RetroMainLoop
 * @core: (nullable): a #RetroCore, or %NULL
 *
 * Sets @core as the #RetroCore handled by @self.
 */
void
retro_main_loop_set_core (RetroMainLoop *self,
                          RetroCore     *core)
{
  g_return_if_fail (self != NULL);

  if (self->core == core)
    return;

  retro_main_loop_stop (self);

  if (self->core != NULL) {
    g_signal_handler_disconnect (G_OBJECT (self->core), self->core_fps_id);
    g_clear_object (&self->core);
  }

  if (core != NULL) {
    self->core = g_object_ref (core);
    self->core_fps_id =
      g_signal_connect_object (G_OBJECT (self->core),
                               "notify::frames-per-second",
                               (GCallback) retro_main_loop_on_frames_per_second_changed,
                               self,
                               0);
  }

  g_object_notify_by_pspec ((GObject *) self, properties[PROP_CORE]);
}

/**
 * retro_main_loop_set_speed_rate:
 * @self: a #RetroMainLoop
 * @speed_rate: a speed rate
 *
 * Sets the speed rate at which to run the core.
 */
void
retro_main_loop_set_speed_rate (RetroMainLoop *self,
                                gdouble        speed_rate)
{
  g_return_if_fail (self != NULL);

  if (self->speed_rate == speed_rate)
    return;

  self->speed_rate = speed_rate;
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SPEED_RATE]);
}

/**
 * retro_main_loop_start:
 * @self: a #RetroMainLoop
 *
 * Starts runing the core. If the core was stopped, it will restart from this moment.
 */
void
retro_main_loop_start (RetroMainLoop *self)
{
  gdouble fps;

  g_return_if_fail (self != NULL);
  g_return_if_fail (self->core != NULL);

  if (self->loop >= 0 || self->speed_rate <= 0)
    return;

  // TODO What if fps <= 0?
  fps = retro_core_get_frames_per_second (self->core);
  /* Do not make the timeout source hold a reference on the RetroMainLoop, so
   * destroying the RetroMainLoop while it is still running will stop it instead
   * of leaking a reference.
   */
  self->loop = g_timeout_add_full (G_PRIORITY_LOW,
                                   (guint) (1000 / (fps * self->speed_rate)),
                                   (GSourceFunc) retro_main_loop_run,
                                   self,
                                   NULL);
}

/**
 * retro_main_loop_reset:
 * @self: a #RetroMainLoop
 *
 * Resets the core. Results depend on the core's implementation.
 */
void
retro_main_loop_reset (RetroMainLoop *self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (self->core != NULL);

  retro_core_reset (self->core);
}

/**
 * retro_main_loop_stop:
 * @self: a #RetroMainLoop
 *
 * Stops runing the core.
 */
void
retro_main_loop_stop (RetroMainLoop *self)
{
  g_return_if_fail (self != NULL);

  if (self->loop < 0)
    return;

  g_source_remove (self->loop);
  self->loop = -1;
}

/**
 * retro_main_loop_new:
 *
 * Creates a new #RetroMainLoop.
 *
 * Returns: (transfer full): a new #RetroMainLoop
 */
RetroMainLoop *
retro_main_loop_new (RetroCore *core)
{
  return g_object_new (RETRO_TYPE_MAIN_LOOP, "core", core, NULL);
}
