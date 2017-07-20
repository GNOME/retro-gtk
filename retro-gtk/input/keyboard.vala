// This file is part of retro-gtk. License: GPL-3.0+.

/**
 * Key types to feed to a {@link Core} via {@link Input.key_event}.
 */
private enum Retro.KeyboardKey {
	UNKNOWN        = 0,
	FIRST          = 0,
	BACKSPACE      = 8,
	TAB            = 9,
	CLEAR          = 12,
	RETURN         = 13,
	PAUSE          = 19,
	ESCAPE         = 27,
	SPACE          = 32,
	EXCLAIM        = 33,
	QUOTEDBL       = 34,
	HASH           = 35,
	DOLLAR         = 36,
	AMPERSAND      = 38,
	QUOTE          = 39,
	LEFTPAREN      = 40,
	RIGHTPAREN     = 41,
	ASTERISK       = 42,
	PLUS           = 43,
	COMMA          = 44,
	MINUS          = 45,
	PERIOD         = 46,
	SLASH          = 47,
	ZERO           = 48,
	ONE            = 49,
	TWO            = 50,
	THREE          = 51,
	FOUR           = 52,
	FINE           = 53,
	SIX            = 54,
	SEVEN          = 55,
	HEIGHT         = 56,
	NINE           = 57,
	COLON          = 58,
	SEMICOLON      = 59,
	LESS           = 60,
	EQUALS         = 61,
	GREATER        = 62,
	QUESTION       = 63,
	AT             = 64,
	LEFTBRACKET    = 91,
	BACKSLASH      = 92,
	RIGHTBRACKET   = 93,
	CARET          = 94,
	UNDERSCORE     = 95,
	BACKQUOTE      = 96,
	a              = 97,
	b              = 98,
	c              = 99,
	d              = 100,
	e              = 101,
	f              = 102,
	g              = 103,
	h              = 104,
	i              = 105,
	j              = 106,
	k              = 107,
	l              = 108,
	m              = 109,
	n              = 110,
	o              = 111,
	p              = 112,
	q              = 113,
	r              = 114,
	s              = 115,
	t              = 116,
	u              = 117,
	v              = 118,
	w              = 119,
	x              = 120,
	y              = 121,
	z              = 122,
	DELETE         = 127,

	KP0            = 256,
	KP1            = 257,
	KP2            = 258,
	KP3            = 259,
	KP4            = 260,
	KP5            = 261,
	KP6            = 262,
	KP7            = 263,
	KP8            = 264,
	KP9            = 265,
	KP_PERIOD      = 266,
	KP_DIVIDE      = 267,
	KP_MULTIPLY    = 268,
	KP_MINUS       = 269,
	KP_PLUS        = 270,
	KP_ENTER       = 271,
	KP_EQUALS      = 272,

	UP             = 273,
	DOWN           = 274,
	RIGHT          = 275,
	LEFT           = 276,
	INSERT         = 277,
	HOME           = 278,
	END            = 279,
	PAGEUP         = 280,
	PAGEDOWN       = 281,

	F1             = 282,
	F2             = 283,
	F3             = 284,
	F4             = 285,
	F5             = 286,
	F6             = 287,
	F7             = 288,
	F8             = 289,
	F9             = 290,
	F10            = 291,
	F11            = 292,
	F12            = 293,
	F13            = 294,
	F14            = 295,
	F15            = 296,

	NUMLOCK        = 300,
	CAPSLOCK       = 301,
	SCROLLOCK      = 302,
	RSHIFT         = 303,
	LSHIFT         = 304,
	RCTRL          = 305,
	LCTRL          = 306,
	RALT           = 307,
	LALT           = 308,
	RMETA          = 309,
	LMETA          = 310,
	LSUPER         = 311,
	RSUPER         = 312,
	MODE           = 313,
	COMPOSE        = 314,

	HELP           = 315,
	PRINT          = 316,
	SYSREQ         = 317,
	BREAK          = 318,
	MENU           = 319,
	POWER          = 320,
	EURO           = 321,
	UNDO           = 322,

	LAST
}

/**
 * Modifier key types to feed to a {@link Core} via {@link Input.key_event}.
 */
[Flags]
private enum Retro.KeyboardModifierKey {
	NONE       = 0x0000,

	SHIFT      = 0x01,
	CTRL       = 0x02,
	ALT        = 0x04,
	META       = 0x08,

	NUMLOCK    = 0x10,
	CAPSLOCK   = 0x20,
	SCROLLOCK  = 0x40
}

public class Retro.Keyboard : Object {
	public Gtk.Widget widget { get; construct; }

	public signal bool key_event (Gdk.EventKey event);

	public Keyboard (Gtk.Widget widget) {
		Object (widget: widget);
	}

	construct {
		widget.key_press_event.connect ((w, e) => key_event (e));
		widget.key_release_event.connect ((w, e) => key_event (e));
	}

	internal static KeyboardModifierKey modifier_key_converter (uint keyval, Gdk.ModifierType modifiers) {
		var retro_modifiers = KeyboardModifierKey.NONE;
		if ((bool) (modifiers & Gdk.ModifierType.SHIFT_MASK))
			retro_modifiers |= KeyboardModifierKey.SHIFT;
		if ((bool) (modifiers & Gdk.ModifierType.CONTROL_MASK))
			retro_modifiers |= KeyboardModifierKey.CTRL;
		if ((bool) (modifiers & Gdk.ModifierType.MOD1_MASK))
			retro_modifiers |= KeyboardModifierKey.ALT;
		if ((bool) (modifiers & Gdk.ModifierType.META_MASK))
			retro_modifiers |= KeyboardModifierKey.META;
		if (keyval == Gdk.Key.Num_Lock)
			retro_modifiers |= KeyboardModifierKey.NUMLOCK;
		if ((bool) (modifiers & Gdk.ModifierType.LOCK_MASK))
			retro_modifiers |= KeyboardModifierKey.CAPSLOCK;
		if (keyval == Gdk.Key.Scroll_Lock)
			retro_modifiers |= KeyboardModifierKey.SCROLLOCK;
		return retro_modifiers;
	}

	internal static KeyboardKey key_converter (uint keyval) {
		// Common keys (0x0020 to 0x00fe)
		if (keyval < 0x80) {
			var key = (0x7f & keyval);

			// If the key is uppercase, turn it lower case
			if (key >= 'A' && key <= 'Z')
				return (KeyboardKey) (key + 0x20);

			return (KeyboardKey) key;
		}

		// Function keys
		var fx = keyval - Gdk.Key.F1;
		if (fx < 15) return (KeyboardKey) ((uint) KeyboardKey.F1 + fx);

		// Keypad digits
		var kp = keyval - Gdk.Key.KP_0;
		if (kp < 10) return (KeyboardKey) ((uint) KeyboardKey.KP0 + kp);

		// Various keys
		// Missing keys: MODE, COMPOSE, POWER
		switch (keyval) {
			case Gdk.Key.BackSpace:
				return KeyboardKey.BACKSPACE;
			case Gdk.Key.Tab:
				return KeyboardKey.TAB;
			case Gdk.Key.Clear:
				return KeyboardKey.CLEAR;
			case Gdk.Key.Return:
				return KeyboardKey.RETURN;
			case Gdk.Key.Pause:
				return KeyboardKey.PAUSE;
			case Gdk.Key.Escape:
				return KeyboardKey.ESCAPE;
			case Gdk.Key.Delete:
				return KeyboardKey.DELETE;

			case Gdk.Key.Up:
				return KeyboardKey.UP;
			case Gdk.Key.Down:
				return KeyboardKey.DOWN;
			case Gdk.Key.Left:
				return KeyboardKey.LEFT;
			case Gdk.Key.Right:
				return KeyboardKey.RIGHT;
			case Gdk.Key.Insert:
				return KeyboardKey.INSERT;
			case Gdk.Key.Home:
				return KeyboardKey.HOME;
			case Gdk.Key.End:
				return KeyboardKey.END;
			case Gdk.Key.Page_Up:
				return KeyboardKey.PAGEUP;
			case Gdk.Key.Page_Down:
				return KeyboardKey.PAGEDOWN;

			case Gdk.Key.KP_Decimal:
				return KeyboardKey.KP_PERIOD;
			case Gdk.Key.KP_Divide:
				return KeyboardKey.KP_DIVIDE;
			case Gdk.Key.KP_Multiply:
				return KeyboardKey.KP_MULTIPLY;
			case Gdk.Key.KP_Subtract:
				return KeyboardKey.KP_MINUS;
			case Gdk.Key.KP_Add:
				return KeyboardKey.KP_PLUS;
			case Gdk.Key.KP_Enter:
				return KeyboardKey.KP_ENTER;
			case Gdk.Key.KP_Equal:
				return KeyboardKey.KP_EQUALS;

			case Gdk.Key.Num_Lock:
				return KeyboardKey.NUMLOCK;
			case Gdk.Key.Caps_Lock:
				return KeyboardKey.CAPSLOCK;
			case Gdk.Key.Scroll_Lock:
				return KeyboardKey.SCROLLOCK;
			case Gdk.Key.Shift_R:
				return KeyboardKey.RSHIFT;
			case Gdk.Key.Shift_L:
				return KeyboardKey.LSHIFT;
			case Gdk.Key.Control_R:
				return KeyboardKey.RCTRL;
			case Gdk.Key.Control_L:
				return KeyboardKey.LCTRL;
			case Gdk.Key.Alt_R:
				return KeyboardKey.RALT;
			case Gdk.Key.Alt_L:
				return KeyboardKey.LALT;
			case Gdk.Key.Meta_R:
				return KeyboardKey.RMETA;
			case Gdk.Key.Meta_L:
				return KeyboardKey.LMETA;
			case Gdk.Key.Super_R:
				return KeyboardKey.RSUPER;
			case Gdk.Key.Super_L:
				return KeyboardKey.LSUPER;

			case Gdk.Key.Help:
				return KeyboardKey.HELP;
			case Gdk.Key.Print:
				return KeyboardKey.PRINT;
			case Gdk.Key.Sys_Req:
				return KeyboardKey.SYSREQ;
			case Gdk.Key.Break:
				return KeyboardKey.BREAK;
			case Gdk.Key.Menu:
				return KeyboardKey.MENU;
			case Gdk.Key.EuroSign:
				return KeyboardKey.EURO;
			case Gdk.Key.Undo:
				return KeyboardKey.UNDO;

			default:
				return KeyboardKey.UNKNOWN;
		}
	}
}
