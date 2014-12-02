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

private class AudioPlayer : GLib.Object {
	private ThreadedMainLoop  loop;
	private Context           context;
	private SampleSpec        spec;


	private bool started;
	private Stream? stream = null;

	public AudioPlayer (uint32 sample_rate = 44100) {
		spec = SampleSpec () {
			format   = SampleFormat.S16NE,
			rate     = sample_rate,
			channels = 2
		};

		started = false;
	}

	construct {
		loop = new ThreadedMainLoop ();
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

	// state callback, don't connect_playback until we are ready here.
	private void cstate_cb (Context context) {
		if (context.get_state () == Context.State.READY) {
			var attr = Stream.BufferAttr ();

			size_t fragment_size = 0;
			size_t n_fragments   = 0;

			Stream.Flags flags = Stream.Flags.INTERPOLATE_TIMING | Stream.Flags.AUTO_TIMING_UPDATE | Stream.Flags.EARLY_REQUESTS;

			//Stream stream = new Stream (context, "", spec);
			stream = new Stream (context, "", spec);
			stream.set_overflow_callback (() => {
				stderr.printf ("Error: Stream overflow...\n");
			});
			stream.set_underflow_callback (() => {
				stderr.printf ("Error: Stream underflow...\n");
			});

			size_t fs = spec.frame_size ();
			// Don't fix things more than necessary
			if ((fragment_size % fs) == 0 && n_fragments >= 2 && fragment_size > 0) {
				return ;
			}

			// Number of fragments set?
			if (n_fragments < 2) {
				if (fragment_size > 0) {
					n_fragments = (spec.bytes_per_second () / 2 / fragment_size);
					if (n_fragments < 2)
						n_fragments = 2;
				} else
					n_fragments = 12;
			}

			// Fragment size set?
			if (fragment_size <= 0) {
				fragment_size = spec.bytes_per_second () / 2 / n_fragments;
				if (fragment_size < 1024)
					fragment_size = 1024;
			}

			attr.maxlength = (uint32) (fragment_size * (n_fragments+1));
			attr.tlength = (uint32) (fragment_size * n_fragments);
			attr.prebuf = (uint32) fragment_size;
			attr.minreq = (uint32) fragment_size;

			int tmp = stream.connect_playback (null, attr, flags, null, null);
			if (tmp < 0 ) {
				stderr.printf ("Error: Connect_playback returned %s\n", tmp.to_string ());
				stderr.printf ("Error: pa_stream_connect_playback () failed: %s\n", PulseAudio.strerror (context.errno () ));
			}
		}
	}

	public void play (int16[] data) {
		if (!started) start ();

		if (stream != null) {
			var buffer = (void *) data;
			var bytes  =  sizeof(int16) * data.length;

			stream.write (buffer, bytes);
		}
	}

	public void set_sample_rate (uint32 sample_rate) {
		stop ();
		spec.rate = sample_rate;
	}

}

}

