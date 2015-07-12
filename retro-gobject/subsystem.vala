// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * TODO Change visibility once the interface have been tested.
 */
internal struct SubsystemMemoryInfo {
	string extension;
	uint type;
}

/**
 * TODO Change visibility once the interface have been tested.
 */
internal struct SubsystemRomInfo {
	string desc;
	string valid_extensions;
	bool need_fullpath;
	bool block_extract;
	bool required;
	SubsystemMemoryInfo? memory;
	uint num_memory;
}

/**
 * TODO Change visibility once the interface have been tested.
 */
internal struct SubsystemInfo {
	string desc;
	string ident;
	SubsystemRomInfo? roms;
	uint num_roms;
	uint id;
}

}

