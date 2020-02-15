// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-main-loop-source-private.h"

#include <math.h>

typedef struct {
  GSource parent;
  gdouble delay;
  gint64 next_time;
  gdouble rounding_error;
} RetroMainLoopSource;

static gboolean
retro_main_loop_source_prepare (GSource *source,
                                gint    *timeout)
{
  RetroMainLoopSource *self = (RetroMainLoopSource *) source;
  gint64 delay = self->next_time - g_get_monotonic_time ();

  *timeout = MAX (0, (gint) floor (delay / 1000.0) - 1);

  return delay <= 0;
}

static gboolean
retro_main_loop_source_dispatch (GSource     *source,
                                 GSourceFunc  callback,
                                 gpointer     user_data)
{
  RetroMainLoopSource *self = (RetroMainLoopSource *) source;
  gboolean result;
  gint64 time = g_source_get_time (source);
  gdouble delay;

  if (!callback)
    return G_SOURCE_REMOVE;

  result = callback (user_data);

  delay = round (self->delay);

  self->rounding_error += (delay - self->delay);
  if (self->rounding_error >= 1) {
    delay--;
    self->rounding_error--;
  } else if (self->rounding_error <= -1) {
    delay++;
    self->rounding_error++;
  }

  if (self->next_time == 0 || self->next_time + delay <= time)
    self->next_time = time;

  self->next_time += delay;
  g_source_set_ready_time (source, self->next_time);

  return result;
}

static GSourceFuncs retro_main_loop_source_funcs =
  {
    retro_main_loop_source_prepare,
    NULL, /* check */
    retro_main_loop_source_dispatch,
    NULL, /* finalize */
    NULL,
    NULL,
  };

GSource *
retro_main_loop_source_new (gdouble framerate)
{
  RetroMainLoopSource *self;
  GSource *source;

  source = g_source_new (&retro_main_loop_source_funcs,
                         sizeof (RetroMainLoopSource));
  self = (RetroMainLoopSource *) source;
  self->delay = 1000000.0 / framerate;
  g_source_set_priority (source, G_PRIORITY_DEFAULT_IDLE);
  g_source_set_ready_time (source, 0);
  g_source_set_name (source, "RetroMainLoopSource");

  return source;
}
