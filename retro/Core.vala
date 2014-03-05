/* Core.vala  Wraps a libretro implementation in an easy to use object.
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

namespace Retro {

class Core {
	// Callbacks for the libretro module
	
	public delegate bool   Environment      (Retro.Environment.Command cmd, void *data);
	public delegate void   VideoRefresh     ([CCode (array_length = false)] uint8[] data, uint width, uint height, size_t pitch);
	public delegate void   AudioSample      (int16 left, int16 right);
	public delegate size_t AudioSampleBatch (int16[] data, size_t frames);
	public delegate void   InputPoll        ();
	public delegate int16  InputState       (uint port, uint device, uint index, uint id);
	
	// Callbacks for the libretro module: end
	
	// Types of the module's functions
	
	[CCode (has_target = false)]
	private delegate void SetCallback (void *cb);
	
	[CCode (has_target = false)]
	private delegate void Init ();
	[CCode (has_target = false)]
	private delegate void Deinit ();
	
	[CCode (has_target = false)]
	private delegate uint ApiVersion ();
	
	[CCode (has_target = false)]
	private delegate void GetSystemInfo (out unowned SystemInfo info);
	[CCode (has_target = false)]
	private delegate void GetSystemAvInfo (out unowned SystemAvInfo info);
	
	[CCode (has_target = false)]
	private delegate void SetControllerPortDevice (uint port, Device device);
	
	[CCode (has_target = false)]
	private delegate void Reset ();
	[CCode (has_target = false)]
	private delegate void Run ();
	
	[CCode (has_target = false)]
	private delegate size_t SerializeSize ();
	[CCode (has_target = false)]
	private delegate bool Serialize (uint8[] data);
	[CCode (has_target = false)]
	private delegate bool Unserialize (uint8[] data);
	
	[CCode (has_target = false)]
	private delegate void CheatReset ();
	[CCode (has_target = false)]
	private delegate void CheatSet (uint index, bool enabled, string code);
	
	[CCode (has_target = false)]
	private delegate bool LoadGame (GameInfo game);
	[CCode (has_target = false)]
	private delegate bool LoadGameSpecial (GameType game_type, GameInfo info, size_t num_info);
	[CCode (has_target = false)]
	private delegate void UnloadGame ();
	
	[CCode (has_target = false)]
	private delegate Region GetRegion ();
	
	[CCode (has_target = false)]
	private delegate void *GetMemoryData (Memory id);
	[CCode (has_target = false)]
	private delegate size_t GetMemorySize (Memory id);
	
	// Types of the module's functions: end
	
	// Helper C methods
	
	private extern void set_global_self ();
	
	private extern void *get_real_environment_cb ();
	private extern void *get_real_video_refresh_cb ();
	private extern void *get_real_audio_sample_cb ();
	private extern void *get_real_audio_sample_batch_cb ();
	private extern void *get_real_input_poll_cb ();
	private extern void *get_real_input_state_cb ();
	
	// Helper C methods: end
	
	private Module module;
	
	// Module's functions
	
	private SetCallback _set_environment;
	private SetCallback _set_video_refresh;
	private SetCallback _set_audio_sample;
	private SetCallback _set_audio_sample_batch;
	private SetCallback _set_input_poll;
	private SetCallback _set_input_state;
	
	private Init _init;
	private Deinit _deinit;
	private ApiVersion _api_version;
	
	private GetSystemInfo _get_system_info;
	private GetSystemAvInfo _get_system_av_info;
	
	private SetControllerPortDevice _set_controller_port_device;
	
	private Reset _reset;
	private Run _run;
	
	private SerializeSize _serialize_size;
	private Serialize _serialize;
	private Unserialize _unserialize;
	
	private CheatReset _cheat_reset;
	private CheatSet _cheat_set;
	
	private LoadGame _load_game;
	private LoadGameSpecial _load_game_special;
	private UnloadGame _unload_game;
	
	private GetRegion _get_region;
	
	private GetMemoryData _get_memory_data;
	private GetMemorySize _get_memory_size;
	
	// Module's functions: end
	
	// Callback setters and getters
	
	private Environment _environment_cb;
	public Environment environment_cb {
		set {
			_environment_cb = value;
			
			set_global_self ();
			_set_environment (get_real_environment_cb ());
		}
		get {
			return _environment_cb;
		}
	}
	
	private VideoRefresh _video_refresh_cb;
	public VideoRefresh video_refresh_cb {
		set {
			_video_refresh_cb = value;
			
			set_global_self ();
			_set_video_refresh (get_real_video_refresh_cb ());
		}
		get {
			return _video_refresh_cb;
		}
	}
	
	private AudioSample _audio_sample_cb;
	public AudioSample audio_sample_cb {
		set {
			_audio_sample_cb = value;
			
			set_global_self ();
			_set_audio_sample (get_real_audio_sample_cb ());
		}
		get {
			return _audio_sample_cb;
		}
	}
	
	private AudioSampleBatch _audio_sample_batch_cb;
	public AudioSampleBatch audio_sample_batch_cb {
		set {
			_audio_sample_batch_cb = value;
			
			set_global_self ();
			_set_audio_sample_batch (get_real_audio_sample_batch_cb ());
		}
		get {
			return _audio_sample_batch_cb;
		}
	}
	
	private InputPoll _input_poll_cb;
	public InputPoll input_poll_cb {
		set {
			_input_poll_cb = value;
			
			set_global_self ();
			_set_input_poll (get_real_input_poll_cb ());
		}
		get {
			return _input_poll_cb;
		}
	}
	
	private InputState _input_state_cb;
	public InputState input_state_cb {
		set {
			_input_state_cb = value;
			
			set_global_self ();
			_set_input_state (get_real_input_state_cb ());
		}
		get {
			return _input_state_cb;
		}
	}
	
	// Callback setters and getters: end
	
	public Core (string file_name) {
		module = Module.open (file_name, ModuleFlags.BIND_LAZY | ModuleFlags.BIND_LOCAL);
		
		void *function;
		
		// Get the callback setters
		
		module.symbol ("retro_set_environment", out function);
		_set_environment = (SetCallback) function;
		module.symbol ("retro_set_video_refresh", out function);
		_set_video_refresh = (SetCallback) function;
		module.symbol ("retro_set_audio_sample", out function);
		_set_audio_sample = (SetCallback) function;
		module.symbol ("retro_set_audio_sample_batch", out function);
		_set_audio_sample_batch = (SetCallback) function;
		module.symbol ("retro_set_input_poll", out function);
		_set_input_poll = (SetCallback) function;
		module.symbol ("retro_set_input_state", out function);
		_set_input_state = (SetCallback) function;
		
		// Get the other functions
		
		module.symbol ("retro_init", out function);
		_init = (Init) function;
		module.symbol ("retro_deinit", out function);
		_deinit = (Deinit) function;
		module.symbol ("retro_api_version", out function);
		_api_version = (ApiVersion) function;
		module.symbol ("retro_get_system_info", out function);
		_get_system_info = (GetSystemInfo) function;
		module.symbol ("retro_get_system_av_info", out function);
		_get_system_av_info = (GetSystemAvInfo) function;
		module.symbol ("retro_set_controller_port_device", out function);
		_set_controller_port_device = (SetControllerPortDevice) function;
		module.symbol ("retro_reset", out function);
		_reset = (Reset) function;
		module.symbol ("retro_run", out function);
		_run = (Run) function;
		module.symbol ("retro_serialize_size", out function);
		_serialize_size = (SerializeSize) function;
		module.symbol ("retro_serialize", out function);
		_serialize = (Serialize) function;
		module.symbol ("retro_unserialize", out function);
		_unserialize = (Unserialize) function;
		module.symbol ("retro_cheat_reset", out function);
		_cheat_reset = (CheatReset) function;
		module.symbol ("retro_cheat_set", out function);
		_cheat_set = (CheatSet) function;
		module.symbol ("retro_load_game", out function);
		_load_game = (LoadGame) function;
		module.symbol ("retro_load_game_special", out function);
		_load_game_special = (LoadGameSpecial) function;
		module.symbol ("retro_unload_game", out function);
		_unload_game = (UnloadGame) function;
		module.symbol ("retro_get_region", out function);
		_get_region = (GetRegion) function;
		module.symbol ("retro_get_memory_data", out function);
		_get_memory_data = (GetMemoryData) function;
		module.symbol ("retro_get_memory_size", out function);
		_get_memory_size = (GetMemorySize) function;
	}
	
	~Core () {
		deinit ();
	}
	
	public void init () {
		set_global_self ();
		_init ();
	}
	
	public void deinit () {
		set_global_self ();
		_deinit ();
	}
	
	public uint api_version () {
		set_global_self ();
		return _api_version ();
	}
	
	public void get_system_info (out unowned SystemInfo info) {
		set_global_self ();
		_get_system_info (out info);
	}
	
	public void get_system_av_info (out unowned SystemAvInfo info) {
		set_global_self ();
		_get_system_av_info (out info);
	}
	
	public void set_controller_port_device (uint port, Device device) {
		set_global_self ();
		_set_controller_port_device (port, device);
	}
	
	public void reset () {
		set_global_self ();
		_reset ();
	}
	
	public void run () {
		set_global_self ();
		_run ();
	}
	
	public size_t serialize_size () {
		set_global_self ();
		return _serialize_size ();
	}
	
	public bool serialize (uint8[] data) {
		set_global_self ();
		return _serialize (data);
	}
	
	public bool unserialize (uint8[] data) {
		set_global_self ();
		return _unserialize (data);
	}
	
	public void cheat_reset () {
		set_global_self ();
		_cheat_reset ();
	}
	
	public void cheat_set (uint index, bool enabled, string code) {
		set_global_self ();
		_cheat_set (index, enabled, code);
	}
	
	public bool load_game (GameInfo game) {
		set_global_self ();
		return _load_game (game);
	}
	
	public bool load_game_special (GameType game_type, GameInfo info, size_t num_info) {
		set_global_self ();
		return _load_game_special (game_type, info, num_info);
	}
	
	public void unload_game () {
		set_global_self ();
		_unload_game ();
	}
	
	public Region get_region () {
		set_global_self ();
		return _get_region ();
	}
	
	public void *get_memory_data (Memory id) {
		set_global_self ();
		return _get_memory_data (id);
	}
	
	public size_t get_memory_size (Memory id) {
		set_global_self ();
		return _get_memory_size (id);
	}
}

}
