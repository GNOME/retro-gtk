// This file is part of Retro. License: GPLv3

[CCode (gir_namespace = "Retro", gir_version = "0.6")]
namespace Retro {

/**
 * The version of Libretro implemented by Retro.
 */
public static const uint API_VERSION = 1;

private const string ENV_PLUGIN_PATH = "RETRO_PLUGIN_PATH_1_0";

/**
 * Error type thrown by interfaces when accessing one of their Core's
 * callback.
 */
public errordomain CbError {
	NO_CORE,
	NO_CALLBACK
}

/**
 * Gets a module's system informations in a simple and light way.
 *
 * @param module_name the file name of the module to get the system
 * informations
 * @return the system infos of the module or //null//
 */
public SystemInfo? get_system_info (string module_name) {
	var module = GLib.Module.open (module_name, ModuleFlags.BIND_LAZY | ModuleFlags.BIND_LOCAL);

	void *function;
	module.symbol ("retro_get_system_info", out function);

	if (function == null) return null;

	unowned SystemInfo info;
	((GetSystemInfo) function) (out info);

	return info;
}

}

