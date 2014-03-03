namespace Retro.Environment {
	enum Command {
		EXPERIMENTAL_MASK             = 0x10000,
		PRIVATE_MASK                  = 0x20000,
		SET_ROTATION                  = 1,
		GET_OVERSCAN                  = 2,
		GET_CAN_DUPE                  = 3,
		SET_MESSAGE                   = 6,
		SHUTDOWN                      = 7,
		SET_PERFORMANCE_LEVEL         = 8,
		GET_SYSTEM_DIRECTORY          = 9,
		SET_PIXEL_FORMAT              = 10,
		SET_INPUT_DESCRIPTORS         = 11,
		SET_KEYBOARD_CALLBACK         = 12,
		SET_DISK_CONTROL_INTERFACE    = 13,
		SET_HW_RENDER                 = 14,
		GET_VARIABLE                  = 15,
		SET_VARIABLES                 = 16,
		GET_VARIABLE_UPDATE           = 17,
		SET_SUPPORT_NO_GAME           = 18,
		GET_LIBRETRO_PATH             = 19,
		SET_AUDIO_CALLBACK            = 22,
		SET_FRAME_TIME_CALLBACK       = 21,
		GET_RUMBLE_INTERFACE          = 23,
		GET_INPUT_DEVICE_CAPABILITIES = 24,
		GET_SENSOR_INTERFACE          = (25 | Command.EXPERIMENTAL_MASK),
		GET_CAMERA_INTERFACE          = (26 | Command.EXPERIMENTAL_MASK),
		GET_LOG_INTERFACE             = 27,
		GET_PERF_INTERFACE            = 28
	}
}

