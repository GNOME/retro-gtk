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

using Retro;
using Flicky;

using Gtk;

public class Window : Gtk.Window {
	private HashTable<string, string> module_for_ext;

	private enum UiState {
		EMPTY,
		HAS_CORE,
		HAS_GAME
	}
	
	private Gtk.HeaderBar header;
	private KeyboardBox kb_box;
	private Display game_screen;
	
	private Gtk.Image play_image;
	private Gtk.Image pause_image;
	
	private Gtk.Button open_game_button;
	private Gtk.Button start_button;
	private Gtk.Button stop_button;
	private Gtk.MenuButton properties_button;
	private Gtk.Popover popover;
	private Gtk.Widget grid;
	
	private AudioDevice audio_dev;
	
	private Engine engine;
	private Runner runner;
	private bool running { set; get; default = false; }
	
	public Window (string[] modules) {
		module_for_ext = new HashTable<string, string> (str_hash, str_equal);

		foreach (var module in modules) {
			var core = new Engine (module);
			var exts = core.info.valid_extensions.split ("|");
			foreach (var ext in exts) module_for_ext[ext] = module;
		}

		engine = null;
		
		header = new Gtk.HeaderBar ();
		kb_box = new KeyboardBox ();
		game_screen = new Display ();
		
		open_game_button = new Gtk.Button.from_icon_name ("document-open-symbolic", Gtk.IconSize.SMALL_TOOLBAR);
		start_button = new Gtk.Button ();
		stop_button = new Gtk.Button.from_icon_name ("media-skip-backward-symbolic", Gtk.IconSize.SMALL_TOOLBAR);
		properties_button = new Gtk.MenuButton ();
		popover = new Gtk.Popover (properties_button);
		
		set_titlebar (header);
		add (kb_box);
		kb_box.add (game_screen);
		
		header.pack_start (open_game_button);
		header.pack_start (start_button);
		header.pack_start (stop_button);
		header.pack_end (properties_button);
		
		header.set_show_close_button (true);
		
		open_game_button.clicked.connect (on_open_game_button_clicked);
		start_button.clicked.connect (on_start_button_clicked);
		stop_button.clicked.connect (on_stop_button_clicked);
		properties_button.clicked.connect (on_properties_button_clicked);
		
		audio_dev = new AudioDevice ();
		
		header.show ();
		kb_box.show ();
		game_screen.show ();
		
		open_game_button.show ();
		start_button.show ();
		stop_button.show ();
		properties_button.show ();
		
		set_ui_state (UiState.EMPTY);
		
		play_image = new Image.from_icon_name ("media-playback-start-symbolic", Gtk.IconSize.SMALL_TOOLBAR);
		pause_image = new Image.from_icon_name ("media-playback-pause-symbolic", Gtk.IconSize.SMALL_TOOLBAR);
		
		start_button.set_image (running ? pause_image : play_image);
		
		properties_button.set_popover (popover);
		
		set_size_request (800, 600);
	}
	
	void set_titles () {
		header.set_title (engine.info.library_name);
	}
	
	void on_open_game_button_clicked (Gtk.Button button) {
		var dialog = new Gtk.FileChooserDialog ("Open core", this, Gtk.FileChooserAction.OPEN, "_Cancel", ResponseType.CANCEL, "_Open", ResponseType.ACCEPT);
		
		var filter = new FileFilter ();
		filter.set_filter_name ("Valid games");
		foreach (var ext in module_for_ext.get_keys ()) {
			filter.add_pattern ("*." + ext);
		}
		dialog.add_filter (filter);
		
		if (dialog.run () == Gtk.ResponseType.ACCEPT) {
			set_game (dialog.get_filename ());
		}
		
		dialog.destroy ();
	}
	
	void on_start_button_clicked (Gtk.Button button) {
		if (running) {
			runner.stop ();
			running = false;
			start_button.set_image (play_image);
			game_screen.hide_texture ();
		}
		else {
			runner.start ();
			running = true;
			start_button.set_image (pause_image);
			game_screen.show_texture ();
		}
	}
	
	void on_stop_button_clicked (Gtk.Button button) {
		runner.reset ();
	}
	
	void on_properties_button_clicked (Gtk.Button button) {
		if (grid != null) popover.remove (grid);
		
		grid = new OptionsGrid (engine.options);
		grid.show_all ();
		
		popover.add (grid);
	}
	
	private void set_engine (string path) {
		if (runner != null) {
			runner.stop ();
			runner = null;
		}
		
		engine = new Engine(path);
		
		engine.video_refresh.connect ((pb) => {
			game_screen.render (pb);
		});
		
		engine.audio_refresh.connect ((audio_samples) => {
			audio_dev.play (audio_samples.get_samples ());
			// TODO add a way to set the sample rate of the audio device
		});
		
		engine.set_controller_device (0, kb_box);
		
		runner = new Runner (engine);
		open_game_button.show ();
		set_titles ();
		
		set_ui_state (UiState.HAS_CORE);
	}
	
	private void set_game (string path) {
		var split = path.split(".");
		var ext = split[split.length -1];

		if (! module_for_ext.contains (ext)) return;

		set_engine (module_for_ext.lookup (ext));

		engine.load_game (engine.info.need_fullpath ? GameInfo (path) : GameInfo.with_data (path));
		
		header.set_subtitle (File.new_for_path (path).get_basename ());
		
		set_ui_state (UiState.HAS_GAME);
		
		start_button.clicked ();
	}
	
	private void set_ui_state (UiState ui_state) {
		switch (ui_state) {
			case UiState.EMPTY:
				start_button.hide ();
				stop_button.hide ();
				properties_button.hide ();
				break;
			case UiState.HAS_CORE:
				start_button.hide ();
				stop_button.hide ();
				properties_button.hide ();
				break;
			case UiState.HAS_GAME:
				start_button.show ();
				stop_button.show ();
				properties_button.show ();
				break;
		}
	}
}

