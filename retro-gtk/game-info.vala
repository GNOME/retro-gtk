// This file is part of retro-gtk. License: GPL-3.0+.

namespace Retro {

/**
 * Stores information on a game.
 *
 * Used by a {@link Core} to load a game.
 *
 * If the core needs a full path to the game file, then it doesn't need the
 * data as it will load it itself: use the default constructor.
 *
 * If the store doesn't need a full path, the path still be saved as some
 * implementations still need it, and the file will be read and stored into the
 * data field.
 */
public struct GameInfo {
	/**
	 * Path to game, UTF-8 encoded.
	 *
	 * Usually used as a reference.
	 * May be null if rom was loaded from stdin or similar.
	 * {@link SystemInfo.need_fullpath} guaranteed that this path is
	 * valid.
	 */
	public string path;

	/**
	 * Memory buffer of loaded game.
	 *
	 * Will be null if {@link SystemInfo.need_fullpath} was set.
	 */
	[CCode (array_length_type = "gsize")]
	public uint8[] data;

	/**
	 * String of implementation specific meta-data.
	 *
	 * Deprecated in Libretro 1.0
	 */
	public string meta;

	/**
	 * Sets from a game file name.
	 *
	 * Use this constructor if the {@link Core} needs a full path to the
	 * game and not its data.
	 *
	 * @param file_name the game to store information from
	 */
	public GameInfo (string file_name) {
		path = file_name;
		data = {};
	}

	/**
	 * Sets from a game file name.
	 *
	 * Use this constructor if the {@link Core} needs the data of the
	 * game.
	 *
	 * @param file_name the game to store information from
	 * @throws FileError the file can't be read
	 */
	public GameInfo.with_data (string file_name) throws FileError {
		path = file_name;
		FileUtils.get_data (file_name, out data);
	}
}

}

