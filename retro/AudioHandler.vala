namespace Retro {

public interface AudioHandler : Object {
	public abstract void audio_sample_cb (int16 left, int16 right);
	public abstract size_t audio_sample_batch_cb (int16[] data, size_t frames);
}

}
