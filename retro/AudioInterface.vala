namespace Retro {

public interface AudioInterface : Object {
	public abstract weak Core core { get; set; }

	public abstract void play_sample (int16 left, int16 right);
	public abstract size_t play_batch (int16[] data, size_t frames);
}

}
