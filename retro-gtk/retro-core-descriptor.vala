// This file is part of retro-gtk. License: GPLv3

public class Retro.CoreDescriptor : Object {
	public class Platform : Object {

		private KeyFile key_file;
		private string group;

		private Platform (KeyFile key_file, string platform) {
			this.key_file = key_file;
			group = PLATFORM_GROUP_PREFIX + platform;
		}
	}

	private const string LIBRETRO_GROUP = "Libretro";
	private const string PLATFORM_GROUP_PREFIX = "Platform:";
	private const string FIRMWARE_GROUP_PREFIX = "Firmware:";

	private const string TYPE_KEY = "Type";
	private const string NAME_KEY = "Name";
	private const string ICON_KEY = "Icon";
	private const string MODULE_KEY = "Module";
	private const string LIBRETRO_VERSION_KEY = "LibretroVersion";

	private const string PLATFORM_MIME_TYPE_KEY = "MimeType";
	private const string PLATFORM_FIRMWARES_KEY = "Firmwares";

	private const string FIRMWARE_PATH_KEY = "Path";
	private const string FIRMWARE_MD5_KEY = "MD5";
	private const string FIRMWARE_SHA512_KEY = "SHA-512";
	private const string FIRMWARE_MANDATORY_KEY = "Mandatory";

	private const string TYPE_GAME = "Game";
	private const string TYPE_EMULATOR = "Emulator";

	private string filename;
	private KeyFile key_file;

	public CoreDescriptor (string filename) throws CoreDescriptorError, KeyFileError, FileError {
		this.filename = filename;
		key_file = new KeyFile ();
		key_file.load_from_file (filename, KeyFileFlags.NONE);

		check_libretro_group ();

		foreach (var group in key_file.get_groups ()) {
			if (group.has_prefix (PLATFORM_GROUP_PREFIX))
				check_platform_group (group);
			else if (group.has_prefix (FIRMWARE_GROUP_PREFIX))
				check_firmware_group (group);
		}
	}

	public bool has_icon () throws KeyFileError {
		return key_file.has_key (LIBRETRO_GROUP, ICON_KEY);
	}

	public string get_id () {
		return Path.get_basename (filename);
	}

	public bool get_is_game () throws KeyFileError {
		return key_file.get_string (LIBRETRO_GROUP, TYPE_KEY) == TYPE_GAME;
	}

	public bool get_is_emulator () throws KeyFileError {
		return key_file.get_string (LIBRETRO_GROUP, TYPE_KEY) == TYPE_EMULATOR;
	}

	public string get_name () throws KeyFileError {
		return key_file.get_string (LIBRETRO_GROUP, NAME_KEY);
	}

	public GLib.Icon get_icon () throws KeyFileError {
		var icon_name = key_file.get_string (LIBRETRO_GROUP, ICON_KEY);

		return new ThemedIcon (icon_name);
	}

	public string get_module () throws KeyFileError {
		return key_file.get_string (LIBRETRO_GROUP, MODULE_KEY);
	}

	public File? get_module_file () throws KeyFileError {
		var file = File.new_for_path (filename);
		var dir = file.get_parent ();
		if (dir == null)
			return null;

		var module = get_module ();
		var module_file = dir.get_child (module);
		if (!module_file.query_exists ())
			return null;

		return module_file;
	}

	public bool has_platform (string platform) {
		return key_file.has_group (PLATFORM_GROUP_PREFIX + platform);
	}

	public bool has_firmwares (string platform) throws KeyFileError {
		return key_file.has_key (PLATFORM_GROUP_PREFIX + platform, PLATFORM_FIRMWARES_KEY);
	}

	public bool has_firmware_md5 (string firmware) throws KeyFileError {
		return key_file.has_key (FIRMWARE_GROUP_PREFIX + firmware, FIRMWARE_MD5_KEY);
	}

	public bool has_firmware_sha512 (string firmware) throws KeyFileError {
		return key_file.has_key (FIRMWARE_GROUP_PREFIX + firmware, FIRMWARE_SHA512_KEY);
	}

	public string[] get_mime_type (string platform) throws KeyFileError {
		return key_file.get_string_list (PLATFORM_GROUP_PREFIX + platform, PLATFORM_MIME_TYPE_KEY);
	}

	public string[] get_firmwares (string platform) throws KeyFileError {
		return key_file.get_string_list (PLATFORM_GROUP_PREFIX + platform, PLATFORM_FIRMWARES_KEY);
	}

	public string get_firmware_path (string firmware) throws KeyFileError {
		return key_file.get_string (FIRMWARE_GROUP_PREFIX + firmware, FIRMWARE_PATH_KEY);
	}

	public string get_firmware_md5 (string firmware) throws KeyFileError {
		return key_file.get_string (FIRMWARE_GROUP_PREFIX + firmware, FIRMWARE_MD5_KEY);
	}

	public string get_firmware_sha512 (string firmware) throws KeyFileError {
		return key_file.get_string (FIRMWARE_GROUP_PREFIX + firmware, FIRMWARE_SHA512_KEY);
	}

	public bool get_is_firmware_mandatory (string firmware) throws KeyFileError {
		return key_file.get_boolean (FIRMWARE_GROUP_PREFIX + firmware, FIRMWARE_MANDATORY_KEY);
	}

	public bool get_platform_supports_mime_types (string platform, string[] mime_types) throws KeyFileError {
		var supported_mime_types = get_mime_type (platform);
		foreach (var mime_type in mime_types)
			if (!(mime_type in supported_mime_types))
				return false;

		return true;
	}

	private void check_libretro_group () throws CoreDescriptorError, KeyFileError {
		check_has_required_key (LIBRETRO_GROUP, TYPE_KEY);
		check_has_required_key (LIBRETRO_GROUP, NAME_KEY);
		check_has_required_key (LIBRETRO_GROUP, MODULE_KEY);
		check_has_required_key (LIBRETRO_GROUP, LIBRETRO_VERSION_KEY);
	}

	private void check_platform_group (string group) throws CoreDescriptorError, KeyFileError {
		check_has_required_key (group, PLATFORM_MIME_TYPE_KEY);
		if (key_file.has_key (group, PLATFORM_FIRMWARES_KEY))
			foreach (var firmware in key_file.get_string_list (group, PLATFORM_FIRMWARES_KEY))
				if (!key_file.has_group (FIRMWARE_GROUP_PREFIX + firmware))
					throw new CoreDescriptorError.FIRMWARE_NOT_FOUND ("%s isn't a valid Libretro core descriptor: [%s] mentioned in [%s] not found.", filename, FIRMWARE_GROUP_PREFIX + firmware, group);
	}

	private void check_firmware_group (string group) throws CoreDescriptorError, KeyFileError {
		check_has_required_key (group, FIRMWARE_PATH_KEY);
		check_has_required_key (group, FIRMWARE_MANDATORY_KEY);
	}

	private void check_has_required_key (string group, string key) throws CoreDescriptorError, KeyFileError {
		if (!key_file.has_key (LIBRETRO_GROUP, TYPE_KEY))
			throw new CoreDescriptorError.REQUIRED_KEY_NOT_FOUND ("%s isn't a valid Libretro core descriptor: required key %s not found in group [%s].", filename, key, group);
	}
}

