/* RetroGtk  Building blocks for a Retro frontend.
 * Copyright (C) 2014  Adrien Plazas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

using Gtk;
using Gdk;

using Retro;

namespace RetroGtk {

public class Keyboard : Object {
	public Widget widget { get; construct; }

	public signal void key_event (bool down, KeyboardKey keycode, uint32 character, KeyboardModifierKey key_modifiers);

	public Keyboard (Widget widget) {
		Object (widget: widget);
	}

	construct {
		widget.key_press_event.connect ((w, e) => on_key_event (e, true));
		widget.key_release_event.connect ((w, e) => on_key_event (e, false));
	}

	private KeyboardModifierKey modifier_key_converter (uint keyval, Gdk.ModifierType modifiers) {
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

	private KeyboardKey key_converter (uint keyval) {
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

