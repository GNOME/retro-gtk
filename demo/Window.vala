namespace Retro {

class Window : Gtk.Window {
	private Gtk.HeaderBar header;
	private Gtk.Image game_screen;
	
	private Engine engine;
	
	private uint loop;
	
	public Window () {
		engine = null;
		
		header = new Gtk.HeaderBar ();
		game_screen = new Gtk.Image ();
		
		header.show ();
		game_screen.show ();
		
		set_titlebar (header);
		add (game_screen);
		
		header.set_show_close_button (true);
	}
	
	~Window () {
		Source.remove (loop);
	}
	
	public void set_engine (string path) {
		engine = new Engine(path);
		engine.init ();
		
		engine.video_refresh.connect ((pb) => {
			var pbx2 = pb.scale_simple (pb.get_width () * 2, pb.get_height () * 2, Gdk.InterpType.NEAREST);
			game_screen.set_from_pixbuf (pbx2);
		});
	}
	
	public void set_game (string path) {
		GameInfo game;
		//set_game_info (out game, path, engine.core.need_fullpath);
		set_game_info (out game, path, false);
		
		bool loaded = engine.load_game (game);
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

