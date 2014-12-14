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

using Jsk;

namespace RetroGtk {

public class Gamepad : Object, ControllerDevice {
	public Jsk.Gamepad gamepad { get; construct; }

	private int16[] gamepad_state;

	public Gamepad (Jsk.Gamepad gamepad) {
		Object (gamepad: gamepad);
	}

	construct {
		gamepad_state = new int16[Jsk.GamepadInputType.MAX];

		gamepad.gamepad_event.connect ((e) => gamepad_state[e.type] = e.value);
	}

	public void poll () {}

	public int16 get_input_state (Retro.DeviceType device, uint index, uint id) {
		switch (device) {
			case Retro.DeviceType.JOYPAD:
				return get_button_pressed ((GamepadButtonType) id) ? int16.MAX : 0;
			case Retro.DeviceType.ANALOG:
				return get_analog_value ((Retro.AnalogIndex) index, (Retro.AnalogId) id);
			default:
				return 0;
		}
	}

	public Retro.DeviceType get_device_type () {
		return Retro.DeviceType.ANALOG;
	}

	public uint64 get_device_capabilities () {
		return (1 << Retro.DeviceType.JOYPAD) | (1 << Retro.DeviceType.ANALOG);
	}

	public bool get_button_pressed (GamepadButtonType button) {
		switch (button) {
			case GamepadButtonType.ACTION_DOWN:
				return gamepad_state[GamepadInputType.APAD_DOWN] != 0;
			case GamepadButtonType.ACTION_LEFT:
				return gamepad_state[GamepadInputType.APAD_LEFT] != 0;
			case GamepadButtonType.SELECT:
				return gamepad_state[GamepadInputType.SELECT] != 0;
			case GamepadButtonType.START:
				return gamepad_state[GamepadInputType.START] != 0;
			case GamepadButtonType.DIRECTION_UP:
				return gamepad_state[GamepadInputType.DPAD_UP] != 0;
			case GamepadButtonType.DIRECTION_DOWN:
				return gamepad_state[GamepadInputType.DPAD_DOWN] != 0;
			case GamepadButtonType.DIRECTION_LEFT:
				return gamepad_state[GamepadInputType.DPAD_LEFT] != 0;
			case GamepadButtonType.DIRECTION_RIGHT:
				return gamepad_state[GamepadInputType.DPAD_RIGHT] != 0;
			case GamepadButtonType.ACTION_RIGHT:
				return gamepad_state[GamepadInputType.APAD_RIGHT] != 0;
			case GamepadButtonType.ACTION_UP:
				return gamepad_state[GamepadInputType.APAD_UP] != 0;
			case GamepadButtonType.SHOULDER_L:
				return gamepad_state[GamepadInputType.SHOULDER_L] != 0;
			case GamepadButtonType.SHOULDER_R:
				return gamepad_state[GamepadInputType.SHOULDER_R] != 0;
			case GamepadButtonType.SHOULDER_L2:
				return gamepad_state[GamepadInputType.TRIGGER_L] != 0;
			case GamepadButtonType.SHOULDER_R2:
				return gamepad_state[GamepadInputType.TRIGGER_R] != 0;
			case GamepadButtonType.STICK_L:
				return gamepad_state[GamepadInputType.STICK_L] != 0;
			case GamepadButtonType.STICK_R:
				return gamepad_state[GamepadInputType.STICK_R] != 0;
			case GamepadButtonType.HOME:
				return gamepad_state[GamepadInputType.HOME] != 0;
			default:
				return false;
		}
	}

	public int16 get_analog_value (Retro.AnalogIndex index, Retro.AnalogId id) {
		switch (index) {
			case Retro.AnalogIndex.LEFT:
				switch (id) {
					case Retro.AnalogId.X:
						return gamepad_state[GamepadInputType.LEFT_X];
					case Retro.AnalogId.Y:
						return gamepad_state[GamepadInputType.LEFT_Y];
					default:
						return 0;
				}
			case Retro.AnalogIndex.RIGHT:
				switch (id) {
					case Retro.AnalogId.X:
						return gamepad_state[GamepadInputType.RIGHT_X];
					case Retro.AnalogId.Y:
						return gamepad_state[GamepadInputType.RIGHT_Y];
					default:
						return 0;
				}
			default:
				return 0;
		}
	}
}

}

