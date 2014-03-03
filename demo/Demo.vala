using Retro;
using Gtk;

extern void set_game_info (out GameInfo game, string path, bool need_fullpath);

class Demo : Object {
	public static int main (string[] argv) {
		if (argv.length < 3) return 1;
		
		Gtk.init (ref argv);
		
		var w = new Retro.Window ();
		w.show ();
		w.destroy.connect (()=>{
			Gtk.main_quit();
		});
		
		w.set_engine (argv[1]);
		w.set_game (argv[2]);
		
		w.start ();
		
		Gtk.main ();
		
		return 0;
}

}

