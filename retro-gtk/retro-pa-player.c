// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-pa-player.h"

#include <pulse/simple.h>
#include <pulse/pulseaudio.h>

struct _RetroPaPlayer
{
  GObject parent_instance;
  RetroCore *core;
  gulong on_audio_output_id;
  gdouble sample_rate;
  pa_simple *simple;
};

G_DEFINE_TYPE (RetroPaPlayer, retro_pa_player, G_TYPE_OBJECT)

/* Private */

static void
retro_pa_player_finalize (GObject *object)
{
  RetroPaPlayer *self = (RetroPaPlayer *)object;

  g_clear_object (&self->core);

  if (self->simple != NULL) {
    pa_simple_free (self->simple);
    self->simple = NULL;
  }

  G_OBJECT_CLASS (retro_pa_player_parent_class)->finalize (object);
}

static void
retro_pa_player_class_init (RetroPaPlayerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_pa_player_finalize;
}

static void
retro_pa_player_init (RetroPaPlayer *self)
{
}

static void
retro_pa_player_prepare_for_sample_rate (RetroPaPlayer *self,
                                         gdouble        sample_rate)
{
  pa_sample_spec sample_spec = {0};
  gint error;

  g_return_if_fail (RETRO_IS_PA_PLAYER (self));

  self->sample_rate = sample_rate;

  pa_sample_spec_init (&sample_spec);
  sample_spec.format = PA_SAMPLE_S16NE;
  sample_spec.rate = (guint32) sample_rate;
  sample_spec.channels = 2;

  if (self->simple != NULL)
    pa_simple_free (self->simple);

  self->simple = pa_simple_new (NULL, NULL, PA_STREAM_PLAYBACK, NULL, "",
                                &sample_spec, NULL, NULL, &error);
  if (!self->simple) {
      g_critical ("pa_simple_new() failed: %s", pa_strerror(error));
  }
}

static void
retro_pa_player_on_audio_output (RetroCore *sender,
                                 gint16    *data,
                                 int        length,
                                 gdouble    sample_rate,
                                 gpointer   user_data)
{
  RetroPaPlayer *self = RETRO_PA_PLAYER (user_data);

  g_return_if_fail (RETRO_IS_PA_PLAYER (self));

  if (self->simple == NULL || sample_rate != self->sample_rate)
    retro_pa_player_prepare_for_sample_rate (self, sample_rate);

  if (self->simple == NULL)
    return;

  pa_simple_write (self->simple, data, sizeof (gint16) * length, NULL);
}

/* Public */

/**
 * retro_pa_player_set_core:
 * @self: a #RetroPaPlayer
 * @core: (nullable): a #RetroCore, or %NULL
 *
 * Sets @core as the #RetroCore played by @self.
 */
void
retro_pa_player_set_core (RetroPaPlayer *self,
                          RetroCore     *core)
{
  g_return_if_fail (RETRO_IS_PA_PLAYER (self));

  if (self->core == core)
    return;

  if (self->core != NULL) {
    g_signal_handler_disconnect (G_OBJECT (self->core),
                                 self->on_audio_output_id);
    g_clear_object (&self->core);
  }

  if (core != NULL) {
    self->core = g_object_ref (core);
    self->on_audio_output_id =
      g_signal_connect_object (core,
                               "audio-output",
                               (GCallback) retro_pa_player_on_audio_output,
                               self,
                               0);
  }

  if (self->simple != NULL) {
    pa_simple_free (self->simple);
    self->simple = NULL;
  }
}

/**
 * retro_pa_player_new:
 *
 * Creates a new #RetroPaPlayer.
 *
 * Returns: (transfer full): a new #RetroPaPlayer
 */
RetroPaPlayer *
retro_pa_player_new (void)
{
  return g_object_new (RETRO_TYPE_PA_PLAYER, NULL);
}
