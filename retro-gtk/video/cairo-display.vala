// This file is part of RetroGtk. License: GPLv3

public class RetroGtk.CairoDisplay : Gtk.DrawingArea, Retro.Video, Display {
	private Retro.Core _core;
	public Retro.Core core {
		get { return _core; }
		set {
			if (value == core)
				return;

			_core = value;
			pixel_format = Retro.PixelFormat.ORGB1555;

			if (core != null && core.video_interface != this)
				core.video_interface = this;
		}
	}

	public Gdk.Pixbuf pixbuf { set; get; }

	public Retro.Rotation rotation { get; set; default = Retro.Rotation.NONE; }
	public bool overscan { get; set; default = false; }
	public bool can_dupe { get; set; default = true; }
	public Retro.PixelFormat pixel_format { get; set; default = Retro.PixelFormat.ORGB1555; }

	private bool show_surface;

	construct {
		show_surface = true;

		notify["pixbuf"].connect (queue_draw);
	}

	[CCode (cname = "video_to_pixbuf", cheader_filename="video-converter.h")]
	static extern Gdk.Pixbuf video_to_pixbuf ([CCode (array_length = false)] uint8[] data, uint width, uint height, size_t pitch, Retro.PixelFormat pixel_format);

	public void render (uint8[] data, uint width, uint height, size_t pitch) {
		if (data == null)
			return; // Dupe a frame

		pixbuf = video_to_pixbuf (data, width, height, pitch, pixel_format);
	}

	public void show_video () {
		show_surface = true;
		queue_draw ();
	}

	public void hide_video () {
		show_surface = false;
		queue_draw ();
	}

	public override bool draw (Cairo.Context cr) {
		draw_background (cr);

		if (!show_surface)
			return false;

		var to_draw = pixbuf;
		if (to_draw == null)
			return false;

		var surface = Gdk.cairo_surface_create_from_pixbuf (to_draw, 1, null);
		double w, h, x, y;
		get_video_box (out w, out h, out x, out y);
		double xs = w / to_draw.width;
		double ys = h / to_draw.height;

		cr.scale (xs, ys);
		cr.set_source_surface (surface, x/xs, y/ys);
		cr.paint ();

		return true;
	}

	private void draw_background (Cairo.Context cr) {
		cr.set_source_rgb (0, 0, 0);
		cr.paint ();
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
