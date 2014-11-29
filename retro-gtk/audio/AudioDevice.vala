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

using Retro;

namespace RetroGtk {

public class AudioDevice : GLib.Object, Retro.Audio {
	private ulong av_info_sig = 0;
	private ulong init_sig = 0;

	public weak Core _core;
	public weak Core core {
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

	private AudioPlayer audio_player;

	public AudioDevice (uint32 sample_rate = 44100) {
		Object (sample_rate: sample_rate);
	}

	construct {
		audio_player = new AudioPlayer ();
	}

	private uint32 _sample_rate;
	public uint32 sample_rate {
		get { return _sample_rate; }
		set {
			if (_sample_rate == value) return;

			_sample_rate = value;

			audio_player.set_sample_rate (value);
		}
		default = 44100;
	}

	private void play_sample (int16 left, int16 right) {
		if (audio_player != null)
			audio_player.play ({ left, right });
	}

	private size_t play_batch (int16[] data, size_t frames) {
		if (audio_player != null)
			audio_player.play (data);
		return 0;
	}

	private void update_sample_rate () {
		if (core == null) return;

		var info = _core.av_info;
		if (info != null) sample_rate = (uint32) info.sample_rate;
	}
}

}

