// This file is part of retro-gtk. License: GPL-3.0+.

[CCode (gir_namespace = "Retro", gir_version = "0.12")]
namespace Retro {

/**
 * The version of Libretro implemented by Retro.
 */
public const uint API_VERSION = 1;

private const string ENV_PLUGIN_PATH = "RETRO_PLUGIN_PATH_1_0";

/**
 * Error type thrown by interfaces when accessing one of their Core's
 * callback.
 */
private errordomain CbError {
	NO_CORE,
	NO_CALLBACK
}

public extern void g_log (Core core, string log_domain, LogLevelFlags log_level, string message);

}

