namespace Retro {

public interface VideoHandler : Object {
	public abstract weak Core core { get; set; }

	public abstract void render (uint8[] data, uint width, uint height, size_t pitch);

	public abstract Rotation rotation { get; set; }
	public abstract bool overscan { get; set; }
	public abstract bool can_dupe { get; set; }
	public abstract PixelFormat pixel_format { get; set; }
}

}
