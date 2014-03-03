using PulseAudio;

public class AudioDevice : GLib.Object {
	private GLibMainLoop      loop;
	private Context           context;
	private Context.Flags     context_flags;
	private SampleSpec        spec;
	private Stream.BufferAttr attr;
	
	public uint32 sample_rate { construct; get; }
	
	private bool started;
	private Stream? stream = null;
	
	public AudioDevice (uint32 sample_rate = 44100) {
		Object (sample_rate: sample_rate);
	}

	construct {
		spec = SampleSpec () {
			format   = SampleFormat.S16NE,
			rate     = sample_rate,
			channels = 2
		};
		
		started = false;
	}

	private void start () {
		loop = new GLibMainLoop (); // there are other loops that can be used if you are not using glib/gtk main
		
		context = new Context (loop.get_api (), null);
		context_flags = Context.Flags.NOFAIL;
		context.set_state_callback (cstate_cb);
		
		// Connect the context
		if (context.connect ( null, context_flags, null) < 0) {
			print ( "pa_context_connect () failed: %s\n", PulseAudio.strerror (context.errno ()));
		}
		
		started = true;
	}
	
	// state callback, don't connect_playback until we are ready here.
	private void cstate_cb (Context context) {
		if (context.get_state () == Context.State.READY) {
			attr = Stream.BufferAttr ();
			
			size_t fragment_size = 0;
			size_t n_fragments   = 0;
			
			Stream.Flags flags = Stream.Flags.INTERPOLATE_TIMING | Stream.Flags.AUTO_TIMING_UPDATE | Stream.Flags.EARLY_REQUESTS;
			
			//Stream stream = new Stream (context, "", spec);
			stream = new Stream (context, "", spec);
			stream.set_overflow_callback (() => {
				print ("AudioDevice: stream overflow...\n");
			});
			stream.set_underflow_callback (() => {
				print ("AudioDevice: stream underflow...\n");
			});
			
			size_t fs = spec.frame_size ();
			// Don't fix things more than necessary
			if ( (fragment_size % fs) == 0 && n_fragments >= 2 && fragment_size > 0) {
				print ("something went wrong\n");
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
			
			print ("fragment_size: %s, n_fragments: %s, fs: %s\n", fragment_size.to_string (), n_fragments.to_string (), fs.to_string ());
			
			attr.maxlength = (uint32) (fragment_size * (n_fragments+1));
			attr.tlength = (uint32) (fragment_size * n_fragments);
			attr.prebuf = (uint32) fragment_size;
			attr.minreq = (uint32) fragment_size;
			
			int tmp = stream.connect_playback (null, attr, flags, null, null);
			if (tmp < 0 ) {
				print ("connect_playback returned %s\n", tmp.to_string ());
				print (": pa_stream_connect_playback () failed: %s\n", PulseAudio.strerror (context.errno () ));
			}
		}
	}

	public void play (int16[] data) {
		if (!started) start ();
		
		if (stream != null) {
			var buffer = (void *) data;
			var bytes  =  sizeof(int16) * data.length;
			
			stdout.printf ("writing %lu / %lu\n", bytes, stream.writable_size ());
			
			stream.write (buffer, bytes);
		}
	}
}

