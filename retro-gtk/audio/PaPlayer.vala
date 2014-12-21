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

using PulseAudio;

using Retro;

namespace RetroGtk {

private class RingBuffer : Object {
	public size_t buffer_size { get; construct; }

	private uint8* buffer;
	private size_t start;
	private size_t written;
	private size_t end {
		get { return (start + written) % buffer_size; }
	}

	public RingBuffer (size_t size) {
		Object (buffer_size: size);
	}

	construct {
		buffer = malloc (buffer_size);
		start = written = 0;
	}

	~RingBuffer () {
		free (buffer);
	}

	public bool push (uint8* src, size_t size) {
		if (size > get_writable_size ())
			return false;

		if (end + size <= buffer_size) {
			Memory.copy (buffer + end, src, size);
			written += size;
		}
		else {
			size_t cpsize = buffer_size - end;
			push (src, cpsize);
			push (src + cpsize, size - cpsize);
		}

		return true;
	}

	public bool pop (uint8* dst, size_t size) {
		if (size > get_readable_size ())
			return false;

		if (start + size <= buffer_size) {
			Memory.copy (dst, buffer + start, size);
			start = (start + size) % buffer_size;
			written -= size;
		}
		else {
			size_t cpsize = buffer_size - start;
			pop (dst, cpsize);
			pop (dst + cpsize, size - cpsize);
		}

		return true;
	}

	public size_t get_readable_size () {
		return written;
	}

	public size_t get_writable_size () {
		return buffer_size - written;
	}
}

private class PaDevice : GLib.Object {
	private ThreadedMainLoop  loop;
	private Context           context;
	private SampleSpec        spec;
	private RingBuffer ring_buffer;

	private bool started;
	private Stream? stream = null;

	public PaDevice (uint32 sample_rate = 44100) {
		spec = SampleSpec () {
			format   = SampleFormat.S16NE,
			rate     = sample_rate,
			channels = 2
		};

		started = false;
	}

	construct {
		loop = new ThreadedMainLoop ();
		ring_buffer = new RingBuffer (88200);
	}

	private void start () {
		context = new Context (loop.get_api (), null);
		var context_flags = Context.Flags.NOFAIL;
		context.set_state_callback (cstate_cb);

		// Connect the context
		if (context.connect (null, context_flags, null) < 0) {
			stderr.printf ("Error: pa_context_connect () failed: %s\n", PulseAudio.strerror (context.errno ()));
		}

		loop.start ();

		started = true;
	}

	private void stop () {
		if (!started) return;

		loop.stop ();

		context = null;
		stream = null;

		started = false;
	}

	private void cstate_cb (Context context) {
		if (context.get_state () == Context.State.READY) {
			var attr = Stream.BufferAttr ();

			Stream.Flags flags = Stream.Flags.INTERPOLATE_TIMING | Stream.Flags.AUTO_TIMING_UPDATE | Stream.Flags.EARLY_REQUESTS;

			stream = new Stream (context, "", spec);

			size_t n_fragments = 12;

			size_t fragment_size = spec.bytes_per_second () / 2 / n_fragments;
			if (fragment_size < 1024)
				fragment_size = 1024;

			attr.maxlength = (uint32) (fragment_size * (n_fragments+1));
			attr.tlength = (uint32) (fragment_size * n_fragments);
			attr.prebuf = (uint32) fragment_size;
			attr.minreq = (uint32) fragment_size;

			stream.connect_playback (null, attr, flags, null, null);
		}
	}

	public void play (int16[] data) {
		if (!started) start ();

		if (stream != null) {
			ring_buffer.push ((uint8*) data, data.length * sizeof(int16));

			var attr = stream.get_buffer_attr ();
			if (attr != null) {
				size_t to_read = attr.minreq;
				if (ring_buffer.get_readable_size () >= to_read) {
					void* tmp_buffer = malloc (to_read);

					ring_buffer.pop (tmp_buffer, to_read);
					stream.write (tmp_buffer, to_read);

					free (tmp_buffer);
				}
			}
		}
	}

	public void set_sample_rate (uint32 sample_rate) {
		stop ();
		spec.rate = sample_rate;
	}
}

public class PaPlayer : GLib.Object, Retro.Audio {
	private ulong av_info_sig = 0;
	private ulong init_sig = 0;

	private weak Core _core;
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

	private PaDevice device;

	public PaPlayer (uint32 sample_rate = 44100) {
		Object (sample_rate: sample_rate);
	}

	construct {
		device = new PaDevice ();
	}

	private uint32 _sample_rate;
	public uint32 sample_rate {
		get { return _sample_rate; }
		set {
			if (_sample_rate == value) return;

			_sample_rate = value;

			device.set_sample_rate (value);
		}
		default = 44100;
	}

	private void play_sample (int16 left, int16 right) {
		if (device != null)
			device.play ({ left, right });
	}

	private size_t play_batch (int16[] data, size_t frames) {
		if (device != null)
			device.play (data);
		return 0;
	}

	private void update_sample_rate () {
		if (core == null) return;

		var info = _core.av_info;
		if (info != null) sample_rate = (uint32) info.sample_rate;
	}
}

}

