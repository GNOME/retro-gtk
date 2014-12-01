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

public interface Audio : Object {
	public abstract weak Core core { get; set; }

	/**
	 * Renders a single audio frame
	 *
	 * Format is signed 16-bit native endian.
	 */
	public abstract void play_sample (int16 left, int16 right);

	/**
	 * Renders multiple audio frames
	 *
	 * One frame is defined as a sample of left and right channels,
	 * interleaved.
	 */
	public abstract size_t play_batch (int16[] data, size_t frames);

	/**
	 * Notifies the Core that it can play audio
	 *
	 * Used by Core where audio can be asynchronous to let the frontend
	 * ask for audio to play. The audio data will then be sent via the
	 * usual methods.
	 */
	public virtual signal void audio_request () throws CbError {
		if (core == null)
			throw new InterfaceError.NO_CORE ("No core");

		if (core.audio_callback == null)
			throw new InterfaceError.NO_CALLBACK ("No audio callback");

		core.audio_callback.callback ();
	}

	/**
	 * Notifies the Core that the frontend can ask for audio to be played
	 *
	 * It must be set to true before calling {@link audio_request()}, and
	 * it must be set to false one the audio player don't want to request
	 * for audio anymore.
	 */
	public bool can_request_audio (bool enabled) throws CbError {
		if (core == null)
			throw new InterfaceError.NO_CORE ("No core");

		if (core.audio_callback == null)
			throw new InterfaceError.NO_CALLBACK ("No audio callback");

		core.audio_callback.set_state (enabled);
	}
}

[CCode (has_target = false)]
private delegate void AudioCallbackCallback ();

[CCode (has_target = false)]
private delegate void AudioCallbackSetState (bool enabled);

private struct AudioCallback {
	AudioCallbackCallback callback;
	AudioCallbackSetState set_state;
}

}
