// This file is part of RetroGtk. License: GPLv3

using Retro;

namespace RetroGtk {

public interface Display : Gtk.Widget, Video {
	public abstract void show_video ();
	public abstract void hide_video ();
}

}

