// This file is part of retro-gtk. License: GPL-3.0+.

namespace Retro {

/**
 * The memory region types.
 */
public enum MemoryType {
	TYPE_MASK               = 0xff,
	SAVE_RAM                = 0,
	RTC                     = 1,
	SYSTEM_RAM              = 2,
	VIDEO_RAM               = 3,
	SNES_BSX_RAM            = (1 << 8) | MemoryType.SAVE_RAM,
	SNES_BSX_PRAM           = (2 << 8) | MemoryType.SAVE_RAM,
	SNES_SUFAMI_TURBO_A_RAM = (3 << 8) | MemoryType.SAVE_RAM,
	SNES_SUFAMI_TURBO_B_RAM = (4 << 8) | MemoryType.SAVE_RAM,
	SNES_GAME_BOY_RAM       = (5 << 8) | MemoryType.SAVE_RAM,
	SNES_GAME_BOY_RTC       = (6 << 8) | MemoryType.RTC;

	/**
	 * Gets the basic type of a memory type.
	 *
	 * Applies the type mask on a MemoryType to get its basic type.
	 * If the memory type is already basic, it will return the same type.
	 *
	 * E.g MemoryType.SNES_BSX_RAM.get_basic_type () returns
	 * MemoryType.SAVE_RAM, and MemoryType.SAVE_RAM.get_basic_type () also
	 * returns MemoryType.SAVE_RAM.
	 *
	 * @return the basic type of a memory type
	 */
	public MemoryType get_basic_type () {
		var basic_type = this & MemoryType.TYPE_MASK;
		return basic_type;
	}
}

/**
 * TODO Change visibility once the interface have been tested.
 */
[Flags]
internal enum MemDesc {
	CONST     = (1 << 0),
	BIGENDIAN = (1 << 1),
	ALIGN_2   = (1 << 16),
	ALIGN_4   = (2 << 16),
	ALIGN_8   = (3 << 16),
	MINSIZE_2 = (1 << 24),
	MINSIZE_4 = (2 << 24),
	MINSIZE_8 = (3 << 24)
}

/**
 * TODO Change visibility once the interface have been tested.
 */
internal struct MemoryDescriptor {
	uint64 flags;
	void *ptr;
	size_t offset;
	size_t start;
	size_t select;
	size_t disconnect;
	size_t len;
	string addrspace;
}

/**
 * TODO Change visibility once the interface have been tested.
 */
internal struct MemoryMap {
	MemoryDescriptor? descriptors;
	uint num_descriptors;
}

}

