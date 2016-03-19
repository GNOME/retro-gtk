/* Copyright (C) 2010-2014 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this libretro API header (libretro-environment.h).
 * ---------------------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __RETRO_ENVIRONMENT_H__
#define __RETRO_ENVIRONMENT_H__

/* If set, this call is not part of the public libretro API yet. It can
 * change or be removed at any time. */
#define RETRO_ENVIRONMENT_EXPERIMENTAL 0x10000
/* Environment callback to be used internally in frontend. */
#define RETRO_ENVIRONMENT_PRIVATE 0x20000

/* Environment commands. */
#define RETRO_ENVIRONMENT_SET_ROTATION  1  /* const unsigned * --
                                            * Sets screen rotation of graphics.
                                            * Is only implemented if rotation can be accelerated by hardware.
                                            * Valid values are 0, 1, 2, 3, which rotates screen by 0, 90, 180,
                                            * 270 degrees counter-clockwise respectively.
                                            */
#define RETRO_ENVIRONMENT_GET_OVERSCAN  2  /* bool * --
                                            * Boolean value whether or not the implementation should use overscan,
                                            * or crop away overscan.
                                            */
#define RETRO_ENVIRONMENT_GET_CAN_DUPE  3  /* bool * --
                                            * Boolean value whether or not frontend supports frame duping,
                                            * passing NULL to video frame callback.
                                            */

                                           /* Environ 4, 5 are no longer supported (GET_VARIABLE / SET_VARIABLES),
                                            * and reserved to avoid possible ABI clash.
                                            */

#define RETRO_ENVIRONMENT_SET_MESSAGE   6  /* const struct retro_message * --
                                            * Sets a message to be displayed in implementation-specific manner
                                            * for a certain amount of 'frames'.
                                            * Should not be used for trivial messages, which should simply be
                                            * logged via RETRO_ENVIRONMENT_GET_LOG_INTERFACE (or as a
                                            * fallback, stderr).
                                            */
#define RETRO_ENVIRONMENT_SHUTDOWN      7  /* N/A (NULL) --
                                            * Requests the frontend to shutdown.
                                            * Should only be used if game has a specific
                                            * way to shutdown the game from a menu item or similar.
                                            */
#define RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL 8
                                           /* const unsigned * --
                                            * Gives a hint to the frontend how demanding this implementation
                                            * is on a system. E.g. reporting a level of 2 means
                                            * this implementation should run decently on all frontends
                                            * of level 2 and up.
                                            *
                                            * It can be used by the frontend to potentially warn
                                            * about too demanding implementations.
                                            *
                                            * The levels are "floating".
                                            *
                                            * This function can be called on a per-game basis,
                                            * as certain games an implementation can play might be
                                            * particularly demanding.
                                            * If called, it should be called in retro_load_game().
                                            */
#define RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY 9
                                           /* const char ** --
                                            * Returns the "system" directory of the frontend.
                                            * This directory can be used to store system specific
                                            * content such as BIOSes, configuration data, etc.
                                            * The returned value can be NULL.
                                            * If so, no such directory is defined,
                                            * and it's up to the implementation to find a suitable directory.
                                            *
                                            * NOTE: Some cores used this folder also for "save" data such as
                                            * memory cards, etc, for lack of a better place to put it.
                                            * This is now discouraged, and if possible, cores should try to
                                            * use the new GET_SAVE_DIRECTORY.
                                            */
#define RETRO_ENVIRONMENT_SET_PIXEL_FORMAT 10
                                           /* const enum retro_pixel_format * --
                                            * Sets the internal pixel format used by the implementation.
                                            * The default pixel format is RETRO_PIXEL_FORMAT_0RGB1555.
                                            * This pixel format however, is deprecated (see enum retro_pixel_format).
                                            * If the call returns false, the frontend does not support this pixel
                                            * format.
                                            *
                                            * This function should be called inside retro_load_game() or
                                            * retro_get_system_av_info().
                                            */
#define RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS 11
                                           /* const struct retro_input_descriptor * --
                                            * Sets an array of retro_input_descriptors.
                                            * It is up to the frontend to present this in a usable way.
                                            * The array is terminated by retro_input_descriptor::description
                                            * being set to NULL.
                                            * This function can be called at any time, but it is recommended
                                            * to call it as early as possible.
                                            */
#define RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK 12
                                           /* const struct retro_keyboard_callback * --
                                            * Sets a callback function used to notify core about keyboard events.
                                            */
#define RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE 13
                                           /* const struct retro_disk_control_callback * --
                                            * Sets an interface which frontend can use to eject and insert
                                            * disk images.
                                            * This is used for games which consist of multiple images and
                                            * must be manually swapped out by the user (e.g. PSX).
                                            */
#define RETRO_ENVIRONMENT_SET_HW_RENDER 14
                                           /* struct retro_hw_render_callback * --
                                            * Sets an interface to let a libretro core render with
                                            * hardware acceleration.
                                            * Should be called in retro_load_game().
                                            * If successful, libretro cores will be able to render to a
                                            * frontend-provided framebuffer.
                                            * The size of this framebuffer will be at least as large as
                                            * max_width/max_height provided in get_av_info().
                                            * If HW rendering is used, pass only RETRO_HW_FRAME_BUFFER_VALID or
                                            * NULL to retro_video_refresh_t.
                                            */
#define RETRO_ENVIRONMENT_GET_VARIABLE 15
                                           /* struct retro_variable * --
                                            * Interface to acquire user-defined information from environment
                                            * that cannot feasibly be supported in a multi-system way.
                                            * 'key' should be set to a key which has already been set by
                                            * SET_VARIABLES.
                                            * 'data' will be set to a value or NULL.
                                            */
#define RETRO_ENVIRONMENT_SET_VARIABLES 16
                                           /* const struct retro_variable * --
                                            * Allows an implementation to signal the environment
                                            * which variables it might want to check for later using
                                            * GET_VARIABLE.
                                            * This allows the frontend to present these variables to
                                            * a user dynamically.
                                            * This should be called as early as possible (ideally in
                                            * retro_set_environment).
                                            *
                                            * 'data' points to an array of retro_variable structs
                                            * terminated by a { NULL, NULL } element.
                                            * retro_variable::key should be namespaced to not collide
                                            * with other implementations' keys. E.g. A core called
                                            * 'foo' should use keys named as 'foo_option'.
                                            * retro_variable::value should contain a human readable
                                            * description of the key as well as a '|' delimited list
                                            * of expected values.
                                            *
                                            * The number of possible options should be very limited,
                                            * i.e. it should be feasible to cycle through options
                                            * without a keyboard.
                                            *
                                            * First entry should be treated as a default.
                                            *
                                            * Example entry:
                                            * { "foo_option", "Speed hack coprocessor X; false|true" }
                                            *
                                            * Text before first ';' is description. This ';' must be
                                            * followed by a space, and followed by a list of possible
                                            * values split up with '|'.
                                            *
                                            * Only strings are operated on. The possible values will
                                            * generally be displayed and stored as-is by the frontend.
                                            */
#define RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE 17
                                           /* bool * --
                                            * Result is set to true if some variables are updated by
                                            * frontend since last call to RETRO_ENVIRONMENT_GET_VARIABLE.
                                            * Variables should be queried with GET_VARIABLE.
                                            */
#define RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME 18
                                           /* const bool * --
                                            * If true, the libretro implementation supports calls to
                                            * retro_load_game() with NULL as argument.
                                            * Used by cores which can run without particular game data.
                                            * This should be called within retro_set_environment() only.
                                            */
#define RETRO_ENVIRONMENT_GET_LIBRETRO_PATH 19
                                           /* const char ** --
                                            * Retrieves the absolute path from where this libretro
                                            * implementation was loaded.
                                            * NULL is returned if the libretro was loaded statically
                                            * (i.e. linked statically to frontend), or if the path cannot be
                                            * determined.
                                            * Mostly useful in cooperation with SET_SUPPORT_NO_GAME as assets can
                                            * be loaded without ugly hacks.
                                            */

                                           /* Environment 20 was an obsolete version of SET_AUDIO_CALLBACK.
                                            * It was not used by any known core at the time,
                                            * and was removed from the API. */
#define RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK 22
                                           /* const struct retro_audio_callback * --
                                            * Sets an interface which is used to notify a libretro core about audio
                                            * being available for writing.
                                            * The callback can be called from any thread, so a core using this must
                                            * have a thread safe audio implementation.
                                            * It is intended for games where audio and video are completely
                                            * asynchronous and audio can be generated on the fly.
                                            * This interface is not recommended for use with emulators which have
                                            * highly synchronous audio.
                                            *
                                            * The callback only notifies about writability; the libretro core still
                                            * has to call the normal audio callbacks
                                            * to write audio. The audio callbacks must be called from within the
                                            * notification callback.
                                            * The amount of audio data to write is up to the implementation.
                                            * Generally, the audio callback will be called continously in a loop.
                                            *
                                            * Due to thread safety guarantees and lack of sync between audio and
                                            * video, a frontend  can selectively disallow this interface based on
                                            * internal configuration. A core using this interface must also
                                            * implement the "normal" audio interface.
                                            *
                                            * A libretro core using SET_AUDIO_CALLBACK should also make use of
                                            * SET_FRAME_TIME_CALLBACK.
                                            */
#define RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK 21
                                           /* const struct retro_frame_time_callback * --
                                            * Lets the core know how much time has passed since last
                                            * invocation of retro_run().
                                            * The frontend can tamper with the timing to fake fast-forward,
                                            * slow-motion, frame stepping, etc.
                                            * In this case the delta time will use the reference value
                                            * in frame_time_callback..
                                            */
#define RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE 23
                                           /* struct retro_rumble_interface * --
                                            * Gets an interface which is used by a libretro core to set
                                            * state of rumble motors in controllers.
                                            * A strong and weak motor is supported, and they can be
                                            * controlled indepedently.
                                            */
#define RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES 24
                                           /* uint64_t * --
                                            * Gets a bitmask telling which device type are expected to be
                                            * handled properly in a call to retro_input_state_t.
                                            * Devices which are not handled or recognized always return
                                            * 0 in retro_input_state_t.
                                            * Example bitmask: caps = (1 << RETRO_DEVICE_JOYPAD) | (1 << RETRO_DEVICE_ANALOG).
                                            * Should only be called in retro_run().
                                            */
#define RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE (25 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* struct retro_sensor_interface * --
                                            * Gets access to the sensor interface.
                                            * The purpose of this interface is to allow
                                            * setting state related to sensors such as polling rate,
                                            * enabling/disable it entirely, etc.
                                            * Reading sensor state is done via the normal
                                            * input_state_callback API.
                                            */
#define RETRO_ENVIRONMENT_GET_CAMERA_INTERFACE (26 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* struct retro_camera_callback * --
                                            * Gets an interface to a video camera driver.
                                            * A libretro core can use this interface to get access to a
                                            * video camera.
                                            * New video frames are delivered in a callback in same
                                            * thread as retro_run().
                                            *
                                            * GET_CAMERA_INTERFACE should be called in retro_load_game().
                                            *
                                            * Depending on the camera implementation used, camera frames
                                            * will be delivered as a raw framebuffer,
                                            * or as an OpenGL texture directly.
                                            *
                                            * The core has to tell the frontend here which types of
                                            * buffers can be handled properly.
                                            * An OpenGL texture can only be handled when using a
                                            * libretro GL core (SET_HW_RENDER).
                                            * It is recommended to use a libretro GL core when
                                            * using camera interface.
                                            *
                                            * The camera is not started automatically. The retrieved start/stop
                                            * functions must be used to explicitly
                                            * start and stop the camera driver.
                                            */
#define RETRO_ENVIRONMENT_GET_LOG_INTERFACE 27
                                           /* struct retro_log_callback * --
                                            * Gets an interface for logging. This is useful for
                                            * logging in a cross-platform way
                                            * as certain platforms cannot use use stderr for logging.
                                            * It also allows the frontend to
                                            * show logging information in a more suitable way.
                                            * If this interface is not used, libretro cores should
                                            * log to stderr as desired.
                                            */
#define RETRO_ENVIRONMENT_GET_PERF_INTERFACE 28
                                           /* struct retro_perf_callback * --
                                            * Gets an interface for performance counters. This is useful
                                            * for performance logging in a cross-platform way and for detecting
                                            * architecture-specific features, such as SIMD support.
                                            */
#define RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE 29
                                           /* struct retro_location_callback * --
                                            * Gets access to the location interface.
                                            * The purpose of this interface is to be able to retrieve
                                            * location-based information from the host device,
                                            * such as current latitude / longitude.
                                            */
#define RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY 30
                                           /* const char ** --
                                            * Returns the "content" directory of the frontend.
                                            * This directory can be used to store specific assets that the
                                            * core relies upon, such as art assets,
                                            * input data, etc etc.
                                            * The returned value can be NULL.
                                            * If so, no such directory is defined,
                                            * and it's up to the implementation to find a suitable directory.
                                            */
#define RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY 31
                                           /* const char ** --
                                            * Returns the "save" directory of the frontend.
                                            * This directory can be used to store SRAM, memory cards,
                                            * high scores, etc, if the libretro core
                                            * cannot use the regular memory interface (retro_get_memory_data()).
                                            *
                                            * NOTE: libretro cores used to check GET_SYSTEM_DIRECTORY for
                                            * similar things before.
                                            * They should still check GET_SYSTEM_DIRECTORY if they want to
                                            * be backwards compatible.
                                            * The path here can be NULL. It should only be non-NULL if the
                                            * frontend user has set a specific save path.
                                            */
#define RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO 32
                                           /* const struct retro_system_av_info * --
                                            * Sets a new av_info structure. This can only be called from
                                            * within retro_run().
                                            * This should *only* be used if the core is completely altering the
                                            * internal resolutions, aspect ratios, timings, sampling rate, etc.
                                            * Calling this can require a full reinitialization of video/audio
                                            * drivers in the frontend,
                                            *
                                            * so it is important to call it very sparingly, and usually only with
                                            * the users explicit consent.
                                            * An eventual driver reinitialize will happen so that video and
                                            * audio callbacks
                                            * happening after this call within the same retro_run() call will
                                            * target the newly initialized driver.
                                            *
                                            * This callback makes it possible to support configurable resolutions
                                            * in games, which can be useful to
                                            * avoid setting the "worst case" in max_width/max_height.
                                            *
                                            * ***HIGHLY RECOMMENDED*** Do not call this callback every time
                                            * resolution changes in an emulator core if it's
                                            * expected to be a temporary change, for the reasons of possible
                                            * driver reinitialization.
                                            * This call is not a free pass for not trying to provide
                                            * correct values in retro_get_system_av_info(). If you need to change
                                            * things like aspect ratio or nominal width/height,
                                            * use RETRO_ENVIRONMENT_SET_GEOMETRY, which is a softer variant
                                            * of SET_SYSTEM_AV_INFO.
                                            *
                                            * If this returns false, the frontend does not acknowledge a
                                            * changed av_info struct.
                                            */
#define RETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK 33
                                           /* const struct retro_get_proc_address_interface * --
                                            * Allows a libretro core to announce support for the
                                            * get_proc_address() interface.
                                            * This interface allows for a standard way to extend libretro where
                                            * use of environment calls are too indirect,
                                            * e.g. for cases where the frontend wants to call directly into the core.
                                            *
                                            * If a core wants to expose this interface, SET_PROC_ADDRESS_CALLBACK
                                            * **MUST** be called from within retro_set_environment().
                                            */
#define RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO 34
                                           /* const struct retro_subsystem_info * --
                                            * This environment call introduces the concept of libretro "subsystems".
                                            * A subsystem is a variant of a libretro core which supports
                                            * different kinds of games.
                                            * The purpose of this is to support e.g. emulators which might
                                            * have special needs, e.g. Super Nintendo's Super GameBoy, Sufami Turbo.
                                            * It can also be used to pick among subsystems in an explicit way
                                            * if the libretro implementation is a multi-system emulator itself.
                                            *
                                            * Loading a game via a subsystem is done with retro_load_game_special(),
                                            * and this environment call allows a libretro core to expose which
                                            * subsystems are supported for use with retro_load_game_special().
                                            * A core passes an array of retro_game_special_info which is terminated
                                            * with a zeroed out retro_game_special_info struct.
                                            *
                                            * If a core wants to use this functionality, SET_SUBSYSTEM_INFO
                                            * **MUST** be called from within retro_set_environment().
                                            */
#define RETRO_ENVIRONMENT_SET_CONTROLLER_INFO 35
                                           /* const struct retro_controller_info * --
                                            * This environment call lets a libretro core tell the frontend
                                            * which controller types are recognized in calls to
                                            * retro_set_controller_port_device().
                                            *
                                            * Some emulators such as Super Nintendo
                                            * support multiple lightgun types which must be specifically
                                            * selected from.
                                            * It is therefore sometimes necessary for a frontend to be able
                                            * to tell the core about a special kind of input device which is
                                            * not covered by the libretro input API.
                                            *
                                            * In order for a frontend to understand the workings of an input device,
                                            * it must be a specialized type
                                            * of the generic device types already defined in the libretro API.
                                            *
                                            * Which devices are supported can vary per input port.
                                            * The core must pass an array of const struct retro_controller_info which
                                            * is terminated with a blanked out struct. Each element of the struct
                                            * corresponds to an ascending port index to
                                            * retro_set_controller_port_device().
                                            * Even if special device types are set in the libretro core,
                                            * libretro should only poll input based on the base input device types.
                                            */
#define RETRO_ENVIRONMENT_SET_MEMORY_MAPS (36 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* const struct retro_memory_map * --
                                            * This environment call lets a libretro core tell the frontend
                                            * about the memory maps this core emulates.
                                            * This can be used to implement, for example, cheats in a core-agnostic way.
                                            *
                                            * Should only be used by emulators; it doesn't make much sense for
                                            * anything else.
                                            * It is recommended to expose all relevant pointers through
                                            * retro_get_memory_* as well.
                                            *
                                            * Can be called from retro_init and retro_load_game.
                                            */
#define RETRO_ENVIRONMENT_SET_GEOMETRY 37
                                           /* const struct retro_game_geometry * --
                                            * This environment call is similar to SET_SYSTEM_AV_INFO for changing
                                            * video parameters, but provides a guarantee that drivers will not be
                                            * reinitialized.
                                            * This can only be called from within retro_run().
                                            *
                                            * The purpose of this call is to allow a core to alter nominal
                                            * width/heights as well as aspect ratios on-the-fly, which can be
                                            * useful for some emulators to change in run-time.
                                            *
                                            * max_width/max_height arguments are ignored and cannot be changed
                                            * with this call as this could potentially require a reinitialization or a
                                            * non-constant time operation.
                                            * If max_width/max_height are to be changed, SET_SYSTEM_AV_INFO is required.
                                            *
                                            * A frontend must guarantee that this environment call completes in
                                            * constant time.
                                            */
#define RETRO_ENVIRONMENT_GET_USERNAME 38
                                           /* const char **
                                            * Returns the specified username of the frontend, if specified by the user.
                                            * This username can be used as a nickname for a core that has online facilities
                                            * or any other mode where personalization of the user is desirable.
                                            * The returned value can be NULL.
                                            * If this environ callback is used by a core that requires a valid username,
                                            * a default username should be specified by the core.
                                            */
#define RETRO_ENVIRONMENT_GET_LANGUAGE 39
                                           /* unsigned * --
                                            * Returns the specified language of the frontend, if specified by the user.
                                            * It can be used by the core for localization purposes.
                                            */

#endif
