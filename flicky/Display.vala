/* Flicky  Building blocks for a Retro frontend.
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

namespace Flicky {

public class Display : GtkClutter.Embed {
	private Texture texture;
	
	construct {
		var stage = get_stage ();
		
		stage.set_background_color (Color.get_static (StaticColor.BLACK));
		
		texture = new Texture ();
		show_texture ();
		stage.add_actor (texture);
		
		size_allocate.connect (on_size_allocate);
	}
	
	private void show_texture () {
		texture.set_opacity (0xff);
	}
	
	private void hide_texture () {
		texture.set_opacity (0x00);
	}
	
	public void render (Gdk.Pixbuf picture) {
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
}

}

