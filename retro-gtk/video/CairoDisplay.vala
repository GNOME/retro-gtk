/* Copyright (C) 2014  Adrien Plazas
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

using Cairo;

using Retro;

namespace RetroGtk {

public class CairoDisplay : Gtk.DrawingArea, Video, Display {
	private Core _core;
	public Core core {
		get { return _core; }
		set {
			if (_core == value) return;

			_core = value;
			pixel_format = PixelFormat.ORGB1555;

			if (_core != null && _core.video_interface != this)
				_core.video_interface = this;
		}
	}

	private Mutex surface_mutex = new Mutex ();
	private ImageSurface surface;
	private bool show_surface;

	/*
	 * Pixbuf used to render some video formats
	 * It must be kept valid until while the surface using it is used.
	 */
	private Gdk.Pixbuf pb;

	public Rotation rotation { get; set; default = Rotation.NONE; }
	public bool overscan { get; set; default = false; }
	public bool can_dupe { get; set; default = true; }
	public Retro.PixelFormat pixel_format { get; set; default = PixelFormat.ORGB1555; }

	construct {
		show_surface = true;
	}

	[CCode (cname = "video_to_argb8888_pixbuf", cheader_filename="video-converter.h")]
	static extern Gdk.Pixbuf video_to_pixbuf ([CCode (array_length = false)] uint8[] data, uint width, uint height, size_t pitch, Retro.PixelFormat pixel_format);

	public void render (uint8[] data, uint width, uint height, size_t pitch) {
		if (data == null) return; // Dupe a frame

		surface_mutex.lock ();
		switch (pixel_format) {
			case PixelFormat.ORGB1555:
				pb = video_to_pixbuf (data, width, height, pitch, pixel_format);
				surface = new ImageSurface.for_data (
					(uchar[]) pb.get_pixels_with_length (), Format.RGB24,
					pb.width, pb.height, Format.RGB24.stride_for_width (pb.width)
				);
				break;

			case PixelFormat.XRGB8888:
				pb = null;
				surface = new ImageSurface.for_data (
					(uchar[]) data, Format.ARGB32,
					(int) width, (int) height, (int) pitch
				);
				break;

			case PixelFormat.RGB565:
				pb = null;
				surface = new ImageSurface.for_data (
					(uchar[]) data, Format.RGB16_565,
					(int) width, (int) height, (int) pitch
				);
				break;

			default:
				stderr.printf ("Error: Unkown video format\n");
				break;
		}

		queue_draw ();
		surface_mutex.unlock ();
	}

	public void show_video () {
		show_surface = true;
		queue_draw ();
	}

	public void hide_video () {
		show_surface = false;
		queue_draw ();
	}

	public override bool draw (Context cr) {
		// Paint the background black
		cr.set_source_rgb (0, 0, 0);
		cr.paint ();

		// Paint the video
		if (surface != null && show_surface) {
			surface_mutex.lock ();

			double w, h, x, y;
			get_video_box (out w, out h, out x, out y);
			double xs = w / surface.get_width ();
			double ys = h / surface.get_height ();

			cr.scale (xs, ys);
			cr.set_source_surface (surface, x/xs, y/ys);
			cr.paint ();

			surface_mutex.unlock ();
		}

		return false;
	}

	private void get_video_box (out double width, out double height, out double x, out double y) {
		var w = (double) get_allocated_width ();
		var h = (double) get_allocated_height ();

		// Set the size of the display
		double display_ratio = (double) core.av_info.aspect_ratio;
		double allocated_ratio = w / h;

		// If the screen is wider than the video...
		if (allocated_ratio > display_ratio) {
			height = h;
			width = (double) (h * display_ratio);
		}
		else {
			width = w;
			height = (double) (w / display_ratio);
		}

		// Set the position of the display
		x = (w - width) / 2;
		y = (h - height) / 2;
	}
}

}

