/* Flicky  Building blocks for a Retro frontend.
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

namespace Flicky {

/**
 * Audio samples storage.
 * 
 * Stores a batch of audio samples so that they can be shared through signals.
 * The samples are 16 bit, native endian, left-right interlaced PCM.
 * 
 * Their sample rate is also stored.
 */
public class AudioSamples : Object {
	private int16[] samples;
	private double sample_rate;
	
	public AudioSamples (int16[] samples, double sample_rate = 44100) {
		this.samples = samples;
		this.sample_rate = sample_rate;
	}
	
	public AudioSamples.from_sample (int16 left, int16 right, double sample_rate = 44100) {
		this.samples = { left, right };
		this.sample_rate = sample_rate;
	}
	
	public int16[] get_samples () {
		return samples;
	}
	
	public int get_size () {
		return samples.length;
	}
	
	public int get_frames () {
		return samples.length / 2;
	}
	
	public double get_sample_rate () {
		return sample_rate;
	}
}

}

