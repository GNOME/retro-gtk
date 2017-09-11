// This file is part of retro-gtk. License: GPL-3.0+.

public class Retro.CoreView : Gtk.EventBox {
	private Binding pixbuf_binding;
	private Binding sensitive_binding;
	public Gdk.Pixbuf pixbuf { set; get; }

	private bool _can_grab_pointer;
	public bool can_grab_pointer {
		set {
			if (value == _can_grab_pointer)
				return;

			_can_grab_pointer = value;

			if (_can_grab_pointer == false && get_is_pointer_grabbed ())
				ungrab ();
		}
		get { return _can_grab_pointer; }
		default = false;
	}

	public bool snap_pointer_to_borders { set; get; }

	private Core core;
	internal CairoDisplay display;
	internal PaPlayer audio_player;

	internal HashTable<uint?, bool?> key_state;
	internal HashTable<uint?, bool?> mouse_button_state;
	internal Gdk.Screen grabbed_screen;
	internal Gdk.Device grabbed_device;
	internal double mouse_x_delta;
	internal double mouse_y_delta;
	internal int screen_center_x;
	internal int screen_center_y;
	internal int position_on_grab_x;
	internal int position_on_grab_y;
	internal bool pointer_is_on_display;
	internal double pointer_x;
	internal double pointer_y;

	construct {
		can_focus = true;

		display = new CairoDisplay ();
		display.visible = true;
		display.can_focus = false;
		add (display);

		audio_player = new PaPlayer ();

		pixbuf_binding = display.bind_property ("pixbuf", this, "pixbuf",
		                                        BindingFlags.BIDIRECTIONAL |
		                                        BindingFlags.SYNC_CREATE);
		sensitive_binding = bind_property ("sensitive", display, "sensitive",
		                                   BindingFlags.BIDIRECTIONAL |
		                                   BindingFlags.SYNC_CREATE);

		key_state = new HashTable<uint?, bool?> (int_hash, int_equal);
		mouse_button_state = new HashTable<uint?, bool?> (int_hash, int_equal);

		key_press_event.connect (on_key_press_event);
		key_release_event.connect (on_key_release_event);
		button_press_event.connect (on_button_press_event);
		button_release_event.connect (on_button_release_event);
		focus_out_event.connect (on_focus_out_event);
		motion_notify_event.connect (on_motion_notify_event);
	}

	public void set_core (Core? core) {
		if (this.core != null) {
			display.set_core (null);
			audio_player.set_core (null);
		}

		this.core = core;

		if (core != null) {
			display.set_core (core);
			audio_player.set_core (core);
		}
	}

	public extern void set_filter (VideoFilter filter) ;
	public extern void show_video ();
	public extern void hide_video ();
	public extern InputDevice as_input_device (DeviceType device_type);
	internal extern int16 get_input_state (DeviceType device, uint index, uint id);
	internal extern uint64 get_device_capabilities ();
	internal extern bool get_is_pointer_grabbed ();
	internal extern void ungrab ();
	internal extern bool on_key_press_event (Gtk.Widget source, Gdk.EventKey event);
	internal extern bool on_key_release_event (Gtk.Widget source, Gdk.EventKey event);
	internal extern bool on_button_press_event (Gtk.Widget source, Gdk.EventButton event);
	internal extern bool on_button_release_event (Gtk.Widget source, Gdk.EventButton event);
	internal extern bool on_focus_out_event (Gtk.Widget source, Gdk.EventFocus event);
	internal extern bool on_motion_notify_event (Gtk.Widget source, Gdk.EventMotion event);
}
