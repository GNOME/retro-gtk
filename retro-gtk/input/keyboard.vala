// This file is part of RetroGtk. License: GPLv3

namespace RetroGtk {

public class Keyboard : Object {
	public Gtk.Widget widget { get; construct; }

	public signal void key_event (bool down, Retro.KeyboardKey keycode, uint32 character, Retro.KeyboardModifierKey key_modifiers);

	public Keyboard (Gtk.Widget widget) {
		Object (widget: widget);
	}

	construct {
		widget.key_press_event.connect ((w, e) => on_key_event (e, true));
		widget.key_release_event.connect ((w, e) => on_key_event (e, false));
	}

	private Retro.KeyboardModifierKey modifier_key_converter (uint keyval, Gdk.ModifierType modifiers) {
		var retro_modifiers = Retro.KeyboardModifierKey.NONE;
		if ((bool) (modifiers & Gdk.ModifierType.SHIFT_MASK))
			retro_modifiers |= Retro.KeyboardModifierKey.SHIFT;
		if ((bool) (modifiers & Gdk.ModifierType.CONTROL_MASK))
			retro_modifiers |= Retro.KeyboardModifierKey.CTRL;
		if ((bool) (modifiers & Gdk.ModifierType.MOD1_MASK))
			retro_modifiers |= Retro.KeyboardModifierKey.ALT;
		if ((bool) (modifiers & Gdk.ModifierType.META_MASK))
			retro_modifiers |= Retro.KeyboardModifierKey.META;
		if (keyval == Gdk.Key.Num_Lock)
			retro_modifiers |= Retro.KeyboardModifierKey.NUMLOCK;
		if ((bool) (modifiers & Gdk.ModifierType.LOCK_MASK))
			retro_modifiers |= Retro.KeyboardModifierKey.CAPSLOCK;
		if (keyval == Gdk.Key.Scroll_Lock)
			retro_modifiers |= Retro.KeyboardModifierKey.SCROLLOCK;
		return retro_modifiers;
	}

	private Retro.KeyboardKey key_converter (uint keyval) {
		// Common keys (0x0020 to 0x00fe)
		if (keyval < 0x80) {
			var key = (0x7f & keyval);

			// If the key is uppercase, turn it lower case
			if (key >= 'A' && key <= 'Z')
				return (Retro.KeyboardKey) (key + 0x20);

			return (Retro.KeyboardKey) key;
		}

		// Function keys
		var fx = keyval - Gdk.Key.F1;
		if (fx < 15) return (Retro.KeyboardKey) ((uint) Retro.KeyboardKey.F1 + fx);

		// Keypad digits
		var kp = keyval - Gdk.Key.KP_0;
		if (kp < 10) return (Retro.KeyboardKey) ((uint) Retro.KeyboardKey.KP0 + kp);

		// Various keys
		// Missing keys: MODE, COMPOSE, POWER
		switch (keyval) {
			case Gdk.Key.BackSpace:
				return Retro.KeyboardKey.BACKSPACE;
			case Gdk.Key.Tab:
				return Retro.KeyboardKey.TAB;
			case Gdk.Key.Clear:
				return Retro.KeyboardKey.CLEAR;
			case Gdk.Key.Return:
				return Retro.KeyboardKey.RETURN;
			case Gdk.Key.Pause:
				return Retro.KeyboardKey.PAUSE;
			case Gdk.Key.Escape:
				return Retro.KeyboardKey.ESCAPE;
			case Gdk.Key.Delete:
				return Retro.KeyboardKey.DELETE;

			case Gdk.Key.Up:
				return Retro.KeyboardKey.UP;
			case Gdk.Key.Down:
				return Retro.KeyboardKey.DOWN;
			case Gdk.Key.Left:
				return Retro.KeyboardKey.LEFT;
			case Gdk.Key.Right:
				return Retro.KeyboardKey.RIGHT;
			case Gdk.Key.Insert:
				return Retro.KeyboardKey.INSERT;
			case Gdk.Key.Home:
				return Retro.KeyboardKey.HOME;
			case Gdk.Key.End:
				return Retro.KeyboardKey.END;
			case Gdk.Key.Page_Up:
				return Retro.KeyboardKey.PAGEUP;
			case Gdk.Key.Page_Down:
				return Retro.KeyboardKey.PAGEDOWN;

			case Gdk.Key.KP_Decimal:
				return Retro.KeyboardKey.KP_PERIOD;
			case Gdk.Key.KP_Divide:
				return Retro.KeyboardKey.KP_DIVIDE;
			case Gdk.Key.KP_Multiply:
				return Retro.KeyboardKey.KP_MULTIPLY;
			case Gdk.Key.KP_Subtract:
				return Retro.KeyboardKey.KP_MINUS;
			case Gdk.Key.KP_Add:
				return Retro.KeyboardKey.KP_PLUS;
			case Gdk.Key.KP_Enter:
				return Retro.KeyboardKey.KP_ENTER;
			case Gdk.Key.KP_Equal:
				return Retro.KeyboardKey.KP_EQUALS;

			case Gdk.Key.Num_Lock:
				return Retro.KeyboardKey.NUMLOCK;
			case Gdk.Key.Caps_Lock:
				return Retro.KeyboardKey.CAPSLOCK;
			case Gdk.Key.Scroll_Lock:
				return Retro.KeyboardKey.SCROLLOCK;
			case Gdk.Key.Shift_R:
				return Retro.KeyboardKey.RSHIFT;
			case Gdk.Key.Shift_L:
				return Retro.KeyboardKey.LSHIFT;
			case Gdk.Key.Control_R:
				return Retro.KeyboardKey.RCTRL;
			case Gdk.Key.Control_L:
				return Retro.KeyboardKey.LCTRL;
			case Gdk.Key.Alt_R:
				return Retro.KeyboardKey.RALT;
			case Gdk.Key.Alt_L:
				return Retro.KeyboardKey.LALT;
			case Gdk.Key.Meta_R:
				return Retro.KeyboardKey.RMETA;
			case Gdk.Key.Meta_L:
				return Retro.KeyboardKey.LMETA;
			case Gdk.Key.Super_R:
				return Retro.KeyboardKey.RSUPER;
			case Gdk.Key.Super_L:
				return Retro.KeyboardKey.LSUPER;

			case Gdk.Key.Help:
				return Retro.KeyboardKey.HELP;
			case Gdk.Key.Print:
				return Retro.KeyboardKey.PRINT;
			case Gdk.Key.Sys_Req:
				return Retro.KeyboardKey.SYSREQ;
			case Gdk.Key.Break:
				return Retro.KeyboardKey.BREAK;
			case Gdk.Key.Menu:
				return Retro.KeyboardKey.MENU;
			case Gdk.Key.EuroSign:
				return Retro.KeyboardKey.EURO;
			case Gdk.Key.Undo:
				return Retro.KeyboardKey.UNDO;

			default:
				return Retro.KeyboardKey.UNKNOWN;
		}
	}

	private bool on_key_event (Gdk.EventKey event, bool pressed) {
		key_event (
			pressed,
			key_converter (event.keyval),
			event.str.to_utf8 ()[0],
			modifier_key_converter (event.keyval, event.state)
		);

		return false;
	}

}

}

