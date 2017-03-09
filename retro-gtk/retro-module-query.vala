// This file is part of retro-gtk. License: GPLv3

public class Retro.ModuleQuery {
	private const string ENV_PLUGIN_PATH = "LIBRETRO_PLUGIN_PATH";

	private bool recursive;

	public ModuleQuery (bool recursive) {
		this.recursive = recursive;
	}

	public ModuleIterator iterator () {
		var paths = get_plugin_lookup_paths ();

		return new ModuleIterator (paths, recursive);
	}

	private static string[] get_plugin_lookup_paths () {
		var envp = Environ.@get ();
		var env_plugin_path = Environ.get_variable (envp, ENV_PLUGIN_PATH);
		if (env_plugin_path == null)
			return Config.RETRO_PLUGIN_PATH.split (":");

		return @"$env_plugin_path:$(Config.RETRO_PLUGIN_PATH)".split (":");
	}
}

