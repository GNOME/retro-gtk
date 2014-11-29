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

/**
 * Loads a libretro module.
 */
private class Module : Object {
	/**
	 * The file name of the module.
	 */
	public string file_name { construct; get; }

	/**
	 * The dynamically loaded libretro module.
	 */
	private GLib.Module module;

	public CallbackSetter set_environment { get; private set; }
	public CallbackSetter set_video_refresh { get; private set; }
	public CallbackSetter set_audio_sample { get; private set; }
	public CallbackSetter set_audio_sample_batch { get; private set; }
	public CallbackSetter set_input_poll { get; private set; }
	public CallbackSetter set_input_state { get; private set; }

	public Init init { get; private set; }
	public Deinit deinit { get; private set; }
	public ApiVersion api_version { get; private set; }

	public GetSystemInfo get_system_info { get; private set; }
	public GetSystemAvInfo get_system_av_info { get; private set; }

	public SetControllerPortDevice set_controller_port_device { get; private set; }

	public Reset reset { get; private set; }
	public Run run { get; private set; }

	public SerializeSize serialize_size { get; private set; }
	public Serialize serialize { get; private set; }
	public Unserialize unserialize { get; private set; }

	public CheatReset cheat_reset { get; private set; }
	public CheatSet cheat_set { get; private set; }

	public LoadGame load_game { get; private set; }
	public LoadGameSpecial load_game_special { get; private set; }
	public UnloadGame unload_game { get; private set; }

	public GetRegion get_region { get; private set; }

	public GetMemoryData get_memory_data { get; private set; }
	public GetMemorySize get_memory_size { get; private set; }

	/**
	 * Create a Module from the file name of a libretro implementation.
	 *
	 * The file must be a dynamically loadable shared object implementing the
	 * same version of the libretro API as Retro.
	 *
	 * @param file_name the file name of the libretro implementation to load
	 */
	public Module (string file_name) {
		Object (file_name: file_name);
	}

	construct {
		module = GLib.Module.open (file_name, ModuleFlags.BIND_LAZY | ModuleFlags.BIND_LOCAL);

		void *function;

		// Get the callback setters from the module

		module.symbol ("retro_set_environment", out function);
		set_environment = (CallbackSetter) function;
		module.symbol ("retro_set_video_refresh", out function);
		set_video_refresh = (CallbackSetter) function;
		module.symbol ("retro_set_audio_sample", out function);
		set_audio_sample = (CallbackSetter) function;
		module.symbol ("retro_set_audio_sample_batch", out function);
		set_audio_sample_batch = (CallbackSetter) function;
		module.symbol ("retro_set_input_poll", out function);
		set_input_poll = (CallbackSetter) function;
		module.symbol ("retro_set_input_state", out function);
		set_input_state = (CallbackSetter) function;

		// Get the other functions from the module

		module.symbol ("retro_init", out function);
		init = (Init) function;
		module.symbol ("retro_deinit", out function);
		deinit = (Deinit) function;
		module.symbol ("retro_api_version", out function);
		api_version = (ApiVersion) function;
		module.symbol ("retro_get_system_info", out function);
		get_system_info = (GetSystemInfo) function;
		module.symbol ("retro_get_system_av_info", out function);
		get_system_av_info = (GetSystemAvInfo) function;
		module.symbol ("retro_set_controller_port_device", out function);
		set_controller_port_device = (SetControllerPortDevice) function;
		module.symbol ("retro_reset", out function);
		reset = (Reset) function;
		module.symbol ("retro_run", out function);
		run = (Run) function;
		module.symbol ("retro_serialize_size", out function);
		serialize_size = (SerializeSize) function;
		module.symbol ("retro_serialize", out function);
		serialize = (Serialize) function;
		module.symbol ("retro_unserialize", out function);
		unserialize = (Unserialize) function;
		module.symbol ("retro_cheat_reset", out function);
		cheat_reset = (CheatReset) function;
		module.symbol ("retro_cheat_set", out function);
		cheat_set = (CheatSet) function;
		module.symbol ("retro_load_game", out function);
		load_game = (LoadGame) function;
		module.symbol ("retro_load_game_special", out function);
		load_game_special = (LoadGameSpecial) function;
		module.symbol ("retro_unload_game", out function);
		unload_game = (UnloadGame) function;
		module.symbol ("retro_get_region", out function);
		get_region = (GetRegion) function;
		module.symbol ("retro_get_memory_data", out function);
		get_memory_data = (GetMemoryData) function;
		module.symbol ("retro_get_memory_size", out function);
		get_memory_size = (GetMemorySize) function;
	}
}

}
