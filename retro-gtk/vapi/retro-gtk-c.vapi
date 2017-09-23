// This file is part of retro-gtk. License: GPL-3.0+.

[CCode (cheader_filename = "retro-device-type.h")]
public enum Retro.DeviceType {
	TYPE_MASK = 0xff,
	NONE = 0,
	JOYPAD = 1,
	MOUSE = 2,
	KEYBOARD = 3,
	LIGHTGUN = 4,
	ANALOG = 5,
	POINTER = 6,
}

[CCode (cheader_filename = "retro-memory-type.h")]
public enum Retro.MemoryType {
	SAVE_RAM,
	RTC,
	SYSTEM_RAM,
	VIDEO_RAM,
}

[CCode (cheader_filename = "retro-pixel-format.h")]
public enum Retro.PixelFormat {
	XRGB1555,
	XRGB8888,
	RGB565,
	UNKNOWN = -1,
}

[CCode (cheader_filename = "retro-video-filter.h")]
public enum Retro.VideoFilter {
	SMOOTH,
	SHARP;

	public static VideoFilter from_string (string filter);
}

[CCode (cheader_filename = "retro-input-device.h")]
public interface Retro.InputDevice : GLib.Object {
	public abstract void  poll ();
	public abstract int16 get_input_state (DeviceType device, uint index, uint id);
	public abstract DeviceType get_device_type ();
	public abstract uint64 get_device_capabilities ();
}

[CCode (cheader_filename = "retro-input-descriptor.h")]
public struct Retro.InputDescriptor {
	uint port;
	DeviceType device;
	uint index;
	uint id;
	string description;
}

[CCode (cheader_filename = "retro-controller-iterator.h")]
public class Retro.ControllerIterator : GLib.Object {
	public extern bool next (out unowned uint? port, out unowned InputDevice controller);
}
