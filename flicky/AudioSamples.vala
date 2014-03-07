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

public class AudioSamples : Object {
	private int16[] data;
	
	public AudioSamples (int16[] data) {
		this.data = data;
	}
	
	public AudioSamples.from_sample (int16 left, int16 right) {
		this.data = { left, right };
	}
	
	public int16[] get_samples () {
		return data;
	}
	
	public int get_size () {
		return data.length;
	}
	
	public int get_frames () {
		return data.length / 2;
	}
}

}

