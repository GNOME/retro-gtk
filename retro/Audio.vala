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
 * An interface to play a {@link Core}'s audio.
 */
public interface Audio : Object {
	/**
	 * The core to handle audio from.
	 */
	public abstract Core core { get; set; }

	/**
	 * Renders a single audio frame.
	 *
	 * Format is signed 16-bit native endian.
	 *
	 * @param left the left channel of the audio sample
	 * @param right the right channel of the audio sample
	 */
	public abstract void play_sample (int16 left, int16 right);

	/**
	 * Renders multiple audio frames.
	 *
	 * One frame is defined as a sample of left and right channels,
	 * interleaved.
	 *
	 * @param data the set of audio samples
	 * @param frames the number of samples
	 * @return FIXME
	 */
	public abstract size_t play_batch (int16[] data, size_t frames);

	/**
	 * Notifies the {@link core} that it can play audio.
	 *
	 * Used by {@link core} where audio can be asynchronous to let the
	 * frontend ask for audio to play. The audio data will then be sent via
	 * the usual methods.
	 *
	 * @throws CbError the {@link core} or its callback couldn't be found
	 */
	public void audio_request () throws CbError {
		if (core == null)
			throw new CbError.NO_CORE ("No core");

		if (core.audio_callback == null)
			throw new CbError.NO_CALLBACK ("No audio callback");

		core.audio_callback.callback ();
	}

	/**
	 * Notifies the {@link core} that the frontend can ask for audio to be
	 * played.
	 *
	 * It must be set to true before calling {@link audio_request}, and
	 * it must be set to false one the audio player don't want to request
	 * for audio anymore.
	 *
	 * @param enabled //true// to let frontend send audio requests, //false//
	 * otherwise
	 * @throws CbError the {@link core} or its callback couldn't be found
	 */
	public void can_request_audio (bool enabled) throws CbError {
		if (core == null)
			throw new CbError.NO_CORE ("No core");

		if (core.audio_callback == null)
			throw new CbError.NO_CALLBACK ("No audio callback");

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
