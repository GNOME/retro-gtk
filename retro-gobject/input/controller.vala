// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * TODO Change visibility once the interface have been tested.
 */
internal struct ControllerDescription {
	string desc;
	uint id;
}

/**
 * TODO Change visibility once the interface have been tested.
 */
internal struct ControllerInfo {
	ControllerDescription? types;
	uint num_types;
}

}

