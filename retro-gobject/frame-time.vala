// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * TODO Change visibility once the interface have been tested.
 */
internal interface FrameTime: Object {
	public abstract int64 usec_reference { set; get; default = 0; }
	public abstract void callback (int64 usec);
}

private class CoreFrameTime: Object, FrameTime {
	[CCode (has_target = false)]
	internal delegate void FrameTimeCallback (int64 usec);

	internal struct Callback {
		FrameTimeCallback callback;
		int64             usec_reference;
	}

	public Callback callback_struct { construct; private get; }
	public int64 usec_reference { set; get; }

	internal CoreFrameTime (Callback callback_struct) {
		Object (callback_struct: callback_struct);
	}

	construct {
		notify["usec_reference"].connect (() => {
			callback_struct.usec_reference = usec_reference;
		});
	}

	public void callback (int64 usec) {
		if (callback_struct.callback != null) {
			callback_struct.callback (usec);
		}
	}
}

}

