/* Window.vala  A simple display.
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

using Retro;
using RetroGtk;

using Gtk;

public class GamepadViewWindow : Gtk.Window {
	private GamepadView view;
	private KeyboardBoxJoypadAdapter joypad;

	construct {
		view = new RetroGtk.GamepadView ();
		view.set_size_request (320, 240);
		view.show ();

		var kb = new KeyboardBox ();
		kb.key_state_changed.connect (() => { update (); });
		kb.show ();

		joypad = new KeyboardBoxJoypadAdapter (kb);

		kb.add (view);
		add (kb);
	}

	private void update () {
		const JoypadId[] ids = {
			JoypadId.B,
			JoypadId.Y,
			JoypadId.SELECT,
			JoypadId.START,
			JoypadId.UP,
			JoypadId.DOWN,
			JoypadId.LEFT,
			JoypadId.RIGHT,
			JoypadId.A,
			JoypadId.X,
			JoypadId.L,
			JoypadId.R,
			JoypadId.L2,
			JoypadId.R2,
			JoypadId.L3,
			JoypadId.R3
		};

		foreach (var id in ids) {
			view.highlight_button (
				GamepadButtonType.from_joypad_id (id),
				joypad.get_input_state (DeviceType.JOYPAD, 0, id) != 0
			);
		}
	}
}

