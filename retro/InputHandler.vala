namespace Retro {

public interface InputHandler : Object {
	public abstract void poll ();
	public abstract int16 get_state (uint port, DeviceType device, uint index, uint id);
}

}
