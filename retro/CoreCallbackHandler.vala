namespace Retro {

public interface CoreCallbackHandler : Object {
	public abstract void video_refresh_cb (uint8[] data, uint width, uint height, size_t pitch);
	public abstract void audio_sample_cb (int16 left, int16 right);
	public abstract size_t audio_sample_batch_cb (int16[] data, size_t frames);
	public abstract void input_poll_cb ();
	public abstract int16 input_state_cb (uint port, DeviceType device, uint index, uint id);
}

}
