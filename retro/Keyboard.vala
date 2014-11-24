/* Retro  GObject libretro wrapper.
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

namespace Retro {

public enum KeyboardKey {
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

[Flags]
public enum KeyboardModifierKey {
	NONE       = 0x0000,

	SHIFT      = 0x01,
	CTRL       = 0x02,
	ALT        = 0x04,
	META       = 0x08,

	NUMLOCK    = 0x10,
	CAPSLOCK   = 0x20,
	SCROLLOCK  = 0x40
}

[CCode (has_target = false)]
public delegate void KeyboardEvent (bool down, KeyboardKey keycode, uint32 character, KeyboardModifierKey key_modifiers);

public struct KeyboardCallback {
	KeyboardEvent callback;
}

}
