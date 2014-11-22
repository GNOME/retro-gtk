namespace Retro {

public interface VideoHandler : Object {
	public abstract void render (uint8[] data, uint width, uint height, size_t pitch);
}

}
