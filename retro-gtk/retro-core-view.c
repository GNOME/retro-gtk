// This file is part of retro-gtk. License: GPL-3.0+.

/**
 * SECTION:retro-core-view
 * @short_description: A widget to handle a core's inputs and outputs
 * @title: RetroCoreView
 * @See_also: #RetroCore, #RetroVideoFilter
 */

#include "retro-core-view.h"

#include "retro-gl-display-private.h"
#include "retro-controller-codes-private.h"
#include "retro-core-view-controller-private.h"
#include "retro-input-private.h"
#include "retro-keyboard-private.h"

#define RETRO_CONTROLLER_TYPE_COUNT (RETRO_CONTROLLER_TYPE_POINTER + 1)

struct _RetroCoreView
{
  GtkWidget parent_instance;
  RetroCore *core;
  RetroGLDisplay *display;
  gboolean can_grab_pointer;
  gboolean snap_pointer_to_borders;
  GHashTable *key_state;
  GHashTable *keyval_state;
  RetroKeyJoypadMapping *key_joypad_mapping;
  GHashTable *mouse_button_state;
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
  gboolean dragging;
};

G_DEFINE_TYPE (RetroCoreView, retro_core_view, GTK_TYPE_WIDGET)

enum {
  PROP_CAN_GRAB_POINTER = 1,
  PROP_SNAP_POINTER_TO_BORDERS,
  N_PROPS,
};

static GParamSpec *properties [N_PROPS];

enum {
  SIGNAL_CONTROLLER_STATE_CHANGED,
  N_SIGNALS,
};

static guint signals [N_SIGNALS];

/* Private */

static void
set_input_pressed (GHashTable *table,
                   guint       input)
{
  static gboolean value = TRUE;

  if (g_hash_table_contains (table, &input))
    g_hash_table_replace (table,
                          g_memdup2 (&input, sizeof (guint)),
                          g_memdup2 (&value, sizeof (gboolean)));
  else
    g_hash_table_insert (table,
                         g_memdup2 (&input, sizeof (guint)),
                         g_memdup2 (&value, sizeof (gboolean)));
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
  /* TODO: Reimplement this. See https://github.com/WebKit/webkit/blob/main/Source/WebKit/UIProcess/gtk/PointerLockManagerX11.cpp */
}

static gboolean
get_is_pointer_grabbed (RetroCoreView *self)
{
  return self->grabbed_device != NULL;
}

static void
grab (RetroCoreView *self,
      GdkDevice     *device)
{
  /* TODO: Reimplement this. See https://github.com/WebKit/webkit/blob/main/Source/WebKit/UIProcess/gtk/PointerLockManagerX11.cpp */

  g_set_object (&self->grabbed_device, device);

  recenter_pointer (self);

  g_signal_emit (self, signals[SIGNAL_CONTROLLER_STATE_CHANGED], 0);
}

static void
ungrab (RetroCoreView *self)
{
  /* TODO: Reimplement this. See https://github.com/WebKit/webkit/blob/main/Source/WebKit/UIProcess/gtk/PointerLockManagerX11.cpp */

  g_clear_object (&self->grabbed_device);

  g_signal_emit (self, signals[SIGNAL_CONTROLLER_STATE_CHANGED], 0);
}

static gboolean get_key_state (RetroCoreView *self,
                               guint16        hardware_keycode);

static gboolean
key_pressed_cb (RetroCoreView   *self,
                guint            keyval,
                guint            keycode,
                GdkModifierType  state)
{
  gboolean changed;

  if (keyval == GDK_KEY_Escape &&
      (state & GDK_CONTROL_MASK) &&
      get_is_pointer_grabbed (self))
    ungrab (self);

  changed = !get_key_state (self, keycode);

  set_input_pressed (self->key_state, keycode);
  set_input_pressed (self->keyval_state, keyval);

  if (changed)
    g_signal_emit (self, signals[SIGNAL_CONTROLLER_STATE_CHANGED], 0);

  return FALSE;
}

static void
key_released_cb (RetroCoreView   *self,
                 guint            keyval,
                 guint            keycode,
                 GdkModifierType  state)
{
  set_input_released (self->key_state, keycode);
  set_input_released (self->keyval_state, keyval);

  g_signal_emit (self, signals[SIGNAL_CONTROLLER_STATE_CHANGED], 0);
}

static void
drag_begin_cb (RetroCoreView *self,
               gdouble        start_x,
               gdouble        start_y,
               GtkGesture    *gesture)
{
  guint button = gtk_gesture_single_get_current_button (GTK_GESTURE_SINGLE (gesture));

  gtk_gesture_set_state (gesture, GTK_EVENT_SEQUENCE_CLAIMED);

  gtk_widget_grab_focus (GTK_WIDGET (self));

  if (retro_core_view_get_can_grab_pointer (self)) {
    if (get_is_pointer_grabbed (self))
      set_input_pressed (self->mouse_button_state, button);
    else {
      GdkDevice *device = gtk_event_controller_get_current_event_device (GTK_EVENT_CONTROLLER (gesture));

      grab (self, device);
    }
  }
  else {
    set_input_pressed (self->mouse_button_state, button);
    self->pointer_is_on_display =
      retro_gl_display_get_coordinates_on_display (self->display,
                                                   start_x,
                                                   start_y,
                                                   &self->pointer_x,
                                                   &self->pointer_y);
  }

  self->dragging = TRUE;

  g_signal_emit (self, signals[SIGNAL_CONTROLLER_STATE_CHANGED], 0);
}

static void
drag_update_cb (RetroCoreView *self,
                gdouble        offset_x,
                gdouble        offset_y,
                GtkGesture    *gesture)
{
  gdouble start_x, start_y;

  if (retro_core_view_get_can_grab_pointer (self))
    return;

  gtk_gesture_drag_get_start_point (GTK_GESTURE_DRAG (gesture), &start_x, &start_y);

  self->pointer_is_on_display =
    retro_gl_display_get_coordinates_on_display (self->display,
                                                 start_x + offset_x,
                                                 start_y + offset_y,
                                                 &self->pointer_x,
                                                 &self->pointer_y);

  g_signal_emit (self, signals[SIGNAL_CONTROLLER_STATE_CHANGED], 0);
}

static void
drag_end_cb (RetroCoreView *self,
             gdouble        offset_x,
             gdouble        offset_y,
             GtkGesture    *gesture)

{
  guint button = gtk_gesture_single_get_current_button (GTK_GESTURE_SINGLE (gesture));

  gtk_gesture_set_state (gesture, GTK_EVENT_SEQUENCE_CLAIMED);

  set_input_released (self->mouse_button_state, button);

  self->dragging = FALSE;

  g_signal_emit (self, signals[SIGNAL_CONTROLLER_STATE_CHANGED], 0);
}

static gboolean
leave_cb (RetroCoreView *self)
{
  if (get_is_pointer_grabbed (self))
    ungrab (self);

  reset_input (self->key_state);
  reset_input (self->mouse_button_state);

  g_signal_emit (self, signals[SIGNAL_CONTROLLER_STATE_CHANGED], 0);

  return GDK_EVENT_PROPAGATE;
}

static void
motion_cb (RetroCoreView      *self,
           gdouble             x,
           gdouble             y,
           GtkEventController *controller)
{
  if (self->dragging)
    return;

  if (retro_core_view_get_can_grab_pointer (self)) {
    GdkDevice *device = gtk_event_controller_get_current_event_device (controller);

    if (get_is_pointer_grabbed (self) &&
        device == self->grabbed_device) {
      self->mouse_x_delta += x - (double) self->screen_center_x;
      self->mouse_y_delta += y - (double) self->screen_center_y;

      recenter_pointer (self);
    }
  } else {
    self->pointer_is_on_display =
      retro_gl_display_get_coordinates_on_display (self->display,
                                                   x,
                                                   y,
                                                   &self->pointer_x,
                                                   &self->pointer_y);

  }

  g_signal_emit (self, signals[SIGNAL_CONTROLLER_STATE_CHANGED], 0);
}

static gboolean
get_key_state (RetroCoreView *self,
               guint16        hardware_keycode)
{
  return get_input_state (self->key_state, hardware_keycode);
}

static gboolean
get_joypad_button_state (RetroCoreView *self,
                         RetroJoypadId  button)
{
  guint16 hardware_keycode;

  if (button >= RETRO_JOYPAD_ID_COUNT)
    return 0;

  hardware_keycode = retro_key_joypad_mapping_get_button_key (self->key_joypad_mapping, button);

  return get_key_state (self, hardware_keycode);
}

static gboolean
get_mouse_button_state (RetroCoreView *self,
                        guint16        button)
{
  return get_input_state (self->mouse_button_state, button);
}

static gboolean
get_keyboard_key_state (RetroCoreView *self,
                        guint16        key)
{
  guint16 keyval;

  if (key >= RETRO_KEYBOARD_KEY_LAST)
    return FALSE;

  keyval = retro_keyboard_key_to_val_converter (key);

  return get_input_state (self->keyval_state, keyval);
}

static void
retro_core_view_dispose (GObject *object)
{
  RetroCoreView *self = RETRO_CORE_VIEW (object);

  g_clear_pointer ((GtkWidget **) &self->display, gtk_widget_unparent);

  g_clear_object (&self->grabbed_device);

  G_OBJECT_CLASS (retro_core_view_parent_class)->dispose (object);
}

static void
retro_core_view_finalize (GObject *object)
{
  RetroCoreView *self = RETRO_CORE_VIEW (object);

  g_clear_object (&self->core);
  g_hash_table_unref (self->key_state);
  g_hash_table_unref (self->keyval_state);
  g_object_unref (self->key_joypad_mapping);
  g_hash_table_unref (self->mouse_button_state);

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

  object_class->dispose = retro_core_view_dispose;
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

  /**
   * RetroCoreView::controller-state-changed:
   * @self: the #RetroCoreView
   *
   * The ::controller-state-changed signal is emitted when a key is pressed
   * or released, mouse pointer is moved, or a mouse button is pressed or
   * released.
   *
   * Applications should not connect to it.
   *
   * Stability: Private
   */
  signals[SIGNAL_CONTROLLER_STATE_CHANGED] =
    g_signal_new ("controller-state-changed",
                  RETRO_TYPE_CORE_VIEW,
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  0);

  gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BIN_LAYOUT);

  gtk_widget_class_set_css_name (widget_class, "retrocoreview");
}

static void
retro_core_view_init (RetroCoreView *self)
{
  GtkEventController *controller;

  gtk_widget_set_focusable (GTK_WIDGET (self), TRUE);

  self->display = g_object_ref_sink (retro_gl_display_new ());
  gtk_widget_set_can_focus (GTK_WIDGET (self->display), FALSE);
  gtk_widget_set_parent (GTK_WIDGET (self->display), GTK_WIDGET (self));

  g_object_bind_property (G_OBJECT (self), "sensitive",
                          G_OBJECT (self->display), "sensitive",
                          G_BINDING_BIDIRECTIONAL |
                          G_BINDING_SYNC_CREATE);

  self->key_state = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);
  self->keyval_state = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);
  self->key_joypad_mapping = retro_key_joypad_mapping_new_default ();
  self->mouse_button_state = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_free);

  controller = gtk_event_controller_key_new ();
  g_signal_connect_swapped (controller, "key-pressed", G_CALLBACK (key_pressed_cb), self);
  g_signal_connect_swapped (controller, "key-released", G_CALLBACK (key_released_cb), self);
  gtk_widget_add_controller (GTK_WIDGET (self), controller);

  controller = gtk_event_controller_focus_new ();
  g_signal_connect_swapped (controller, "leave", G_CALLBACK (leave_cb), self);
  gtk_widget_add_controller (GTK_WIDGET (self), controller);

  controller = gtk_event_controller_motion_new ();
  g_signal_connect_swapped (controller, "motion", G_CALLBACK (motion_cb), self);
  gtk_widget_add_controller (GTK_WIDGET (self), controller);

  controller = GTK_EVENT_CONTROLLER (gtk_gesture_drag_new ());
  g_signal_connect_swapped (controller, "drag-begin", G_CALLBACK (drag_begin_cb), self);
  g_signal_connect_swapped (controller, "drag-update", G_CALLBACK (drag_update_cb), self);
  g_signal_connect_swapped (controller, "drag-end", G_CALLBACK (drag_end_cb), self);
  gtk_widget_add_controller (GTK_WIDGET (self), controller);
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
  }

  if (core != NULL) {
    self->core = g_object_ref (core);
    retro_gl_display_set_core (self->display, core);
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

  g_set_object (&self->key_joypad_mapping, mapping);
  if (self->key_joypad_mapping == NULL)
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
  guint64 capabilities;

  g_return_if_fail (RETRO_IS_CORE_VIEW (self));
  g_return_if_fail (RETRO_IS_CORE (core));

  capabilities = retro_core_view_get_controller_capabilities (self);

  for (RetroControllerType type = RETRO_CONTROLLER_TYPE_NONE;
       type < RETRO_CONTROLLER_TYPE_COUNT;
       type++) {
    g_autoptr (RetroController) controller = NULL;

    if ((capabilities & (1 << type)) == 0)
      continue;

    controller = retro_core_view_as_controller (self, type);
    retro_core_set_default_controller (core, type, controller);
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

    return get_joypad_button_state (self, id) ? G_MAXINT16 : 0;
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
      return get_mouse_button_state (self, 1) ? G_MAXINT16 : 0;
    case RETRO_MOUSE_ID_RIGHT:
      return get_mouse_button_state (self, 3) ? G_MAXINT16 : 0;
    default:
      return 0;
    }
  case RETRO_CONTROLLER_TYPE_KEYBOARD:
    if (!retro_input_get_keyboard (input, &id))
      return 0;

    return get_keyboard_key_state (self, id) ? G_MAXINT16 : 0;
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

      return get_mouse_button_state (self, 1) ? 1 : 0;
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
      get_is_pointer_grabbed (self))
    ungrab (self);

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
