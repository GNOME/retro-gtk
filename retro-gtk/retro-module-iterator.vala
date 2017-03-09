// This file is part of retro-gtk. License: GPLv3

public class Retro.ModuleIterator {
	private string[] directories;
	private int current_directory;
	private FileEnumerator file_enumerator;
	private CoreDescriptor? core_descriptor;

	internal ModuleIterator (string[] lookup_paths) {
		directories = lookup_paths;
	}

	public new CoreDescriptor? get () {
		return core_descriptor;
	}

	public bool next () {
		while (current_directory < directories.length) {
			try {
				if (next_in_current_path ())
					return true;
			}
			catch (Error e) {
				debug (e.message);
			}

			current_directory++;
		}

		file_enumerator = null;
		core_descriptor = null;

		return false;
	}

	private bool next_in_current_path () throws Error {
		var directory = File.new_for_path (directories[current_directory]);

		if (file_enumerator == null)
			file_enumerator = directory.enumerate_children ("", FileQueryInfoFlags.NOFOLLOW_SYMLINKS);

		if (file_enumerator == null)
			return false;

		for (var info = file_enumerator.next_file () ; info != null ; info = file_enumerator.next_file ()) {
			var core_descriptor_basename = info.get_name ();
			if (!core_descriptor_basename.has_suffix (".libretro"))
				continue;

			var core_descriptor_file = directory.get_child (core_descriptor_basename);
			var core_descriptor_path = core_descriptor_file.get_path ();
			try {
				core_descriptor = new CoreDescriptor (core_descriptor_path);

				return true;
			}
			catch (Error e) {
				debug (e.message);
			}
		}

		file_enumerator = null;

		return false;
	}
}

