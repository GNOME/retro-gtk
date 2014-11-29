namespace Retro {

public interface InputInterface : Object {
	public abstract weak Core core { get; set; }

	public abstract void poll ();
	public abstract int16 get_state (uint port, DeviceType device, uint index, uint id);

	public abstract void set_descriptors (InputDescriptor[] input_descriptors);
	public abstract KeyboardCallback? keyboard_callback { set; get; }
	public abstract uint64 get_device_capabilities ();
}

}
