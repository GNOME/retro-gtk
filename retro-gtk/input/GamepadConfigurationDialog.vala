/* Engine.vala  A simple frontend for libretro.
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

using Gdk;
using Gtk;

namespace RetroGtk {

public class GamepadConfigurationDialog : Gtk.Dialog {
	private GamepadView view;
	private Label prompt;

	private Widget apply_button;

	private GamepadConfiguration current_configuration { get; set; }
	public GamepadConfiguration? configuration { get; private set; }

	private uint current_button;
	private static const GamepadButtonType[] buttons = {
		GamepadButtonType.ACTION_DOWN,
		GamepadButtonType.ACTION_RIGHT,
		GamepadButtonType.ACTION_LEFT,
		GamepadButtonType.ACTION_UP,
		GamepadButtonType.DIRECTION_UP,
		GamepadButtonType.DIRECTION_DOWN,
		GamepadButtonType.DIRECTION_LEFT,
		GamepadButtonType.DIRECTION_RIGHT,
		GamepadButtonType.START,
		GamepadButtonType.SELECT,
		GamepadButtonType.HOME,
		GamepadButtonType.SHOULDER_L,
		GamepadButtonType.SHOULDER_R,
		GamepadButtonType.SHOULDER_L2,
		GamepadButtonType.SHOULDER_R2,
		GamepadButtonType.STICK_L,
		GamepadButtonType.STICK_R
	};

	construct {
		current_button = 0;
		use_header_bar = 1;

		current_configuration = new GamepadConfiguration ();

		view = new RetroGtk.GamepadView ();
		view.set_size_request (320, 240);
		view.show ();

		prompt = new Label (null);
		prompt.show ();

		var kb = new EventBox ();
		new KeyboardState (kb);
		kb.key_press_event.connect (on_button_press_event);
		kb.show ();

		var box = new Box (Orientation.VERTICAL, 6);
		box.pack_start (view);
		box.pack_start (prompt);
		box.show ();

		kb.add (box);
		get_content_area ().pack_start (kb);

		apply_button = add_button ("_Apply", ResponseType.APPLY);
		add_button ("_Cancel", ResponseType.CANCEL);

		response.connect ((response_id) => {
			if (response_id == ResponseType.APPLY)
				configuration = current_configuration;
		});

		update ();
	}

	private void prompt_button (GamepadButtonType? button) {
		view.reset ();

		if (button == null) {
			prompt.set_text ("Configuration finished");
			return;
		}

		view.highlight_button ((GamepadButtonType) button, true);

		var button_name = button.to_string ();
		prompt.set_text (@"Press a key for the $button_name button");
	}

	private bool on_button_press_event (EventKey event) {
		if (current_button < buttons.length) {
			current_configuration.set_button_key (buttons[current_button], event.hardware_keycode);
			current_button++;
			update ();
		}
		return false;
	}

	private void update () {
		if (current_button < buttons.length) {
			prompt_button (buttons[current_button]);
			set_response_sensitive (ResponseType.APPLY, false);
		}
		else {
			prompt_button (null);
			set_response_sensitive (ResponseType.APPLY, true);
		}
	}
}

}

