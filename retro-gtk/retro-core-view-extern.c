// This file is part of retro-gtk. License: GPL-3.0+.

#include <linux/input-event-codes.h>
#include "retro-gtk-internal.h"
#include "retro-core-view-input-device.h"
#include "retro-joypad-id.h"
#include "retro-mouse-id.h"
#include "retro-pointer-id.h"

static guint16 DEFAULT_KEY_JOYPAD_BUTTON_MAPPING[RETRO_JOYPAD_ID_COUNT] = {
  KEY_S,
  KEY_A,
  KEY_BACKSPACE,
  KEY_ENTER,
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_D,
  KEY_W,
  KEY_Q,
  KEY_E,
  KEY_Z,
  KEY_C,
  KEY_1,
  KEY_3,
};

/* Private */

static void
set_input_pressed (GHashTable *table,
                   guint       input)
{
  static gboolean value = TRUE;

  if (g_hash_table_contains (table, &input))
    g_hash_table_replace (table,
                          g_memdup (&input, sizeof (guint)),
                          g_memdup (&value, sizeof (gboolean)));
  else
    g_hash_table_insert (table,
                         g_memdup (&input, sizeof (guint)),
                         g_memdup (&value, sizeof (gboolean)));
}

static void
set_input_released (GHashTable *table,
                    guint       input)
{
  g_hash_table_remove (table, &input);
}

static void
reset_input (GHashTable *table)
{
  g_hash_table_remove_all (table);
}

static gboolean
get_input_state (GHashTable *table,
                 guint       input)
{
  return g_hash_table_contains (table, &input);
}

static gint16
axis_to_retro_axis (gdouble value)
{
  if (value <= -1.0)
    return -G_MAXINT16;

  if (value >= 1.0)
    return G_MAXINT16;

  return (gint16) (value * G_MAXINT16);
}

static void
recenter_pointer (RetroCoreView *self)
{
  gdk_device_warp (self->grabbed_device, self->grabbed_screen,
                   self->screen_center_x, self->screen_center_y);
}

// FIXME Make static as soon as possible.
gboolean
retro_core_view_get_is_pointer_grabbed (RetroCoreView *self)
{
  g_return_val_if_fail (self != NULL, FALSE);

  return self->grabbed_device != NULL;
}

static void
retro_core_view_grab (RetroCoreView *self,
                      GdkDevice     *device,
                      GdkWindow     *window,
                      GdkEvent      *event)
{
  GdkSeat *seat;
  GdkDisplay *display;
  GdkCursor *cursor;
  GdkScreen *screen = NULL;
  GdkMonitor *monitor;
  GdkRectangle monitor_geometry;

  g_return_if_fail (self != NULL);
  g_return_if_fail (device != NULL);
  g_return_if_fail (window != NULL);
  g_return_if_fail (event != NULL);

  if (self->grabbed_device != NULL)
    g_object_unref (self->grabbed_device);

  if (self->grabbed_screen != NULL)
    g_object_unref (self->grabbed_screen);

  self->grabbed_device = g_object_ref (device);
  seat = gdk_device_get_seat (device);
  display = gdk_device_get_display (device);
  cursor = gdk_cursor_new_for_display (display, GDK_BLANK_CURSOR);
  gdk_seat_grab (seat, window, GDK_SEAT_CAPABILITY_ALL_POINTING, FALSE, cursor, event, NULL, NULL);
  monitor = gdk_display_get_monitor_at_window (display, window);
  gdk_monitor_get_geometry (monitor, &monitor_geometry);

  gdk_device_get_position (device, &screen, &self->position_on_grab_x, &self->position_on_grab_y);
  self->grabbed_screen = g_object_ref (screen);
  self->screen_center_x = monitor_geometry.x + monitor_geometry.width / 2;
  self->screen_center_y = monitor_geometry.y + monitor_geometry.height / 2;
  self->mouse_x_delta = 0;
  self->mouse_y_delta = 0;

  recenter_pointer (self);

  g_object_unref (cursor);
}

// FIXME Make static as soon as possible.
void
retro_core_view_ungrab (RetroCoreView *self)
{
  GdkSeat *seat;

  g_return_if_fail (self != NULL);
  g_return_if_fail (self->grabbed_device != NULL);

  seat = gdk_device_get_seat (self->grabbed_device);
  gdk_seat_ungrab (seat);
  gdk_device_warp (self->grabbed_device, self->grabbed_screen,
                   self->position_on_grab_x, self->position_on_grab_y);

  g_clear_object (&self->grabbed_device);
  g_clear_object (&self->grabbed_screen);
}

// FIXME Make static as soon as possible.
gboolean
retro_core_view_on_key_press_event (RetroCoreView *self,
                                    GtkWidget     *source,
                                    GdkEventKey   *event)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (event->keyval == GDK_KEY_Escape &&
      (event->state & GDK_CONTROL_MASK) &&
      retro_core_view_get_is_pointer_grabbed (self))
    retro_core_view_ungrab (self);

  set_input_pressed (self->key_state, event->hardware_keycode);

  return FALSE;
}

// FIXME Make static as soon as possible.
gboolean
retro_core_view_on_key_release_event (RetroCoreView *self,
                                      GtkWidget     *source,
                                      GdkEventKey   *event)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  set_input_released (self->key_state, event->hardware_keycode);

  return FALSE;
}

// FIXME Make static as soon as possible.
gboolean
retro_core_view_on_button_press_event (RetroCoreView  *self,
                                       GtkWidget      *source,
                                       GdkEventButton *event)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (retro_core_view_get_can_grab_pointer (self)) {
    if (retro_core_view_get_is_pointer_grabbed (self))
      set_input_pressed (self->mouse_button_state, event->button);
    else
      retro_core_view_grab (self,
                            event->device,
                            event->window,
                            (GdkEvent *) event);
  }
  else {
    set_input_pressed (self->mouse_button_state, event->button);
    self->pointer_is_on_display =
      retro_cairo_display_get_coordinates_on_display (self->display,
                                                      event->x,
                                                      event->y,
                                                      &self->pointer_x,
                                                      &self->pointer_y);
  }

  return FALSE;
}

// FIXME Make static as soon as possible.
gboolean
retro_core_view_on_button_release_event (RetroCoreView  *self,
                                         GtkWidget      *source,
                                         GdkEventButton *event)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  set_input_released (self->mouse_button_state, event->button);

  return FALSE;
}

// FIXME Make static as soon as possible.
gboolean
retro_core_view_on_focus_out_event (RetroCoreView *self,
                                    GtkWidget     *source,
                                    GdkEventFocus *event)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (retro_core_view_get_is_pointer_grabbed (self))
    retro_core_view_ungrab (self);

  reset_input (self->key_state);
  reset_input (self->mouse_button_state);

  return FALSE;
}

// FIXME Make static as soon as possible.
gboolean
retro_core_view_on_motion_notify_event (RetroCoreView  *self,
                                        GtkWidget      *source,
                                        GdkEventMotion *event)
{
  g_return_val_if_fail (self != NULL, FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (retro_core_view_get_can_grab_pointer (self)) {
    if (retro_core_view_get_is_pointer_grabbed (self) &&
        event->device == self->grabbed_device) {
      self->mouse_x_delta += event->x_root - (double) self->screen_center_x;
      self->mouse_y_delta += event->y_root - (double) self->screen_center_y;

      recenter_pointer (self);
    }
  }
  else {
    self->pointer_is_on_display =
      retro_cairo_display_get_coordinates_on_display (self->display,
                                                      event->x,
                                                      event->y,
                                                      &self->pointer_x,
                                                      &self->pointer_y);

  }

  return FALSE;
}

static gboolean
retro_core_view_get_key_state (RetroCoreView *self,
                               guint16        hardware_keycode)
{
  g_return_val_if_fail (self != NULL, FALSE);

  return get_input_state (self->key_state, hardware_keycode);
}

static gboolean
retro_core_view_get_joypad_button_state (RetroCoreView *self,
                                         RetroJoypadId  button)
{
  guint16 hardware_keycode;

  g_return_val_if_fail (self != NULL, FALSE);

  if (button >= RETRO_JOYPAD_ID_COUNT)
    return 0;

  // GDK adds 8 to the Linux input event codes to create the hardware keycode.
  hardware_keycode = DEFAULT_KEY_JOYPAD_BUTTON_MAPPING[button] + 8;

  return retro_core_view_get_key_state (self, hardware_keycode);
}

static gboolean
retro_core_view_get_mouse_button_state (RetroCoreView *self,
                                        guint16        button)
{
  g_return_val_if_fail (self != NULL, FALSE);

  return get_input_state (self->mouse_button_state, button);
}

// FIXME Make static as soon as possible.
gint16
retro_core_view_get_input_state (RetroCoreView   *self,
                                 RetroDeviceType  device,
                                 guint            index,
                                 guint            id)
{
  gint16 result;

  g_return_val_if_fail (self != NULL, 0);

  switch (device) {
  case RETRO_DEVICE_TYPE_JOYPAD:
    if (id >= RETRO_JOYPAD_ID_COUNT)
      return 0;

    return retro_core_view_get_joypad_button_state (self, id) ? G_MAXINT16 : 0;
  case RETRO_DEVICE_TYPE_MOUSE:
    switch (id) {
    case RETRO_MOUSE_ID_X:
      result = (gint16) self->mouse_x_delta;
      self->mouse_x_delta = 0;

      return result;
    case RETRO_MOUSE_ID_Y:
      result = self->mouse_y_delta;
      self->mouse_y_delta = 0;

      return result;
    case RETRO_MOUSE_ID_LEFT:
      return retro_core_view_get_mouse_button_state (self, 1) ? G_MAXINT16 : 0;
    case RETRO_MOUSE_ID_RIGHT:
      return retro_core_view_get_mouse_button_state (self, 3) ? G_MAXINT16 : 0;
    default:
      return 0;
    }
  case RETRO_DEVICE_TYPE_POINTER:
    switch (id) {
    case RETRO_POINTER_ID_X:
      return axis_to_retro_axis (self->pointer_x);
    case RETRO_POINTER_ID_Y:
      return axis_to_retro_axis (self->pointer_y);
    case RETRO_POINTER_ID_PRESSED:
      if (!self->pointer_is_on_display ||
          retro_core_view_get_snap_pointer_to_borders (self))
        return 0;

      return retro_core_view_get_mouse_button_state (self, 1) ? 1 : 0;
    default:
      return 0;
    }
  default:
    return 0;
  }
}

// FIXME Make static as soon as possible.
guint64
retro_core_view_get_device_capabilities (RetroCoreView *self)
{
  g_return_val_if_fail (self != NULL, 0);

  return 1 << RETRO_DEVICE_TYPE_JOYPAD |
         1 << RETRO_DEVICE_TYPE_MOUSE |
         1 << RETRO_DEVICE_TYPE_POINTER;
}

/* Public */

RetroInputDevice *
retro_core_view_as_input_device (RetroCoreView   *self,
                                 RetroDeviceType  device_type)
{
  g_return_val_if_fail (self != NULL, NULL);

  return RETRO_INPUT_DEVICE (retro_core_view_input_device_new (self, device_type));
}

void
retro_core_view_set_filter (RetroCoreView    *self,
                            RetroVideoFilter  filter)
{
  g_return_val_if_fail (self != NULL, NULL);

  retro_cairo_display_set_filter (self->display, filter);
}

void
retro_core_view_show_video (RetroCoreView *self)
{
  g_return_val_if_fail (self != NULL, NULL);

  retro_cairo_display_show_video (self->display);
}

void
retro_core_view_hide_video (RetroCoreView *self)
{
  g_return_val_if_fail (self != NULL, NULL);

  retro_cairo_display_hide_video (self->display);
}
