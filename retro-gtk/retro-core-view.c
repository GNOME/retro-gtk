// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-core-view.h"

#include "retro-cairo-display.h"
#include "retro-gl-display.h"
#include "retro-controller-codes.h"
#include "retro-core-view-controller.h"
#include "retro-input-private.h"
#include "retro-pa-player.h"

struct _RetroCoreView
{
  GtkBin parent_instance;
  RetroCore *core;
  RetroGLDisplay *display;
  GBinding *sensitive_binding;
  RetroPaPlayer *audio_player;
  gboolean can_grab_pointer;
  gboolean snap_pointer_to_borders;
  GHashTable *key_state;
  GHashTable *keyval_state;
  RetroKeyJoypadMapping *key_joypad_mapping;
  GHashTable *mouse_button_state;
  GtkEventController *key_controller;
  GtkEventController *motion_controller;
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

G_DEFINE_TYPE (RetroCoreView, retro_core_view, GTK_TYPE_BIN)

enum {
  PROP_CAN_GRAB_POINTER = 1,
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
retro_core_view_on_key_pressed (GtkEventControllerKey *controller,
                                guint                  keyval,
                                guint                  keycode,
                                GdkModifierType        state,
                                gpointer               data)
{
  RetroCoreView *self = RETRO_CORE_VIEW (data);

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

  if (keyval == GDK_KEY_Escape &&
      (state & GDK_CONTROL_MASK) &&
      retro_core_view_get_is_pointer_grabbed (self))
    retro_core_view_ungrab (self);

  set_input_pressed (self->key_state, keycode);
  set_input_pressed (self->keyval_state, keyval);

  return FALSE;
}

static gboolean
retro_core_view_on_key_released (GtkEventControllerKey *controller,
                                 guint                  keyval,
                                 guint                  keycode,
                                 GdkModifierType        state,
                                 gpointer               data)
{
  RetroCoreView *self = RETRO_CORE_VIEW (data);

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

  set_input_released (self->key_state, keycode);
  set_input_released (self->keyval_state, keyval);

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

  gtk_widget_grab_focus (GTK_WIDGET (source));

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
      retro_gl_display_get_coordinates_on_display (self->display,
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
retro_core_view_on_focus_out (GtkEventControllerKey *controller,
                              gpointer               data)
{
  RetroCoreView *self = RETRO_CORE_VIEW (data);

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

  if (retro_core_view_get_is_pointer_grabbed (self))
    retro_core_view_ungrab (self);

  reset_input (self->key_state);
  reset_input (self->mouse_button_state);

  return FALSE;
}

static gboolean
retro_core_view_on_motion (GtkEventControllerMotion *controller,
                           gdouble                   x,
                           gdouble                   y,
                           gpointer                  data)
{
  RetroCoreView *self = RETRO_CORE_VIEW (data);

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

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
      retro_gl_display_get_coordinates_on_display (self->display,
                                                   x,
                                                   y,
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

  hardware_keycode = retro_key_joypad_mapping_get_button_key (self->key_joypad_mapping, button);

  return retro_core_view_get_key_state (self, hardware_keycode);
}

static gboolean
retro_core_view_get_mouse_button_state (RetroCoreView *self,
                                        guint16        button)
{
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

  return get_input_state (self->mouse_button_state, button);
}

static gboolean
retro_core_view_get_keyboard_key_state (RetroCoreView *self,
                                        guint16        key)
{
  guint16 keyval;

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), FALSE);

  if (key >= RETRO_KEYBOARD_KEY_LAST)
    return FALSE;

  keyval = retro_keyboard_key_to_val_converter (key);

  return get_input_state (self->keyval_state, keyval);
}

static void
retro_core_view_finalize (GObject *object)
{
  RetroCoreView *self = RETRO_CORE_VIEW (object);

  g_clear_object (&self->core);
  g_object_unref (self->display);
  g_object_unref (self->sensitive_binding);
  g_object_unref (self->audio_player);
  g_object_unref (self->key_controller);
  g_object_unref (self->motion_controller);
  g_hash_table_unref (self->key_state);
  g_hash_table_unref (self->keyval_state);
  g_object_unref (self->key_joypad_mapping);
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
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = retro_core_view_finalize;
  object_class->get_property = retro_core_view_get_property;
  object_class->set_property = retro_core_view_set_property;

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

  gtk_widget_class_set_css_name (widget_class, "retrocoreview");
}

RetroCoreView* retro_core_view_construct (GType object_type) {
  RetroCoreView * self = NULL;
  self = (RetroCoreView*) g_object_new (object_type, NULL);
  return self;
}

static void
retro_core_view_init (RetroCoreView *self)
{
  g_object_set ((GtkWidget*) self, "can-default", TRUE, NULL);
  g_object_set ((GtkWidget*) self, "can-focus", TRUE, NULL);

  self->display = g_object_ref_sink (retro_gl_display_new ());
  gtk_widget_set_visible (GTK_WIDGET (self->display), TRUE);
  g_object_set (GTK_WIDGET (self->display), "can-focus", FALSE, NULL);
  gtk_container_add (GTK_CONTAINER (self), GTK_WIDGET (self->display));

  self->sensitive_binding =
    g_object_bind_property (G_OBJECT (self), "sensitive",
                            G_OBJECT (self->display), "sensitive",
                            G_BINDING_BIDIRECTIONAL |
                            G_BINDING_SYNC_CREATE);

  self->audio_player = retro_pa_player_new ();

  self->key_controller = gtk_event_controller_key_new ();
  self->motion_controller = gtk_event_controller_motion_new ();

  gtk_widget_add_controller (GTK_WIDGET (self), self->key_controller);
  gtk_widget_add_controller (GTK_WIDGET (self), self->motion_controller);

  self->key_state = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);
  self->keyval_state = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);
  self->key_joypad_mapping = retro_key_joypad_mapping_new_default ();
  self->mouse_button_state = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);

  g_signal_connect_object (self->key_controller, "key-pressed", (GCallback) retro_core_view_on_key_pressed, self, 0);
  g_signal_connect_object (self->key_controller, "key-released", (GCallback) retro_core_view_on_key_released, self, 0);
  g_signal_connect_object (self, "button-press-event", (GCallback) retro_core_view_on_button_press_event, self, 0);
  g_signal_connect_object (self, "button-release-event", (GCallback) retro_core_view_on_button_release_event, self, 0);
  g_signal_connect_object (self->key_controller, "focus-out", (GCallback) retro_core_view_on_focus_out, self, 0);
  g_signal_connect_object (self->motion_controller, "motion", (GCallback) retro_core_view_on_motion, self, 0);
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
    retro_gl_display_set_core (self->display, NULL);
    retro_pa_player_set_core (self->audio_player, NULL);
  }

  if (core != NULL) {
    self->core = g_object_ref (core);
    retro_gl_display_set_core (self->display, core);
    retro_pa_player_set_core (self->audio_player, core);
  }
}

/**
 * retro_core_view_set_pixbuf:
 * @self: a #RetroCoreView
 * @pixbuf: a #GdkPixbuf
 *
 * Sets @pixbuf as the currently displayed video frame.
 *
 * retro_pixbuf_set_aspect_ratio() can be used to specify the aspect ratio for
 * the pixbuf. Otherwise the core's aspect ratio will be used.
 */
void
retro_core_view_set_pixbuf (RetroCoreView *self,
                            GdkPixbuf     *pixbuf)
{
  g_return_if_fail (RETRO_IS_CORE_VIEW (self));
  g_return_if_fail (pixbuf == NULL || GDK_IS_PIXBUF (pixbuf));

  retro_gl_display_set_pixbuf (self->display, pixbuf);
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

  return retro_gl_display_get_pixbuf (self->display);
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

  retro_gl_display_set_filter (self->display, filter);
}

/**
 * retro_core_view_set_key_joypad_mapping:
 * @self: a #RetroCoreView
 * @mapping: (nullable): a #RetroKeyJoypadMapping
 *
 * Sets the key joypad mapping on @self. If given mapping is %NULL, then set to
 * an empty configuration.
 */
void
retro_core_view_set_key_joypad_mapping (RetroCoreView         *self,
                                        RetroKeyJoypadMapping *mapping)
{
  g_return_if_fail (RETRO_IS_CORE_VIEW (self));
  g_return_if_fail (mapping == NULL || RETRO_IS_KEY_JOYPAD_MAPPING (mapping));

  g_object_unref (self->key_joypad_mapping);
  if (mapping != NULL)
    self->key_joypad_mapping = g_object_ref (mapping);
  else
    self->key_joypad_mapping = retro_key_joypad_mapping_new ();
}

/**
 * retro_core_view_get_key_joypad_mapping:
 * @self: a #RetroCoreView
 *
 * Gets the currently set key joypad mapping.
 *
 * Returns: (transfer none): a #RetroKeyJoypadMapping
 */
RetroKeyJoypadMapping *
retro_core_view_get_key_joypad_mapping (RetroCoreView *self)
{
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), NULL);

  return g_object_ref (self->key_joypad_mapping);
}

/**
 * retro_core_view_as_controller:
 * @self: a #RetroCoreView
 * @controller_type: the controller type to expose @self as
 *
 * Creates a new #RetroController exposing @self as the specified controller
 * type. The valid controller types are RETRO_CONTROLLER_TYPE_JOYPAD,
 * RETRO_CONTROLLER_TYPE_MOUSE, RETRO_CONTROLLER_TYPE_KEYBOARD and
 * RETRO_CONTROLLER_TYPE_POINTER.
 *
 * Returns: (transfer full): a new #RetroController
 */
RetroController *
retro_core_view_as_controller (RetroCoreView       *self,
                               RetroControllerType  controller_type)
{
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), NULL);

  return RETRO_CONTROLLER (retro_core_view_controller_new (self, controller_type));
}

/**
 * retro_core_view_set_as_default_controller:
 * @self: a #RetroCoreView
 * @core: (nullable): a #RetroCore
 *
 * Sets the @self as the default controllers of @core.
 */
void
retro_core_view_set_as_default_controller (RetroCoreView *self,
                                           RetroCore     *core)
{
  RetroControllerType type;
  RetroController *controller;
  guint64 capabilities;

  g_return_if_fail (RETRO_IS_CORE_VIEW (self));
  g_return_if_fail (RETRO_IS_CORE (core));

  capabilities = retro_core_view_get_controller_capabilities (self);

  for (type = RETRO_CONTROLLER_TYPE_NONE;
       type < RETRO_CONTROLLER_TYPE_COUNT;
       type++) {
    if ((capabilities & (1 << type)) == 0)
      continue;

    controller = retro_core_view_as_controller (self, type);
    retro_core_set_default_controller (core, type, controller);
    g_object_unref (controller);
  }
}

/**
 * retro_core_view_get_input_state:
 * @self: a #RetroCoreView
 * @input: a #RetroInput to query @self
 *
 * Gets the state of an input of @self.
 *
 * Returns: the input's state
 */
gint16
retro_core_view_get_input_state (RetroCoreView *self,
                                 RetroInput    *input)
{
  guint id;
  gint16 result;

  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), 0);

  switch (retro_input_get_controller_type (input)) {
  case RETRO_CONTROLLER_TYPE_JOYPAD:
    if (!retro_input_get_joypad (input, &id))
      return 0;

    return retro_core_view_get_joypad_button_state (self, id) ? G_MAXINT16 : 0;
  case RETRO_CONTROLLER_TYPE_MOUSE:
    if (!retro_input_get_mouse (input, &id))
      return 0;

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
  case RETRO_CONTROLLER_TYPE_KEYBOARD:
    if (!retro_input_get_keyboard (input, &id))
      return 0;

    return retro_core_view_get_keyboard_key_state (self, id) ? G_MAXINT16 : 0;
  case RETRO_CONTROLLER_TYPE_POINTER:
    if (!retro_input_get_pointer (input, &id))
      return 0;

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
 * retro_core_view_get_controller_capabilities:
 * @self: a #RetroCoreView
 *
 * Gets a flag representing the capabilities of @self when exposed as a
 * controller. See retro_controller_get_capabilities() for more information on
 * the flag.
 *
 * See retro_core_view_as_controller() to know the capabilities of
 * #RetroCoreView when exposed as a controller.
 *
 * Returns: the capabilities flag of @self when exposed as a controller
 */
guint64
retro_core_view_get_controller_capabilities (RetroCoreView *self)
{
  g_return_val_if_fail (RETRO_IS_CORE_VIEW (self), 0);

  return 1 << RETRO_CONTROLLER_TYPE_JOYPAD |
         1 << RETRO_CONTROLLER_TYPE_MOUSE |
         1 << RETRO_CONTROLLER_TYPE_KEYBOARD |
         1 << RETRO_CONTROLLER_TYPE_POINTER;
}

/**
 * retro_core_view_get_can_grab_pointer:
 * @self: a #RetroCoreView
 *
 * Gets whether the pointer should be grabbed when clicking on the view. This
 * allows @self to work as a RETRO_CONTROLLER_TYPE_MOUSE instead of a
 * RETRO_CONTROLLER_TYPE_POINTER.
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
 * allows @self to work as a RETRO_CONTROLLER_TYPE_MOUSE instead of a
 * RETRO_CONTROLLER_TYPE_POINTER.
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
 * a RETRO_CONTROLLER_TYPE_POINTER.
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
 * a RETRO_CONTROLLER_TYPE_POINTER.
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
