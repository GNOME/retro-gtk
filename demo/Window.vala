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

namespace Retro {

class Window : Gtk.Window {
	private Gtk.HeaderBar header;
	private Gtk.Image game_screen;
	private Gtk.Button properties_button;
	
	private Engine engine;
	
	private uint loop;
	
	public Window () {
		engine = null;
		
		header = new Gtk.HeaderBar ();
		game_screen = new Gtk.Image ();
		properties_button = new Gtk.Button.from_icon_name ("emblem-system-symbolic", Gtk.IconSize.SMALL_TOOLBAR);
		
		header.show ();
		game_screen.show ();
		properties_button.show ();
		
		set_titlebar (header);
		add (game_screen);
		
		header.pack_end (properties_button);
		
		header.set_show_close_button (true);
		
		properties_button.clicked.connect (on_properties_button_clicked);
	}
	
	~Window () {
		Source.remove (loop);
	}
	
	void on_properties_button_clicked (Gtk.Button button) {
		var dialog = new OptionsDialog (engine.variable_handler);
		dialog.show_all ();
	}
	
	public void set_engine (string path) {
		engine = new Engine(path);
		engine.init ();
		
		engine.video_refresh.connect ((pb) => {
			var pbx2 = pb.scale_simple (pb.get_width () * 2, pb.get_height () * 2, Gdk.InterpType.NEAREST);
			game_screen.set_from_pixbuf (pbx2);
		});
		
		var si = engine.get_system_info ();
		
		header.set_title (si.library_name);
	}
	
	public void set_game (string path) {
		var si = engine.get_system_info ();
		
		GameInfo game;
		set_game_info (out game, path, si.need_fullpath);
		
		bool loaded = engine.load_game (game);
		
		header.set_subtitle (File.new_for_path (path).get_basename ());
	}
	
	public void start () {
		loop = Timeout.add (100/6, run);
	}
	
	public bool run () {
		if (engine != null) {
			engine.run ();
			return true;
		}
		
		return false;
	}
}

}

