namespace Retro {

public interface InputHandler : Object {
	public abstract void input_poll_cb ();
	public abstract int16 input_state_cb (uint port, DeviceType device, uint index, uint id);
}

}
