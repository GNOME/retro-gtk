// This file is part of RetroGtk. License: GPLv3

using PulseAudio;

using Retro;

namespace RetroGtk {

public class PaPlayer : GLib.Object, Retro.Audio {
	private ulong av_info_sig = 0;
	private ulong init_sig = 0;

	private Core _core;
	public Core core {
		get { return _core; }
		set {
			if (_core == value) return;

			if (_core != null && av_info_sig != 0) {
				_core.disconnect (av_info_sig);
				_core.disconnect (init_sig);
				av_info_sig = 0;
			}

			_core = value;

			if (_core != null) {
				update_sample_rate ();
				av_info_sig = _core.notify["av-info"].connect_after (update_sample_rate);

				if (_core.audio_interface != this)
					_core.audio_interface = this;
			}
		}
	}

	private Simple simple;

	public PaPlayer (uint32 sample_rate = 44100) {
		Object (sample_rate: sample_rate);
	}

	construct {
		var sample_spec = SampleSpec() {
			format = SampleFormat.S16NE,
			rate = sample_rate,
			channels = 2
		};
		simple = new Simple (null, null, Stream.Direction.PLAYBACK,
		                     null, "", sample_spec, null, null,
		                     null);
	}

	private uint32 _sample_rate;
	public uint32 sample_rate {
		get { return _sample_rate; }
		set {
			if (_sample_rate == value) return;

			_sample_rate = value;

			var sample_spec = SampleSpec() {
				format = SampleFormat.S16NE,
				rate = value,
				channels = 2
			};
			simple = new Simple (null, null, Stream.Direction.PLAYBACK,
			                     null, "", sample_spec, null, null,
			                     null);
		}
		default = 44100;
	}

	private void play_sample (int16 left, int16 right) {
		int16[] data = { left, right };
		simple.write (data, sizeof (int16) * data.length);
	}

	private size_t play_batch (int16[] data, size_t frames) {
		simple.write (data, sizeof (int16) * data.length);

		return 0;
	}

	private void update_sample_rate () {
		if (core == null) return;

		var info = _core.av_info;
		if (info != null) sample_rate = (uint32) info.sample_rate;
	}
}

}

