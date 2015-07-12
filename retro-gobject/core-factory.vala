// This file is part of Retro. License: GPLv3

namespace Retro {

public class CoreFactory : Object {
	private HashTable<string, Array<string>> module_for_ext;

	private Regex module_base_name;

	public Video video_interface { get; construct set; }
	public Audio audio_interface { get; construct set; }
	public Input input_interface { get; construct set; }
	public Variables variables_interface { get; construct set; }
	public Retro.Log log_interface { get; construct set; }

	public CoreFactory () {
		module_for_ext = new HashTable<string, Array<string>> (str_hash, str_equal);

		module_base_name = /.*\/libretro-(.+?)\.so.*/;

		try {
			var dirpath = Config.PROJECT_PLUGINS_DIR;
			var directory = File.new_for_path (dirpath);
			var enumerator = directory.enumerate_children (FileAttribute.STANDARD_NAME, 0);

			FileInfo file_info;
			while ((file_info = enumerator.next_file ()) != null) {
				var name = file_info.get_name ();
				if (/libretro-.+\.so/.match (name))
					add_module (@"$dirpath/$name");
			}

		} catch (Error e) {
			stderr.printf ("Error: %s\n", e.message);
		}
	}

	private void add_module (string file_name) {
		var info = Retro.get_system_info (file_name);
		if (info == null) return;

		var exts = info.valid_extensions.split ("|");
		foreach (var ext in exts) {
			if (module_for_ext[ext] == null) module_for_ext[ext] = new Array<string> ();
			module_for_ext[ext].append_val (file_name);
		}
	}

	public List<weak string> get_valid_extensions () {
		return module_for_ext.get_keys ();
	}

	public Core? core_for_game (string game_name) {
		var split = game_name.split(".");
		var ext = split[split.length -1];

		if (! module_for_ext.contains (ext))
			return null; // TODO warn

		var modules = module_for_ext[ext];
		// Using foreach on modules.data display warnings

		for (uint i = 0 ; i < modules.length ; i ++) {
			var module = modules.index (i);
			var core = prepare_core (module);

			if (try_load_game (core, game_name))
				return core;
		}

		return null; // TODO warn
	}

	private Core prepare_core (string module) {
		var core = new Core (module);

		var config = Environment.get_user_config_dir ();
		var result = module_base_name.split (module);
		var libname = (result.length >= 2) ? result[1] : core.system_info.library_name;
		var libdir = @"$config/$(Config.PROJECT_DIR_NAME)/$libname";

		core.system_directory = @"$libdir/system";
		core.content_directory = @"$libdir/content";
		core.save_directory = @"$libdir/save";

//		variables_interface.core = null;
//		video_interface.core = null;
//		audio_interface.core = null;
//		input_interface.core = null;

		core.variables_interface = variables_interface;
		core.log_interface = log_interface;

		core.video_interface = video_interface;
		core.audio_interface = audio_interface;
		core.input_interface = input_interface;

		core.init ();

		return core;
	}

	private bool try_load_game (Core core, string game_name) {
		try {
			var fullpath = core.system_info.need_fullpath;
			if (core.load_game (fullpath ? GameInfo (game_name) : GameInfo.with_data (game_name))) {
				if (core.disk_control_interface != null) {
					var disk = core.disk_control_interface;

					disk.set_eject_state (true);

					while (disk.get_num_images () < 1)
						disk.add_image_index ();

					var index = disk.get_num_images () - 1;

					disk.replace_image_index (index, fullpath ? GameInfo (game_name) : GameInfo.with_data (game_name));

					disk.set_eject_state (false);
				}
				return true;
			}
		}
		catch (GLib.FileError e) {
			stderr.printf ("Error: %s\n", e.message);
		}
		catch (Retro.CbError e) {
			stderr.printf ("Error: %s\n", e.message);
		}

		return false;
	}
}

}

