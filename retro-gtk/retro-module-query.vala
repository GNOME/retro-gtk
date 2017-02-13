// This file is part of Retro. License: GPLv3

namespace Retro.ModuleQuery {
	public delegate bool CoreDescriptorQueryCallback (CoreDescriptor core_descriptor);

	private const string ENV_PLUGIN_PATH = "LIBRETRO_PLUGIN_PATH";

	public void foreach_core_descriptor (CoreDescriptorQueryCallback callback) {
		foreach (var path in get_plugin_lookup_paths ()) {
			var directory = File.new_for_path (path);
			if (!directory.query_exists ())
				continue;

			if (directory.query_file_type (FileQueryInfoFlags.NOFOLLOW_SYMLINKS) != FileType.DIRECTORY)
				continue;

			bool stop = false;
			foreach_child (directory, (info) => {
				var core_descriptor_basename = info.get_name ();
				if (!core_descriptor_basename.has_suffix (".libretro"))
					return true;

				var core_descriptor_file = directory.get_child (core_descriptor_basename);
				var core_descriptor_path = core_descriptor_file.get_path ();
				CoreDescriptor core_descriptor;
				try {
					core_descriptor = new CoreDescriptor (core_descriptor_path);
				}
				catch (Error e) {
					debug (e.message);

					return true;
				}

				stop = callback (core_descriptor);
				if (!stop)
					return true;

				return false;
			});

			if (stop)
				break;
		}
	}

	private string[] get_plugin_lookup_paths () {
		var envp = Environ.@get ();
		var env_plugin_path = Environ.get_variable (envp, ENV_PLUGIN_PATH);
		if (env_plugin_path == null)
			return Config.RETRO_PLUGIN_PATH.split (":");

		return @"$env_plugin_path:$(Config.RETRO_PLUGIN_PATH)".split (":");
	}

	private delegate bool ForeachChildFunc (FileInfo child_info);
	private void foreach_child (File file, ForeachChildFunc child_info_callback) {
		try {
			var enumerator = file.enumerate_children ("", FileQueryInfoFlags.NOFOLLOW_SYMLINKS);
			for (var info = enumerator.next_file () ; info != null && child_info_callback (info) ; info = enumerator.next_file ());
		}
		catch (Error e) {
			debug (e.message);
		}
	}
}

