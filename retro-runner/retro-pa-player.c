// This file is part of retro-gtk. License: GPL-3.0+.

#ifdef PULSEAUDIO_ENABLED

#include "retro-pa-player-private.h"

#include "retro-core-private.h"
#include <pulse/simple.h>
#include <pulse/pulseaudio.h>
#include <samplerate.h>

struct _RetroPaPlayer
{
  GObject parent_instance;
  RetroCore *core;
  gulong on_audio_output_id;
  gulong on_iterated_id;
  GArray *buffer;
  gdouble sample_rate;
  pa_simple *simple;
  SRC_STATE *src;
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

  g_clear_pointer (&self->src, src_delete);
  g_array_unref (self->buffer);

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
  gint error;

  self->buffer = g_array_new (FALSE, FALSE, sizeof (gint16));
  self->src = src_new (SRC_SINC_BEST_QUALITY, 2, &error);

  if (!self->src)
    g_error ("Couldn't set up libsamplerate: %s", src_strerror (error));
}

static void
prepare_for_sample_rate (RetroPaPlayer *self,
                         gdouble        sample_rate)
{
  pa_sample_spec sample_spec = {0};
  gint error;

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
resample (RetroPaPlayer *self,
          gdouble        ratio)
{
  g_autofree gfloat *data_in = NULL;
  g_autofree gfloat *data_out = NULL;
  gint error;
  gsize frames_used_in, frames_used_out, length;

  length = self->buffer->len;

  data_in = g_new (gfloat, length);
  data_out = g_new (gfloat, length);

  src_short_to_float_array ((gint16 *) self->buffer->data,
                            data_in,
                            length);

  g_array_set_size (self->buffer, 0);

  frames_used_in = 0;
  frames_used_out = 0;
  while (frames_used_in < length / 2) {
    SRC_DATA data;

    data.data_in = data_in + frames_used_in * sizeof (gfloat);
    data.data_out = data_out;
    data.input_frames = length / 2 - frames_used_in;
    data.output_frames = length / 2;
    data.src_ratio = ratio;
    data.end_of_input = 0;

    error = src_process (self->src, &data);
    if (error) {
      g_critical ("Couldn't resample the audio: %s", src_strerror (error));

      return;
    }

    g_array_set_size (self->buffer,
                      frames_used_out + data.output_frames_gen * 2);
    src_float_to_short_array (data_out,
                              (gint16 *) (self->buffer->data +
                              frames_used_out * sizeof (gint16)),
                              data.output_frames_gen * 2);

    frames_used_in += data.input_frames_used * 2;
    frames_used_out += data.output_frames_gen * 2;
  }
}

static void
on_audio_output (RetroCore     *sender,
                 gint16        *data,
                 int            length,
                 gdouble        sample_rate,
                 RetroPaPlayer *self)
{
  g_array_append_vals (self->buffer, data, length);
}

static void
on_iterated (RetroCore     *core,
             RetroPaPlayer *self)
{
  gdouble sample_rate, speed_rate;

  if (retro_core_is_running_ahead (self->core))
    return;

  if (self->buffer->len == 0)
    return;

  sample_rate = retro_core_get_sample_rate (self->core);
  speed_rate = retro_core_get_speed_rate (self->core);

  // Libsamplerate cannot resample audio with rates outside this range
  // Since audio isn't going to be useful at these rates anyway, just bail.
  if (speed_rate < 1.0 / 256.0 || speed_rate > 256.0) {
    g_array_set_size (self->buffer, 0);

    g_debug ("Can’t resample the audio for speed rates lower than 1/256 or greater than 256. The audio won’t be played.");

    return;
  }

  if (self->simple == NULL || sample_rate != self->sample_rate)
    prepare_for_sample_rate (self, sample_rate);

  if (self->simple == NULL)
    return;

  resample (self, 1 / speed_rate);

  pa_simple_write (self->simple,
                   self->buffer->data,
                   self->buffer->len * sizeof (gint16),
                   NULL);

  g_array_set_size (self->buffer, 0);
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
    g_signal_handler_disconnect (G_OBJECT (self->core),
                                 self->on_iterated_id);
    g_clear_object (&self->core);
  }

  if (core != NULL) {
    self->core = g_object_ref (core);
    self->on_audio_output_id =
      g_signal_connect_object (core,
                               "audio-output",
                               (GCallback) on_audio_output,
                               self,
                               0);
    self->on_iterated_id =
      g_signal_connect_object (core,
                               "iterated",
                               (GCallback) on_iterated,
                               self,
                               0);
  }

  if (self->simple != NULL) {
    pa_simple_free (self->simple);
    self->simple = NULL;
  }

  src_reset (self->src);
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

#endif
