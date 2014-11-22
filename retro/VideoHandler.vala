namespace Retro {

public interface VideoHandler : Object {
	public abstract void video_refresh_cb (uint8[] data, uint width, uint height, size_t pitch);
}

}
