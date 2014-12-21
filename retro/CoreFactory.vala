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

namespace Retro {

public class CoreFactory : Object {
	private HashTable<string, Array<string>> module_for_ext;

	public Video video_interface { get; construct set; }
	public Audio audio_interface { get; construct set; }
	public Input input_interface { get; construct set; }
	public Variables variables_interface { get; construct set; }
	public Retro.Log log_interface { get; construct set; }

	public CoreFactory () {
		module_for_ext = new HashTable<string, Array<string>> (str_hash, str_equal);

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
			var core = new Core (module);

			init_interfaces ();

			core.variables_interface = variables_interface;
			core.log_interface = log_interface;

			core.video_interface = video_interface;
			core.audio_interface = audio_interface;
			core.input_interface = input_interface;

			core.init ();

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
					return core;
				}
			}
			catch (GLib.FileError e) {
				stderr.printf ("Error: %s\n", e.message);
			}
			catch (Retro.CbError e) {
				stderr.printf ("Error: %s\n", e.message);
			}
		}

		return null; // TODO warn
	}

	private void init_interfaces () {
		variables_interface.core = null;
		video_interface.core = null;
		audio_interface.core = null;
		input_interface.core = null;
	}
}

}

