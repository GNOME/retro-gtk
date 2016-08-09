// This file is part of Retro. License: GPLv3

[CCode (gir_namespace = "Retro", gir_version = "0.6")]
namespace Retro.ModuleQuery {
	public delegate bool ModuleInfoQueryCallback (HashTable<string, string> module_info);

	private const string ENV_PLUGIN_PATH = "RETRO_PLUGIN_PATH_1_0";

	public string? lookup_module_for_basename (string basename) throws Error {
		foreach (var path in get_plugin_lookup_paths ()) {
			var directory = File.new_for_path (path);
			var file = directory.get_child (basename);
			if (!file.query_exists ())
				continue;

			return file.get_path ();
		}

		return null;
	}

	public string? lookup_module_for_info (ModuleInfoQueryCallback callback) throws Error {
		foreach (var path in get_plugin_lookup_paths ()) {
			var directory = File.new_for_path (path);
			var enumerator = directory.enumerate_children ("", FileQueryInfoFlags.NOFOLLOW_SYMLINKS);
			for (var info = enumerator.next_file () ; info != null ; info = enumerator.next_file ()) {
				var module_info_basename = info.get_name ();
				if (!module_info_basename.has_suffix ("_libretro.info"))
					continue;

				// Replace the "info" extension by "so".
				var module_basename = module_info_basename[0:module_info_basename.length - 4] + "so";
				var module_file = directory.get_child (module_basename);
				if (!module_file.query_exists ())
					continue;

				if (info.get_file_type () == FileType.DIRECTORY)
					continue;

				var module_info_file = directory.get_child (module_info_basename);
				var module_info = get_module_info (module_info_file);
				if (!callback (module_info))
					continue;

				return module_file.get_path ();
			}
		}

		return null;
	}

	private string[] get_plugin_lookup_paths () {
		var envp = Environ.@get ();
		var env_plugin_path = Environ.get_variable (envp, ENV_PLUGIN_PATH);
		if (env_plugin_path == null)
			return Config.RETRO_PLUGIN_PATH.split (":");

		return @"$env_plugin_path:$(Config.RETRO_PLUGIN_PATH)".split (":");
	}

	private HashTable<string, string> get_module_info (File module_info_file) {
		var module_info = new HashTable<string, string> (str_hash, str_equal);

		var dis = new DataInputStream (module_info_file.read ());
		for (var line = dis.read_line (null) ; line != null ; line = dis.read_line (null)) {
			var splitted_line = line.split ("=", 2);
			if (splitted_line.length != 2)
				continue;

			var key = splitted_line[0].strip ();
			var value = splitted_line[1].strip ();
			if (value.has_prefix ("\"") && value.has_suffix ("\"") && value.length >= 2)
				// Remove the quotes.
				module_info[key] = value[1:value.length - 1];
			else
				module_info[key] = value;
		}

		return module_info;
	}
}

