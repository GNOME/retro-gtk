// This file is part of Retro. License: GPLv3

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
}

}
