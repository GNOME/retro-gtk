[CCode (cheader_filename = "libretro.h")]

namespace Retro {

namespace Audio {
	[CCode (cname = "retro_audio_callback_t")]
	public delegate void AudioCallback ();
	
	[CCode (cname = "retro_audio_set_state_callback_t")]
	public delegate void SetStateCallback (bool enabled);
	
	[CCode (cname = "struct retro_rumble_interface")]
	public struct Callback {
		AudioCallback    callback;
		SetStateCallback set_state;
	} 
}

}

