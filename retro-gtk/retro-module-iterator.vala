// This file is part of retro-gtk. License: GPLv3

public class Retro.ModuleIterator {
	private string[] directories;
	private bool recursive;
	private int current_directory;
	private FileEnumerator file_enumerator;
	private CoreDescriptor? core_descriptor;
	private ModuleIterator? sub_directory;
	private GenericSet<string> visited;

	internal ModuleIterator (string[] lookup_paths, bool recursive) {
		directories = lookup_paths;
		this.recursive = recursive;
		visited = new GenericSet<string> (str_hash, str_equal);
	}

	private ModuleIterator.for_subdirectory (string lookup_path, GenericSet<string> visited_paths) {
		directories = { lookup_path };
		recursive = true;
		visited = visited_paths;
	}

	public new CoreDescriptor? get () {
		return core_descriptor;
	}

	public bool next () {
		while (current_directory < directories.length) {
			set_current_directory_as_visited ();

			try {
				if (next_in_current_path ())
					return true;
			}
			catch (Error e) {
				debug (e.message);
			}

			while (current_directory < directories.length && was_current_directory_visited ())
				current_directory++;
		}

		file_enumerator = null;
		core_descriptor = null;
		sub_directory = null;

		return false;
	}

	private bool next_in_current_path () throws Error {
		if (sub_directory != null && next_in_sub_directory ())
			return true;

		var directory = File.new_for_path (directories[current_directory]);

		if (file_enumerator == null)
			file_enumerator = directory.enumerate_children ("", FileQueryInfoFlags.NOFOLLOW_SYMLINKS);

		if (file_enumerator == null)
			return false;

		for (var info = file_enumerator.next_file () ; info != null ; info = file_enumerator.next_file ()) {
			if (recursive && info.get_file_type () == FileType.DIRECTORY && sub_directory == null) {
				var sub_directory_basename = info.get_name ();
				var sub_directory_file = directory.get_child (sub_directory_basename);
				var sub_directory_path = sub_directory_file.get_path ();

				if (visited.contains (sub_directory_path))
					continue;

				sub_directory = new ModuleIterator.for_subdirectory (sub_directory_path, visited);
				if (next_in_sub_directory ())
					return true;
				else
					continue;
			}

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

	private bool next_in_sub_directory () throws Error {
		if (sub_directory.next ()) {
			core_descriptor = sub_directory.get ();

			return true;
		}

		sub_directory = null;

		return false;
	}

	private void set_current_directory_as_visited () {
		var current_directory_file = File.new_for_path (directories[current_directory]);
		var current_directory_path = current_directory_file.get_path ();
		visited.add (current_directory_path);
	}

	private bool was_current_directory_visited () {
		var current_directory_file = File.new_for_path (directories[current_directory]);
		var current_directory_path = current_directory_file.get_path ();

		return visited.contains (current_directory_path);
	}
}

