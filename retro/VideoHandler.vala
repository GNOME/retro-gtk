namespace Retro {

public interface VideoHandler : Object {
	public abstract weak Core core { get; set; }

	public abstract void render (uint8[] data, uint width, uint height, size_t pitch);
}

}
