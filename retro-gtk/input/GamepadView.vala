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

using Gtk;
using Cairo;

namespace RetroGtk {

public enum GamepadButtonType {
	ACTION_DOWN,
	ACTION_LEFT,
	SELECT,
	START,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	ACTION_RIGHT,
	ACTION_UP,
	SHOULDER_L,
	SHOULDER_R,
	SHOULDER_L2,
	SHOULDER_R2,
	STICK_L,
	STICK_R,
	HOME;

	public static size_t size () {
		return 17;
	}
}

public class GamepadView : DrawingArea {
	private static const StateFlags DEFAULT_STATE = StateFlags.NORMAL;
	private static const StateFlags HIGHLIGHT_STATE = StateFlags.LINK;

	private bool[] button_lightning;

	private static const double TAU = Math.PI * 2;

	private Context cr;

	construct {
		button_lightning = new bool[GamepadButtonType.size ()];
	}

	public void highlight_button (GamepadButtonType button, bool highlighted) {
		button_lightning[button] = highlighted;
		queue_draw ();
	}

	private void render (GamepadButtonType button) {
		var state = button_lightning[button] ? HIGHLIGHT_STATE : DEFAULT_STATE;
		var color = get_style_context ().get_color (state);
		cr.set_source_rgba (color.red, color.green, color.blue, color.alpha);

		cr.fill ();
	}

	public override bool draw (Context cr) {
		this.cr = cr;
		var width = get_allocated_width ();
		var height = get_allocated_height ();

		gamepad (width, height);

		return false;
	}

	private void gamepad (int width, int height) {
		double x, y, w, h;
		if (width * 3 > height * 4) {
			// The bounding box is wider than tall
			h = (double) height;
			w = h * 4/3;
			y = 0.0;
			x = ((double) width - w) / 2;
		}
		else {
			// The bounding box is taller than wide
			w = (double) width;
			h = w * 3/4;
			x = 0.0;
			y = ((double) height - h) / 2;
		}

		var mid_h = y + h*0.55;

		dpad (x + w*0.2, mid_h, w*0.2);
		action_buttons (x + w*0.8, mid_h, w*0.25);
		control_buttons (x + w/2.0, mid_h, w*0.1);
		shoulders (x + w/2.0, y + h*0.2, w/5, w/15, w*0.3, w/20);
		joysticks (x + w/2.0, y + h*0.8, w*0.08, w/3.0);
	}

	private void dpad (double xc, double yc, double side) {
		var width = side / 3;
		var w_shift = side / 6;
		var l_shift = side / 2;

		// Draw a square in the middle to avoid seeing demaraction lines
		var color = get_style_context ().get_color (DEFAULT_STATE);
		cr.set_source_rgba (color.red, color.green, color.blue, color.alpha);

		cr.rectangle (xc - w_shift, yc - w_shift, width, width);
		cr.fill ();

		cr.move_to (xc, yc);
		cr.line_to (xc - w_shift, yc - w_shift);
		cr.line_to (xc - w_shift, yc - l_shift);
		cr.line_to (xc + w_shift, yc - l_shift);
		cr.line_to (xc + w_shift, yc - w_shift);
		cr.line_to (xc, yc);
		render (GamepadButtonType.DIRECTION_UP);

		cr.move_to (xc, yc);
		cr.line_to (xc - w_shift, yc + w_shift);
		cr.line_to (xc - w_shift, yc + l_shift);
		cr.line_to (xc + w_shift, yc + l_shift);
		cr.line_to (xc + w_shift, yc + w_shift);
		cr.line_to (xc, yc);
		render (GamepadButtonType.DIRECTION_DOWN);

		cr.move_to (xc, yc);
		cr.line_to (xc - w_shift, yc - w_shift);
		cr.line_to (xc - l_shift, yc - w_shift);
		cr.line_to (xc - l_shift, yc + w_shift);
		cr.line_to (xc - w_shift, yc + w_shift);
		cr.line_to (xc, yc);
		render (GamepadButtonType.DIRECTION_LEFT);

		cr.move_to (xc, yc);
		cr.line_to (xc + w_shift, yc - w_shift);
		cr.line_to (xc + l_shift, yc - w_shift);
		cr.line_to (xc + l_shift, yc + w_shift);
		cr.line_to (xc + w_shift, yc + w_shift);
		cr.line_to (xc, yc);
		render (GamepadButtonType.DIRECTION_RIGHT);
	}

	private void action_buttons (double xc, double yc, double side) {
		var radius = side / 6;
		var shift = side / 3;

		circle (xc + shift, yc, radius);
		render (GamepadButtonType.ACTION_RIGHT);
		circle (xc - shift, yc, radius);
		render (GamepadButtonType.ACTION_LEFT);
		circle (xc, yc + shift, radius);
		render (GamepadButtonType.ACTION_DOWN);
		circle (xc, yc - shift, radius);
		render (GamepadButtonType.ACTION_UP);
	}

	private void control_buttons (double xc, double yc, double radius) {
		circle (xc, yc, radius / 2.0);
		render (GamepadButtonType.HOME);
		circle (xc - radius, yc, radius / 3.0);
		render (GamepadButtonType.SELECT);
		circle (xc + radius, yc, radius / 3.0);
		render (GamepadButtonType.START);
	}

	private void shoulders (double xc, double yc, double w, double h, double h_shift, double v_shift) {
		double h_spacing = h_shift - w / 2.0;
		double v_spacing = v_shift - h / 2.0;

		cr.rectangle (xc - h_spacing - w, yc + v_spacing, w, h);
		render (GamepadButtonType.SHOULDER_L);

		cr.rectangle (xc + h_spacing, yc + v_spacing, w, h);
		render (GamepadButtonType.SHOULDER_R);

		cr.rectangle (xc - h_spacing - w, yc - v_spacing - h, w, h);
		render (GamepadButtonType.SHOULDER_L2);

		cr.rectangle (xc + h_spacing, yc - v_spacing -h, w, h);
		render (GamepadButtonType.SHOULDER_R2);
	}

	private void joysticks (double xc, double yc, double radius, double spacing) {
		circle (xc - spacing/2.0, yc, radius);
		render (GamepadButtonType.STICK_L);
		circle (xc + spacing/2.0, yc, radius);
		render (GamepadButtonType.STICK_R);
	}

	private void circle (double xc, double yc, double radius) {
		cr.arc (xc, yc, radius, 0, TAU);
	}
}

}

