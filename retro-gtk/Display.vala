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

using Clutter;

using Retro;

namespace RetroGtk {

public class Display : GtkClutter.Embed, Retro.VideoHandler {
	public weak Core _core;
	public weak Core core {
		get { return _core; }
		set {
			if (_core == value) return;

			_core = value;
			pixel_format = PixelFormat.ORGB1555;

			if (_core != null && _core.video_handler != this)
				_core.video_handler = this;
		}
	}

	private Texture texture;

	public Rotation rotation { get; set; default = Rotation.NONE; }
	public bool overscan { get; set; default = false; }
	public bool can_dupe { get; set; default = false; }
	public Retro.PixelFormat pixel_format { get; set; default = PixelFormat.ORGB1555; }

	construct {
		var stage = get_stage ();

		stage.set_background_color (Color.get_static (StaticColor.BLACK));

		texture = new Texture ();
		hide_texture ();
		stage.add_actor (texture);

		size_allocate.connect (on_size_allocate);
	}

	[CCode (cname = "video_to_pixbuf", cheader_filename="video-converter.h")]
	static extern Gdk.Pixbuf video_to_pixbuf ([CCode (array_length = false)] uint8[] data, uint width, uint height, size_t pitch, Retro.PixelFormat pixel_format);

	public void render (uint8[] data, uint width, uint height, size_t pitch) {
		var picture = video_to_pixbuf (data, width, height,  pitch, pixel_format);

		Cogl.TextureFlags flags = Cogl.TextureFlags.NO_AUTO_MIPMAP | Cogl.TextureFlags.NO_SLICING | Cogl.TextureFlags.NO_ATLAS;

		Cogl.PixelFormat format = Cogl.PixelFormat.RGB_888;
		Cogl.PixelFormat internal_format = Cogl.PixelFormat.RGB_888;

		Cogl.Texture cogl_tex = new Cogl.Texture.from_data (
			picture.width,
			picture.height,
			flags,
			format,
			internal_format,
			picture.rowstride,
			(uchar[]) picture.get_pixels_with_length ()
		);

		texture.set_cogl_texture (cogl_tex);
	}

//	public void set_pixel_format (PixelFormat pixel_format) {
//		this.pixel_format = pixel_format;
//	}

	private void on_size_allocate (Gtk.Allocation allocation) {
		double display_ratio = (double) 4 / 3;
		double allocated_ratio = (double) allocation.width / allocation.height;

		bool screen_is_wider = allocated_ratio > display_ratio;

		// Set the size of the display

		float w = 0;
		float h = 0;

		if (screen_is_wider) {
			h = (float) allocation.height;
			w = (float) (h * display_ratio);
		}
		else {
			w = (float) allocation.width;
			h = (float) (w / display_ratio);
		}

		texture.set_size (w, h);

		// Set the position of the display

		float x = 0;
		float y = 0;

		x = (allocation.width - w) / 2;
		y = (allocation.height - h) / 2;

		texture.set_position (x, y);
	}

	public void show_texture () {
		texture.visible = true;
	}

	public void hide_texture () {
		texture.visible = false;
	}
}

}

