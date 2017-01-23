// This file is part of RetroGtk. License: GPLv3

using PulseAudio;

using Retro;

namespace RetroGtk {

public class PaPlayer : GLib.Object {
	private Retro.Core core;
	private ulong on_audio_output_id;
	private double sample_rate;
	private Simple simple;

	public void set_core (Retro.Core core) {
		if (this.core != null)
			this.core.disconnect (on_audio_output_id);

		this.core = core;
		on_audio_output_id = core.audio_output.connect (on_audio_output);
		simple = null;
	}

	private void on_audio_output (int16[] data, double sample_rate) {
		if (simple == null || sample_rate != this.sample_rate)
			prepare_for_sample_rate (sample_rate);

		simple.write (data, sizeof (int16) * data.length);
	}

	private void prepare_for_sample_rate (double sample_rate) {
		this.sample_rate = sample_rate;

		var sample_spec = SampleSpec() {
			format = SampleFormat.S16NE,
			rate = (uint32) sample_rate,
			channels = 2
		};
		simple = new Simple (null, null, Stream.Direction.PLAYBACK,
		                     null, "", sample_spec, null, null,
		                     null);
	}
}

}

