// This file is part of retro-gtk. License: GPL-3.0+.

#include <linux/input-event-codes.h>
#include "retro-cairo-display.h"
#include "retro-core-view.h"
#include "retro-core-view-input-device.h"
#include "retro-joypad-id.h"
#include "retro-mouse-id.h"
#include "retro-pa-player.h"
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

struct _RetroCoreView
{
  GtkEventBox parent_instance;
  RetroCore *core;
  RetroCairoDisplay *display;
  GBinding *pixbuf_binding;
  GBinding *sensitive_binding;
  GdkPixbuf *pixbuf;
  RetroPaPlayer *audio_player;
  gboolean can_grab_pointer;
  gboolean snap_pointer_to_borders;
  GHashTable *key_state;
  GHashTable *mouse_button_state;
  GdkScreen *grabbed_screen;
  GdkDevice *grabbed_device;
  gdouble mouse_x_delta;
  gdouble mouse_y_delta;
  gint screen_center_x;
  gint screen_center_y;
  gint position_on_grab_x;
  gint position_on_grab_y;
  gboolean pointer_is_on_display;
  gdouble pointer_x;
  gdouble pointer_y;
};

G_DEFINE_TYPE (RetroCoreView, retro_core_view, GTK_TYPE_EVENT_BOX)

enum {
  PROP_PIXBUF = 1,
  PROP_CAN_GRAB_POINTER,
  PROP_SNAP_POINTER_TO_BORDERS,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

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

static gboolean
retro_core_view_get_is_pointer_grabbed (RetroCoreView *self)
{
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

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

  g_return_if_fail (RETRO_IS_CORE_VIEW (self));
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

static void
retro_core_view_ungrab (RetroCoreView *self)
{
  GdkSeat *seat;

  g_return_if_fail (RETRO_IS_CORE_VIEW (self));
  g_return_if_fail (self->grabbed_device != NULL);

  seat = gdk_device_get_seat (self->grabbed_device);
  gdk_seat_ungrab (seat);
  gdk_device_warp (self->grabbed_device, self->grabbed_screen,
                   self->position_on_grab_x, self->position_on_grab_y);

  g_clear_object (&self->grabbed_device);
  g_clear_object (&self->grabbed_screen);
}

static gboolean
retro_core_view_on_key_press_event (GtkWidget   *source,
                                    GdkEventKey *event,
                                    gpointer     data)
{
  RetroCoreView *self = RETRO_CORE_VIEW (data);

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (event->keyval == GDK_KEY_Escape &&
      (event->state & GDK_CONTROL_MASK) &&
      retro_core_view_get_is_pointer_grabbed (self))
    retro_core_view_ungrab (self);

  set_input_pressed (self->key_state, event->hardware_keycode);

  return FALSE;
}

static gboolean
retro_core_view_on_key_release_event (GtkWidget   *source,
                                      GdkEventKey *event,
                                      gpointer     data)
{
  RetroCoreView *self = RETRO_CORE_VIEW (data);

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  set_input_released (self->key_state, event->hardware_keycode);

  return FALSE;
}

static gboolean
retro_core_view_on_button_press_event (GtkWidget      *source,
                                       GdkEventButton *event,
                                       gpointer        data)
{
  RetroCoreView *self = RETRO_CORE_VIEW (data);

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);
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

static gboolean
retro_core_view_on_button_release_event (GtkWidget      *source,
                                         GdkEventButton *event,
                                         gpointer        data)
{
  RetroCoreView *self = RETRO_CORE_VIEW (data);

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  set_input_released (self->mouse_button_state, event->button);

  return FALSE;
}

static gboolean
retro_core_view_on_focus_out_event (GtkWidget     *source,
                                    GdkEventFocus *event,
                                    gpointer       data)
{
  RetroCoreView *self = RETRO_CORE_VIEW (data);

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (retro_core_view_get_is_pointer_grabbed (self))
    retro_core_view_ungrab (self);

  reset_input (self->key_state);
  reset_input (self->mouse_button_state);

  return FALSE;
}

static gboolean
retro_core_view_on_motion_notify_event (GtkWidget      *source,
                                        GdkEventMotion *event,
                                        gpointer        data)
{
  RetroCoreView *self = RETRO_CORE_VIEW (data);

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);
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
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

  return get_input_state (self->key_state, hardware_keycode);
}

static gboolean
retro_core_view_get_joypad_button_state (RetroCoreView *self,
                                         RetroJoypadId  button)
{
  guint16 hardware_keycode;

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

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
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

  return get_input_state (self->mouse_button_state, button);
}

static void
retro_core_view_finalize (GObject *object)
{
  RetroCoreView *self = RETRO_CORE_VIEW (object);

  g_clear_object (&self->core);
  g_object_unref (self->display);
  g_object_unref (self->pixbuf_binding);
  g_object_unref (self->sensitive_binding);
  g_clear_object (&self->pixbuf);
  g_object_unref (self->audio_player);
  g_hash_table_unref (self->key_state);
  g_hash_table_unref (self->mouse_button_state);
  g_clear_object (&self->grabbed_screen);
  g_clear_object (&self->grabbed_device);

  G_OBJECT_CLASS (retro_core_view_parent_class)->finalize (object);
}

static void
retro_core_view_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  RetroCoreView *self = RETRO_CORE_VIEW (object);

  switch (prop_id) {
  case PROP_PIXBUF:
    g_value_set_object (value, retro_core_view_get_pixbuf (self));

    break;
  case PROP_CAN_GRAB_POINTER:
    g_value_set_boolean (value, retro_core_view_get_can_grab_pointer (self));

    break;
  case PROP_SNAP_POINTER_TO_BORDERS:
    g_value_set_boolean (value, retro_core_view_get_snap_pointer_to_borders (self));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_core_view_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  RetroCoreView *self = RETRO_CORE_VIEW (object);

  switch (prop_id) {
  case PROP_PIXBUF:
    retro_core_view_set_pixbuf (self, g_value_get_object (value));

    break;
  case PROP_CAN_GRAB_POINTER:
    retro_core_view_set_can_grab_pointer (self, g_value_get_boolean (value));

    break;
  case PROP_SNAP_POINTER_TO_BORDERS:
    retro_core_view_set_snap_pointer_to_borders (self, g_value_get_boolean (value));

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

    break;
  }
}

static void
retro_core_view_class_init (RetroCoreViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = retro_core_view_finalize;
  object_class->get_property = retro_core_view_get_property;
  object_class->set_property = retro_core_view_set_property;

  properties[PROP_PIXBUF] =
    g_param_spec_object ("pixbuf",
                         "Pixbuf",
                         "The displayed pixbuf",
                         gdk_pixbuf_get_type (),
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_NAME |
                         G_PARAM_STATIC_NICK |
                         G_PARAM_STATIC_BLURB);

  properties[PROP_CAN_GRAB_POINTER] =
    g_param_spec_boolean ("can-grab-pointer",
                          "Can grab pointer",
                          "Whether the pointer can be grabbed",
                          FALSE,
                          G_PARAM_READWRITE |
                          G_PARAM_STATIC_NAME |
                          G_PARAM_STATIC_NICK |
                          G_PARAM_STATIC_BLURB);

  properties[PROP_SNAP_POINTER_TO_BORDERS] =
    g_param_spec_boolean ("snap-pointer-to-borders",
                          "Snap pointer to borders",
                          "Wheter the pointer is considered to be on the border when it is out of the display",
                          FALSE,
                          G_PARAM_READWRITE |
                          G_PARAM_STATIC_NAME |
                          G_PARAM_STATIC_NICK |
                          G_PARAM_STATIC_BLURB);

  g_object_class_install_properties (object_class,
                                     N_PROPS,
                                     properties);
}

RetroCoreView* retro_core_view_construct (GType object_type) {
  RetroCoreView * self = NULL;
  self = (RetroCoreView*) g_object_new (object_type, NULL);
  return self;
}

static void
retro_core_view_init (RetroCoreView *self)
{
  g_object_set ((GtkWidget*) self, "can-focus", TRUE, NULL);

  self->display = g_object_ref_sink (retro_cairo_display_new ());
  gtk_widget_set_visible (GTK_WIDGET (self->display), TRUE);
  g_object_set (GTK_WIDGET (self->display), "can-focus", FALSE, NULL);
  gtk_container_add (GTK_CONTAINER (self), GTK_WIDGET (self->display));

  self->pixbuf_binding =
    g_object_bind_property (G_OBJECT (self->display), "pixbuf",
                            G_OBJECT (self), "pixbuf",
                            G_BINDING_BIDIRECTIONAL |
                            G_BINDING_SYNC_CREATE);
  self->sensitive_binding =
    g_object_bind_property (G_OBJECT (self), "sensitive",
                            G_OBJECT (self->display), "sensitive",
                            G_BINDING_BIDIRECTIONAL |
                            G_BINDING_SYNC_CREATE);

  self->audio_player = retro_pa_player_new ();

  self->key_state = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);
  self->mouse_button_state = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);

  g_signal_connect_object (self, "key-press-event", (GCallback) retro_core_view_on_key_press_event, self, 0);
  g_signal_connect_object (self, "key-release-event", (GCallback) retro_core_view_on_key_release_event, self, 0);
  g_signal_connect_object (self, "button-press-event", (GCallback) retro_core_view_on_button_press_event, self, 0);
  g_signal_connect_object (self, "button-release-event", (GCallback) retro_core_view_on_button_release_event, self, 0);
  g_signal_connect_object (self, "focus-out-event", (GCallback) retro_core_view_on_focus_out_event, self, 0);
  g_signal_connect_object (self, "motion-notify-event", (GCallback) retro_core_view_on_motion_notify_event, self, 0);
}

/* Public */

/**
 * retro_core_view_set_core:
 * @self: a #RetroCoreView
 * @core: (nullable): a #RetroCore, or %NULL
 *
 * Sets @core as the #RetroCore handled by @self.
 */
void
retro_core_view_set_core (RetroCoreView *self,
                          RetroCore *core)
{
  g_return_if_fail (RETRO_IS_CORE_VIEW (self));

  if (self->core == core)
    return;

  if (self->core != NULL) {
    g_clear_object (&self->core);
    retro_cairo_display_set_core (self->display, NULL);
    retro_pa_player_set_core (self->audio_player, NULL);
  }

  if (core != NULL) {
    self->core = g_object_ref (core);
    retro_cairo_display_set_core (self->display, core);
    retro_pa_player_set_core (self->audio_player, core);
  }
}

/**
 * retro_core_view_set_pixbuf:
 * @self: a #RetroCoreView
 * @pixbuf: a #GdkPixbuf
 *
 * Sets @pixbuf as the currently displayed video frame.
 */
void
retro_core_view_set_pixbuf (RetroCoreView *self,
                            GdkPixbuf     *pixbuf)
{
  g_return_if_fail (RETRO_IS_CORE_VIEW (self));
  g_return_if_fail (pixbuf == NULL || GDK_IS_PIXBUF (pixbuf));

  if (g_set_object (&self->pixbuf, pixbuf))
    g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_PIXBUF]);
}

/**
 * retro_core_view_get_pixbuf:
 * @self: a #RetroCoreView
 *
 * Gets the currently displayed video frame.
 *
 * Returns: (transfer none): a #GdkPixbuf
 */
GdkPixbuf *
retro_core_view_get_pixbuf (RetroCoreView *self)
{
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), NULL);

  return self->pixbuf;
}

/**
 * retro_core_view_set_filter:
 * @self: a #RetroCoreView
 * @filter: a #RetroVideoFilter
 *
 * Sets the video filter to use to render the core's video on @self.
 */
void
retro_core_view_set_filter (RetroCoreView    *self,
                            RetroVideoFilter  filter)
{
  g_return_if_fail (RETRO_IS_CORE_VIEW (self));

  retro_cairo_display_set_filter (self->display, filter);
}

/**
 * retro_core_view_show_video:
 * @self: a #RetroCoreView
 *
 * Shows the video display of the core.
 */
void
retro_core_view_show_video (RetroCoreView *self)
{
  g_return_if_fail (RETRO_IS_CORE_VIEW (self));

  retro_cairo_display_show_video (self->display);
}

/**
 * retro_core_view_hide_video:
 * @self: a #RetroCoreView
 *
 * Hides the video display of the core.
 */
void
retro_core_view_hide_video (RetroCoreView *self)
{
  g_return_if_fail (RETRO_IS_CORE_VIEW (self));

  retro_cairo_display_hide_video (self->display);
}

/**
 * retro_core_view_as_input_device:
 * @self: a #RetroCoreView
 * @device_type: the controller type to expose @self as
 *
 * Creates a new #RetroInputDevice exposing @self as the specified controller
 * type. The valid controller types are RETRO_DEVICE_TYPE_JOYPAD,
 * RETRO_DEVICE_TYPE_MOUSE and RETRO_DEVICE_TYPE_POINTER.
 *
 * Returns: (transfer full): a new #RetroInputDevice
 */
RetroInputDevice *
retro_core_view_as_input_device (RetroCoreView   *self,
                                 RetroDeviceType  device_type)
{
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), NULL);

  return RETRO_INPUT_DEVICE (retro_core_view_input_device_new (self, device_type));
}

/**
 * retro_core_view_get_input_state:
 * @self: a #RetroCoreView
 * @device: an #RetroDeviceType to query @self
 * @index: an input index to interpret depending on @device
 * @id: an input id to interpret depending on @device
 *
 * Gets the state of an input of @self.
 *
 * Returns: the input's state
 */
gint16
retro_core_view_get_input_state (RetroCoreView   *self,
                                 RetroDeviceType  device,
                                 guint            index,
                                 guint            id)
{
  gint16 result;

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), 0);

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

/**
 * retro_core_view_get_device_capabilities:
 * @self: a #RetroCoreView
 *
 * Gets a flag representing the capabilities of @self when exposed as a
 * controller. See retro_input_device_get_device_capabilities() for more
 * information on the flag.
 *
 * See retro_core_view_as_input_device() to know the capabilities of
 * #RetroCoreView when exposed as a controller.
 *
 * Returns: the capabilities flag of @self when exposed as a controller
 */
guint64
retro_core_view_get_device_capabilities (RetroCoreView *self)
{
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), 0);

  return 1 << RETRO_DEVICE_TYPE_JOYPAD |
         1 << RETRO_DEVICE_TYPE_MOUSE |
         1 << RETRO_DEVICE_TYPE_POINTER;
}

/**
 * retro_core_view_get_can_grab_pointer:
 * @self: a #RetroCoreView
 *
 * Gets whether the pointer should be grabbed when clicking on the view. This
 * allows @self to work as a RETRO_DEVICE_TYPE_MOUSE instead of a
 * RETRO_DEVICE_TYPE_POINTER.
 *
 * Returns: whether the pointer should snap to the borders
 */
gboolean
retro_core_view_get_can_grab_pointer (RetroCoreView *self)
{
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

  return self->can_grab_pointer;
}

/**
 * retro_core_view_set_can_grab_pointer:
 * @self: a #RetroCoreView
 * @can_grab_pointer:  whether the pointer should be grabbed when clicking on
 * the view
 *
 * Sets whether the pointer should be grabbed when clicking on the view. This
 * allows @self to work as a RETRO_DEVICE_TYPE_MOUSE instead of a
 * RETRO_DEVICE_TYPE_POINTER.
 */
void
retro_core_view_set_can_grab_pointer (RetroCoreView *self,
                                      gboolean can_grab_pointer)
{
  g_return_if_fail (RETRO_IS_CORE_VIEW (self));

  if (self->can_grab_pointer == can_grab_pointer)
    return;

  self->can_grab_pointer = can_grab_pointer;

  if (can_grab_pointer == FALSE &&
      retro_core_view_get_is_pointer_grabbed (self))
    retro_core_view_ungrab (self);

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_CAN_GRAB_POINTER]);
}

/**
 * retro_core_view_get_snap_pointer_to_borders:
 * @self: a #RetroCoreView
 *
 * Gets whether the pointer should be considered to be at the border of the
 * video display when it is outside of it. This is used when @self is exposed as
 * a RETRO_DEVICE_TYPE_POINTER.
 *
 * Returns: whether the pointer should snap to the borders
 */
gboolean
retro_core_view_get_snap_pointer_to_borders (RetroCoreView *self)
{
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

  return self->snap_pointer_to_borders;
}

/**
 * retro_core_view_set_snap_pointer_to_borders:
 * @self: a #RetroCoreView
 * @snap_pointer_to_borders: whether the pointer should snap to the borders
 *
 * Sets whether the pointer should be considered to be at the border of the
 * video display when it is outside of it. This is used when @self is exposed as
 * a RETRO_DEVICE_TYPE_POINTER.
 */
void
retro_core_view_set_snap_pointer_to_borders (RetroCoreView *self,
                                             gboolean snap_pointer_to_borders)
{
  g_return_if_fail (RETRO_IS_CORE_VIEW (self));

  if (self->snap_pointer_to_borders == snap_pointer_to_borders)
    return;

  self->snap_pointer_to_borders = snap_pointer_to_borders;

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SNAP_POINTER_TO_BORDERS]);
}

/**
 * retro_core_view_new:
 *
 * Creates a new #RetroCoreView.
 *
 * Returns: (transfer full): a new #RetroCoreView
 */
RetroCoreView *
retro_core_view_new (void)
{
  return g_object_new (RETRO_TYPE_CORE_VIEW, NULL);
}
