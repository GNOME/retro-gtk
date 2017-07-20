// This file is part of retro-gtk. License: GPL-3.0+.

public class Retro.CairoDisplay : Gtk.DrawingArea {
	// Because gdk-pixbuf saves dpi as integer we have to multiply it by big
	// enough number to represent aspect ratio precisely.
	private const float Y_DPI = 1000000.0f;

	public Gdk.Pixbuf pixbuf { set; get; }

	private Core core;
	private VideoFilter filter;
	private ulong on_video_output_id;
	private float aspect_ratio;
	private bool show_surface;

	construct {
		filter = VideoFilter.SMOOTH;
		show_surface = true;

		notify["sensitive"].connect (queue_draw);
		notify["pixbuf"].connect (queue_draw);
	}

	[CCode (cname = "gdk_pixbuf_new_from_video")]
	static extern Gdk.Pixbuf gdk_pixbuf_new_from_video ([CCode (array_length = false)] uint8[] data, uint width, uint height, size_t pitch, PixelFormat pixel_format);

	public void set_core (Core? core) {
		if (this.core != null)
			this.core.disconnect (on_video_output_id);

		this.core = core;

		if (this.core != null)
			on_video_output_id = core.video_output.connect (on_video_output);
	}

	public void set_filter (VideoFilter filter) {
		this.filter = filter;
		queue_draw ();
	}

	private void on_video_output (uint8[] data, uint width, uint height, size_t pitch, PixelFormat pixel_format, float aspect_ratio) {
		this.aspect_ratio = aspect_ratio;
		pixbuf = gdk_pixbuf_new_from_video (data, width, height, pitch, pixel_format);

		var x_dpi = aspect_ratio * Y_DPI;
		var x_dpi_string = x_dpi.to_string ();
		var y_dpi_string = Y_DPI.to_string ();

		pixbuf.set_option ("x-dpi", x_dpi_string);
		pixbuf.set_option ("y-dpi", y_dpi_string);
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

		if (!sensitive) {
			var desaturated = new Gdk.Pixbuf (Gdk.Colorspace.RGB, to_draw.has_alpha, 8, to_draw.width, to_draw.height);
			to_draw.saturate_and_pixelate (desaturated, 0.0f, false);
			to_draw = desaturated;
		}

		var surface = Gdk.cairo_surface_create_from_pixbuf (to_draw, 1, null);
		double w, h, x, y;
		get_video_box (out w, out h, out x, out y);
		double xs = w / to_draw.width;
		double ys = h / to_draw.height;

		cr.scale (xs, ys);
		cr.set_source_surface (surface, x/xs, y/ys);
		var source = cr.get_source ();
		switch (filter) {
		case VideoFilter.SHARP:
			source.set_filter (Cairo.Filter.NEAREST);

			break;
		case VideoFilter.SMOOTH:
		default:
			source.set_filter (Cairo.Filter.BILINEAR);

			break;
		}
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
		double display_ratio = (double) aspect_ratio;
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
