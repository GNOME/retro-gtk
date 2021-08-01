/* Copyright (C) 2010-2018 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this libretro API header (libretro.h).
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

#ifndef LIBRETRO_H__
#define LIBRETRO_H__

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
#if defined(_MSC_VER) && _MSC_VER < 1800 && !defined(SN_TARGET_PS3)
/* Hack applied for MSVC when compiling in C89 mode
 * as it isn't C99-compliant. */
#define bool unsigned char
#define true 1
#define false 0
#else
#include <stdbool.h>
#endif
#endif

#ifndef RETRO_CALLCONV
#  if defined(__GNUC__) && defined(__i386__) && !defined(__x86_64__)
#    define RETRO_CALLCONV __attribute__((cdecl))
#  elif defined(_MSC_VER) && defined(_M_X86) && !defined(_M_X64)
#    define RETRO_CALLCONV __cdecl
#  else
#    define RETRO_CALLCONV /* all other platforms only have one calling convention each */
#  endif
#endif

#ifndef RETRO_API
#  if defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
#    ifdef RETRO_IMPORT_SYMBOLS
#      ifdef __GNUC__
#        define RETRO_API RETRO_CALLCONV __attribute__((__dllimport__))
#      else
#        define RETRO_API RETRO_CALLCONV __declspec(dllimport)
#      endif
#    else
#      ifdef __GNUC__
#        define RETRO_API RETRO_CALLCONV __attribute__((__dllexport__))
#      else
#        define RETRO_API RETRO_CALLCONV __declspec(dllexport)
#      endif
#    endif
#  else
#      if defined(__GNUC__) && __GNUC__ >= 4 && !defined(__CELLOS_LV2__)
#        define RETRO_API RETRO_CALLCONV __attribute__((__visibility__("default")))
#      else
#        define RETRO_API RETRO_CALLCONV
#      endif
#  endif
#endif

/* Used for checking API/ABI mismatches that can break libretro
 * implementations.
 * It is not incremented for compatible changes to the API.
 */
#define RETRO_API_VERSION         1

/*
 * Libretro's fundamental device abstractions.
 *
 * Libretro's input system consists of some standardized device types,
 * such as a joypad (with/without analog), mouse, keyboard, lightgun
 * and a pointer.
 *
 * The functionality of these devices are fixed, and individual cores
 * map their own concept of a controller to libretro's abstractions.
 * This makes it possible for frontends to map the abstract types to a
 * real input device, and not having to worry about binding input
 * correctly to arbitrary controller layouts.
 */

/* Input disabled. */
#define RETRO_DEVICE_NONE         0

/* The JOYPAD is called RetroPad. It is essentially a Super Nintendo
 * controller, but with additional L2/R2/L3/R3 buttons, similar to a
 * PS1 DualShock. */
#define RETRO_DEVICE_JOYPAD       1

/* The mouse is a simple mouse, similar to Super Nintendo's mouse.
 * X and Y coordinates are reported relatively to last poll (poll callback).
 * It is up to the libretro implementation to keep track of where the mouse
 * pointer is supposed to be on the screen.
 * The frontend must make sure not to interfere with its own hardware
 * mouse pointer.
 */
#define RETRO_DEVICE_MOUSE        2

/* KEYBOARD device lets one poll for raw key pressed.
 * It is poll based, so input callback will return with the current
 * pressed state.
 * For event/text based keyboard input, see
 * RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK.
 */
#define RETRO_DEVICE_KEYBOARD     3

/* The ANALOG device is an extension to JOYPAD (RetroPad).
 * Similar to DualShock2 it adds two analog sticks and all buttons can
 * be analog. This is treated as a separate device type as it returns
 * axis values in the full analog range of [-0x8000, 0x7fff].
 * Positive X axis is right. Positive Y axis is down.
 * Buttons are returned in the range [0, 0x7fff].
 * Only use ANALOG type when polling for analog values.
 */
#define RETRO_DEVICE_ANALOG       5

/* Abstracts the concept of a pointing mechanism, e.g. touch.
 * This allows libretro to query in absolute coordinates where on the
 * screen a mouse (or something similar) is being placed.
 * For a touch centric device, coordinates reported are the coordinates
 * of the press.
 *
 * Coordinates in X and Y are reported as:
 * [-0x7fff, 0x7fff]: -0x7fff corresponds to the far left/top of the screen,
 * and 0x7fff corresponds to the far right/bottom of the screen.
 * The "screen" is here defined as area that is passed to the frontend and
 * later displayed on the monitor.
 *
 * The frontend is free to scale/resize this screen as it sees fit, however,
 * (X, Y) = (-0x7fff, -0x7fff) will correspond to the top-left pixel of the
 * game image, etc.
 *
 * To check if the pointer coordinates are valid (e.g. a touch display
 * actually being touched), PRESSED returns 1 or 0.
 *
 * If using a mouse on a desktop, PRESSED will usually correspond to the
 * left mouse button, but this is a frontend decision.
 * PRESSED will only return 1 if the pointer is inside the game screen.
 *
 * For multi-touch, the index variable can be used to successively query
 * more presses.
 * If index = 0 returns true for _PRESSED, coordinates can be extracted
 * with _X, _Y for index = 0. One can then query _PRESSED, _X, _Y with
 * index = 1, and so on.
 * Eventually _PRESSED will return false for an index. No further presses
 * are registered at this point. */
#define RETRO_DEVICE_POINTER      6

/* Buttons for the RetroPad (JOYPAD).
 * The placement of these is equivalent to placements on the
 * Super Nintendo controller.
 * L2/R2/L3/R3 buttons correspond to the PS1 DualShock. */
#define RETRO_DEVICE_ID_JOYPAD_B        0
#define RETRO_DEVICE_ID_JOYPAD_Y        1
#define RETRO_DEVICE_ID_JOYPAD_SELECT   2
#define RETRO_DEVICE_ID_JOYPAD_START    3
#define RETRO_DEVICE_ID_JOYPAD_UP       4
#define RETRO_DEVICE_ID_JOYPAD_DOWN     5
#define RETRO_DEVICE_ID_JOYPAD_LEFT     6
#define RETRO_DEVICE_ID_JOYPAD_RIGHT    7
#define RETRO_DEVICE_ID_JOYPAD_A        8
#define RETRO_DEVICE_ID_JOYPAD_X        9
#define RETRO_DEVICE_ID_JOYPAD_L       10
#define RETRO_DEVICE_ID_JOYPAD_R       11
#define RETRO_DEVICE_ID_JOYPAD_L2      12
#define RETRO_DEVICE_ID_JOYPAD_R2      13
#define RETRO_DEVICE_ID_JOYPAD_L3      14
#define RETRO_DEVICE_ID_JOYPAD_R3      15

/* Index / Id values for ANALOG device. */
#define RETRO_DEVICE_INDEX_ANALOG_LEFT       0
#define RETRO_DEVICE_INDEX_ANALOG_RIGHT      1
#define RETRO_DEVICE_ID_ANALOG_X             0
#define RETRO_DEVICE_ID_ANALOG_Y             1

/* Id values for MOUSE. */
#define RETRO_DEVICE_ID_MOUSE_X                0
#define RETRO_DEVICE_ID_MOUSE_Y                1
#define RETRO_DEVICE_ID_MOUSE_LEFT             2
#define RETRO_DEVICE_ID_MOUSE_RIGHT            3
#define RETRO_DEVICE_ID_MOUSE_WHEELUP          4
#define RETRO_DEVICE_ID_MOUSE_WHEELDOWN        5
#define RETRO_DEVICE_ID_MOUSE_MIDDLE           6
#define RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELUP    7
#define RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELDOWN  8
#define RETRO_DEVICE_ID_MOUSE_BUTTON_4         9
#define RETRO_DEVICE_ID_MOUSE_BUTTON_5         10

/* Id values for POINTER. */
#define RETRO_DEVICE_ID_POINTER_X         0
#define RETRO_DEVICE_ID_POINTER_Y         1
#define RETRO_DEVICE_ID_POINTER_PRESSED   2

/* Id values for LANGUAGE */
enum retro_language
{
   RETRO_LANGUAGE_ENGLISH             = 0,
   RETRO_LANGUAGE_JAPANESE            = 1,
   RETRO_LANGUAGE_FRENCH              = 2,
   RETRO_LANGUAGE_SPANISH             = 3,
   RETRO_LANGUAGE_GERMAN              = 4,
   RETRO_LANGUAGE_ITALIAN             = 5,
   RETRO_LANGUAGE_DUTCH               = 6,
   RETRO_LANGUAGE_PORTUGUESE_BRAZIL   = 7,
   RETRO_LANGUAGE_PORTUGUESE_PORTUGAL = 8,
   RETRO_LANGUAGE_RUSSIAN             = 9,
   RETRO_LANGUAGE_KOREAN              = 10,
   RETRO_LANGUAGE_CHINESE_TRADITIONAL = 11,
   RETRO_LANGUAGE_CHINESE_SIMPLIFIED  = 12,
   RETRO_LANGUAGE_ESPERANTO           = 13,
   RETRO_LANGUAGE_POLISH              = 14,
   RETRO_LANGUAGE_VIETNAMESE          = 15,
   RETRO_LANGUAGE_ARABIC              = 16,
   RETRO_LANGUAGE_LAST,

   /* Ensure sizeof(enum) == sizeof(int) */
   RETRO_LANGUAGE_DUMMY          = INT_MAX
};

/* Keysyms used for ID in input state callback when polling RETRO_KEYBOARD. */
enum retro_key
{
   RETROK_UNKNOWN        = 0,
   RETROK_FIRST          = 0,
   RETROK_BACKSPACE      = 8,
   RETROK_TAB            = 9,
   RETROK_CLEAR          = 12,
   RETROK_RETURN         = 13,
   RETROK_PAUSE          = 19,
   RETROK_ESCAPE         = 27,
   RETROK_SPACE          = 32,
   RETROK_EXCLAIM        = 33,
   RETROK_QUOTEDBL       = 34,
   RETROK_HASH           = 35,
   RETROK_DOLLAR         = 36,
   RETROK_AMPERSAND      = 38,
   RETROK_QUOTE          = 39,
   RETROK_LEFTPAREN      = 40,
   RETROK_RIGHTPAREN     = 41,
   RETROK_ASTERISK       = 42,
   RETROK_PLUS           = 43,
   RETROK_COMMA          = 44,
   RETROK_MINUS          = 45,
   RETROK_PERIOD         = 46,
   RETROK_SLASH          = 47,
   RETROK_0              = 48,
   RETROK_1              = 49,
   RETROK_2              = 50,
   RETROK_3              = 51,
   RETROK_4              = 52,
   RETROK_5              = 53,
   RETROK_6              = 54,
   RETROK_7              = 55,
   RETROK_8              = 56,
   RETROK_9              = 57,
   RETROK_COLON          = 58,
   RETROK_SEMICOLON      = 59,
   RETROK_LESS           = 60,
   RETROK_EQUALS         = 61,
   RETROK_GREATER        = 62,
   RETROK_QUESTION       = 63,
   RETROK_AT             = 64,
   RETROK_LEFTBRACKET    = 91,
   RETROK_BACKSLASH      = 92,
   RETROK_RIGHTBRACKET   = 93,
   RETROK_CARET          = 94,
   RETROK_UNDERSCORE     = 95,
   RETROK_BACKQUOTE      = 96,
   RETROK_a              = 97,
   RETROK_b              = 98,
   RETROK_c              = 99,
   RETROK_d              = 100,
   RETROK_e              = 101,
   RETROK_f              = 102,
   RETROK_g              = 103,
   RETROK_h              = 104,
   RETROK_i              = 105,
   RETROK_j              = 106,
   RETROK_k              = 107,
   RETROK_l              = 108,
   RETROK_m              = 109,
   RETROK_n              = 110,
   RETROK_o              = 111,
   RETROK_p              = 112,
   RETROK_q              = 113,
   RETROK_r              = 114,
   RETROK_s              = 115,
   RETROK_t              = 116,
   RETROK_u              = 117,
   RETROK_v              = 118,
   RETROK_w              = 119,
   RETROK_x              = 120,
   RETROK_y              = 121,
   RETROK_z              = 122,
   RETROK_LEFTBRACE      = 123,
   RETROK_BAR            = 124,
   RETROK_RIGHTBRACE     = 125,
   RETROK_TILDE          = 126,
   RETROK_DELETE         = 127,

   RETROK_KP0            = 256,
   RETROK_KP1            = 257,
   RETROK_KP2            = 258,
   RETROK_KP3            = 259,
   RETROK_KP4            = 260,
   RETROK_KP5            = 261,
   RETROK_KP6            = 262,
   RETROK_KP7            = 263,
   RETROK_KP8            = 264,
   RETROK_KP9            = 265,
   RETROK_KP_PERIOD      = 266,
   RETROK_KP_DIVIDE      = 267,
   RETROK_KP_MULTIPLY    = 268,
   RETROK_KP_MINUS       = 269,
   RETROK_KP_PLUS        = 270,
   RETROK_KP_ENTER       = 271,
   RETROK_KP_EQUALS      = 272,

   RETROK_UP             = 273,
   RETROK_DOWN           = 274,
   RETROK_RIGHT          = 275,
   RETROK_LEFT           = 276,
   RETROK_INSERT         = 277,
   RETROK_HOME           = 278,
   RETROK_END            = 279,
   RETROK_PAGEUP         = 280,
   RETROK_PAGEDOWN       = 281,

   RETROK_F1             = 282,
   RETROK_F2             = 283,
   RETROK_F3             = 284,
   RETROK_F4             = 285,
   RETROK_F5             = 286,
   RETROK_F6             = 287,
   RETROK_F7             = 288,
   RETROK_F8             = 289,
   RETROK_F9             = 290,
   RETROK_F10            = 291,
   RETROK_F11            = 292,
   RETROK_F12            = 293,
   RETROK_F13            = 294,
   RETROK_F14            = 295,
   RETROK_F15            = 296,

   RETROK_NUMLOCK        = 300,
   RETROK_CAPSLOCK       = 301,
   RETROK_SCROLLOCK      = 302,
   RETROK_RSHIFT         = 303,
   RETROK_LSHIFT         = 304,
   RETROK_RCTRL          = 305,
   RETROK_LCTRL          = 306,
   RETROK_RALT           = 307,
   RETROK_LALT           = 308,
   RETROK_RMETA          = 309,
   RETROK_LMETA          = 310,
   RETROK_LSUPER         = 311,
   RETROK_RSUPER         = 312,

   RETROK_HELP           = 315,
   RETROK_PRINT          = 316,
   RETROK_SYSREQ         = 317,
   RETROK_BREAK          = 318,
   RETROK_MENU           = 319,
   RETROK_EURO           = 321,
   RETROK_UNDO           = 322,

   RETROK_LAST,

   RETROK_DUMMY          = INT_MAX /* Ensure sizeof(enum) == sizeof(int) */
};

enum retro_mod
{
   RETROKMOD_NONE       = 0x0000,

   RETROKMOD_SHIFT      = 0x01,
   RETROKMOD_CTRL       = 0x02,
   RETROKMOD_ALT        = 0x04,
   RETROKMOD_META       = 0x08,

   RETROKMOD_NUMLOCK    = 0x10,
   RETROKMOD_CAPSLOCK   = 0x20,
   RETROKMOD_SCROLLOCK  = 0x40,

   RETROKMOD_DUMMY = INT_MAX /* Ensure sizeof(enum) == sizeof(int) */
};

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
                                            * This should be called the first time as early as
                                            * possible (ideally in retro_set_environment).
                                            * Afterward it may be called again for the core to communicate
                                            * updated options to the frontend, but the number of core
                                            * options must not change from the number in the initial call.
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
#define RETRO_ENVIRONMENT_GET_LOG_INTERFACE 27
                                           /* struct retro_log_callback * --
                                            * Gets an interface for logging. This is useful for
                                            * logging in a cross-platform way
                                            * as certain platforms cannot use stderr for logging.
                                            * It also allows the frontend to
                                            * show logging information in a more suitable way.
                                            * If this interface is not used, libretro cores should
                                            * log to stderr as desired.
                                            */
#define RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY 30 /* Old name, kept for compatibility. */
#define RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY 30
                                           /* const char ** --
                                            * Returns the "core assets" directory of the frontend.
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

typedef void (RETRO_CALLCONV *retro_proc_address_t)(void);

enum retro_log_level
{
   RETRO_LOG_DEBUG = 0,
   RETRO_LOG_INFO,
   RETRO_LOG_WARN,
   RETRO_LOG_ERROR,

   RETRO_LOG_DUMMY = INT_MAX
};

/* Logging function. Takes log level argument as well. */
typedef void (RETRO_CALLCONV *retro_log_printf_t)(enum retro_log_level level,
      const char *fmt, ...);

struct retro_log_callback
{
   retro_log_printf_t log;
};

enum retro_rumble_effect
{
   RETRO_RUMBLE_STRONG = 0,
   RETRO_RUMBLE_WEAK = 1,

   RETRO_RUMBLE_DUMMY = INT_MAX
};

/* Sets rumble state for joypad plugged in port 'port'.
 * Rumble effects are controlled independently,
 * and setting e.g. strong rumble does not override weak rumble.
 * Strength has a range of [0, 0xffff].
 *
 * Returns true if rumble state request was honored. */
typedef bool (RETRO_CALLCONV *retro_set_rumble_state_t)(unsigned port,
      enum retro_rumble_effect effect, uint16_t strength);

struct retro_rumble_interface
{
   retro_set_rumble_state_t set_rumble_state;
};

/* Pass this to retro_video_refresh_t if rendering to hardware.
 * Passing NULL to retro_video_refresh_t is still a frame dupe as normal.
 * */
#define RETRO_HW_FRAME_BUFFER_VALID ((void*)-1)

/* Invalidates the current HW context.
 * Any GL state is lost, and must not be deinitialized explicitly.
 * If explicit deinitialization is desired by the libretro core,
 * it should implement context_destroy callback.
 * If called, all GPU resources must be reinitialized.
 * Usually called when frontend reinits video driver.
 * Also called first time video driver is initialized,
 * allowing libretro core to initialize resources.
 */
typedef void (RETRO_CALLCONV *retro_hw_context_reset_t)(void);

/* Gets current framebuffer which is to be rendered to.
 * Could change every frame potentially.
 */
typedef uintptr_t (RETRO_CALLCONV *retro_hw_get_current_framebuffer_t)(void);

/* Get a symbol from HW context. */
typedef retro_proc_address_t (RETRO_CALLCONV *retro_hw_get_proc_address_t)(const char *sym);

enum retro_hw_context_type
{
   RETRO_HW_CONTEXT_NONE             = 0,
   /* OpenGL 2.x. Driver can choose to use latest compatibility context. */
   RETRO_HW_CONTEXT_OPENGL           = 1,
   /* OpenGL ES 2.0. */
   RETRO_HW_CONTEXT_OPENGLES2        = 2,
   /* Modern desktop core GL context. Use version_major/
    * version_minor fields to set GL version. */
   RETRO_HW_CONTEXT_OPENGL_CORE      = 3,
   /* OpenGL ES 3.0 */
   RETRO_HW_CONTEXT_OPENGLES3        = 4,
   /* OpenGL ES 3.1+. Set version_major/version_minor. For GLES2 and GLES3,
    * use the corresponding enums directly. */
   RETRO_HW_CONTEXT_OPENGLES_VERSION = 5,

   /* Vulkan, see RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE. */
   RETRO_HW_CONTEXT_VULKAN           = 6,

   /* Direct3D, set version_major to select the type of interface
    * returned by RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE */
   RETRO_HW_CONTEXT_DIRECT3D         = 7,

   RETRO_HW_CONTEXT_DUMMY = INT_MAX
};

struct retro_hw_render_callback
{
   /* Which API to use. Set by libretro core. */
   enum retro_hw_context_type context_type;

   /* Called when a context has been created or when it has been reset.
    * An OpenGL context is only valid after context_reset() has been called.
    *
    * When context_reset is called, OpenGL resources in the libretro
    * implementation are guaranteed to be invalid.
    *
    * It is possible that context_reset is called multiple times during an
    * application lifecycle.
    * If context_reset is called without any notification (context_destroy),
    * the OpenGL context was lost and resources should just be recreated
    * without any attempt to "free" old resources.
    */
   retro_hw_context_reset_t context_reset;

   /* Set by frontend.
    * TODO: This is rather obsolete. The frontend should not
    * be providing preallocated framebuffers. */
   retro_hw_get_current_framebuffer_t get_current_framebuffer;

   /* Set by frontend.
    * Can return all relevant functions, including glClear on Windows. */
   retro_hw_get_proc_address_t get_proc_address;

   /* Set if render buffers should have depth component attached.
    * TODO: Obsolete. */
   bool depth;

   /* Set if stencil buffers should be attached.
    * TODO: Obsolete. */
   bool stencil;

   /* If depth and stencil are true, a packed 24/8 buffer will be added.
    * Only attaching stencil is invalid and will be ignored. */

   /* Use conventional bottom-left origin convention. If false,
    * standard libretro top-left origin semantics are used.
    * TODO: Move to GL specific interface. */
   bool bottom_left_origin;

   /* Major version number for core GL context or GLES 3.1+. */
   unsigned version_major;

   /* Minor version number for core GL context or GLES 3.1+. */
   unsigned version_minor;

   /* If this is true, the frontend will go very far to avoid
    * resetting context in scenarios like toggling fullscreen, etc.
    * TODO: Obsolete? Maybe frontend should just always assume this ...
    */
   bool cache_context;

   /* The reset callback might still be called in extreme situations
    * such as if the context is lost beyond recovery.
    *
    * For optimal stability, set this to false, and allow context to be
    * reset at any time.
    */

   /* A callback to be called before the context is destroyed in a
    * controlled way by the frontend. */
   retro_hw_context_reset_t context_destroy;

   /* OpenGL resources can be deinitialized cleanly at this step.
    * context_destroy can be set to NULL, in which resources will
    * just be destroyed without any notification.
    *
    * Even when context_destroy is non-NULL, it is possible that
    * context_reset is called without any destroy notification.
    * This happens if context is lost by external factors (such as
    * notified by GL_ARB_robustness).
    *
    * In this case, the context is assumed to be already dead,
    * and the libretro implementation must not try to free any OpenGL
    * resources in the subsequent context_reset.
    */

   /* Creates a debug context. */
   bool debug_context;
};

/* Callback type passed in RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK.
 * Called by the frontend in response to keyboard events.
 * down is set if the key is being pressed, or false if it is being released.
 * keycode is the RETROK value of the char.
 * character is the text character of the pressed key. (UTF-32).
 * key_modifiers is a set of RETROKMOD values or'ed together.
 *
 * The pressed/keycode state can be indepedent of the character.
 * It is also possible that multiple characters are generated from a
 * single keypress.
 * Keycode events should be treated separately from character events.
 * However, when possible, the frontend should try to synchronize these.
 * If only a character is posted, keycode should be RETROK_UNKNOWN.
 *
 * Similarily if only a keycode event is generated with no corresponding
 * character, character should be 0.
 */
typedef void (RETRO_CALLCONV *retro_keyboard_event_t)(bool down, unsigned keycode,
      uint32_t character, uint16_t key_modifiers);

struct retro_keyboard_callback
{
   retro_keyboard_event_t callback;
};

/* Callbacks for RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE.
 * Should be set for implementations which can swap out multiple disk
 * images in runtime.
 *
 * If the implementation can do this automatically, it should strive to do so.
 * However, there are cases where the user must manually do so.
 *
 * Overview: To swap a disk image, eject the disk image with
 * set_eject_state(true).
 * Set the disk index with set_image_index(index). Insert the disk again
 * with set_eject_state(false).
 */

/* If ejected is true, "ejects" the virtual disk tray.
 * When ejected, the disk image index can be set.
 */
typedef bool (RETRO_CALLCONV *retro_set_eject_state_t)(bool ejected);

/* Gets current eject state. The initial state is 'not ejected'. */
typedef bool (RETRO_CALLCONV *retro_get_eject_state_t)(void);

/* Gets current disk index. First disk is index 0.
 */
typedef unsigned (RETRO_CALLCONV *retro_get_image_index_t)(void);

/* Sets image index. Can only be called when disk is ejected.
 */
typedef bool (RETRO_CALLCONV *retro_set_image_index_t)(unsigned index);

/* Gets total number of images which are available to use. */
typedef unsigned (RETRO_CALLCONV *retro_get_num_images_t)(void);

struct retro_game_info;

/* Replaces the disk image associated with index.
 * Arguments to pass in info have same requirements as retro_load_game().
 * Virtual disk tray must be ejected when calling this.
 *
 * Replacing a disk image with info = NULL will remove the disk image
 * from the internal list.
 * As a result, calls to get_image_index() can change.
 *
 * E.g. replace_image_index(1, NULL), and previous get_image_index()
 * returned 4 before.
 * Index 1 will be removed, and the new index is 3.
 */
typedef bool (RETRO_CALLCONV *retro_replace_image_index_t)(unsigned index,
      const struct retro_game_info *info);

/* Adds a new valid index (get_num_images()) to the internal disk list.
 * This will increment subsequent return values from get_num_images() by 1.
 * This image index cannot be used until a disk image has been set
 * with replace_image_index. */
typedef bool (RETRO_CALLCONV *retro_add_image_index_t)(void);

struct retro_disk_control_callback
{
   retro_set_eject_state_t set_eject_state;
   retro_get_eject_state_t get_eject_state;

   retro_get_image_index_t get_image_index;
   retro_set_image_index_t set_image_index;
   retro_get_num_images_t  get_num_images;

   retro_replace_image_index_t replace_image_index;
   retro_add_image_index_t add_image_index;
};

enum retro_pixel_format
{
   /* 0RGB1555, native endian.
    * 0 bit must be set to 0.
    * This pixel format is default for compatibility concerns only.
    * If a 15/16-bit pixel format is desired, consider using RGB565. */
   RETRO_PIXEL_FORMAT_0RGB1555 = 0,

   /* XRGB8888, native endian.
    * X bits are ignored. */
   RETRO_PIXEL_FORMAT_XRGB8888 = 1,

   /* RGB565, native endian.
    * This pixel format is the recommended format to use if a 15/16-bit
    * format is desired as it is the pixel format that is typically
    * available on a wide range of low-power devices.
    *
    * It is also natively supported in APIs like OpenGL ES. */
   RETRO_PIXEL_FORMAT_RGB565   = 2,

   /* Ensure sizeof() == sizeof(int). */
   RETRO_PIXEL_FORMAT_UNKNOWN  = INT_MAX
};

struct retro_message
{
   const char *msg;        /* Message to be displayed. */
   unsigned    frames;     /* Duration in frames of message. */
};

/* Describes how the libretro implementation maps a libretro input bind
 * to its internal input system through a human readable string.
 * This string can be used to better let a user configure input. */
struct retro_input_descriptor
{
   /* Associates given parameters with a description. */
   unsigned port;
   unsigned device;
   unsigned index;
   unsigned id;

   /* Human readable description for parameters.
    * The pointer must remain valid until
    * retro_unload_game() is called. */
   const char *description;
};

struct retro_system_info
{
   /* All pointers are owned by libretro implementation, and pointers must
    * remain valid until retro_deinit() is called. */

   const char *library_name;      /* Descriptive name of library. Should not
                                   * contain any version numbers, etc. */
   const char *library_version;   /* Descriptive version of core. */

   const char *valid_extensions;  /* A string listing probably content
                                   * extensions the core will be able to
                                   * load, separated with pipe.
                                   * I.e. "bin|rom|iso".
                                   * Typically used for a GUI to filter
                                   * out extensions. */

   /* If true, retro_load_game() is guaranteed to provide a valid pathname
    * in retro_game_info::path.
    * ::data and ::size are both invalid.
    *
    * If false, ::data and ::size are guaranteed to be valid, but ::path
    * might not be valid.
    *
    * This is typically set to true for libretro implementations that must
    * load from file.
    * Implementations should strive for setting this to false, as it allows
    * the frontend to perform patching, etc. */
   bool        need_fullpath;

   bool        block_extract;
};

struct retro_game_geometry
{
   unsigned base_width;    /* Nominal video width of game. */
   unsigned base_height;   /* Nominal video height of game. */
   unsigned max_width;     /* Maximum possible width of game. */
   unsigned max_height;    /* Maximum possible height of game. */

   float    aspect_ratio;  /* Nominal aspect ratio of game. If
                            * aspect_ratio is <= 0.0, an aspect ratio
                            * of base_width / base_height is assumed.
                            * A frontend could override this setting,
                            * if desired. */
};

struct retro_system_timing
{
   double fps;             /* FPS of video content. */
   double sample_rate;     /* Sampling rate of audio. */
};

struct retro_system_av_info
{
   struct retro_game_geometry geometry;
   struct retro_system_timing timing;
};

struct retro_variable
{
   /* Variable to query in RETRO_ENVIRONMENT_GET_VARIABLE.
    * If NULL, obtains the complete environment string if more
    * complex parsing is necessary.
    * The environment string is formatted as key-value pairs
    * delimited by semicolons as so:
    * "key1=value1;key2=value2;..."
    */
   const char *key;

   /* Value to be obtained. If key does not exist, it is set to NULL. */
   const char *value;
};

struct retro_game_info
{
   const char *path;       /* Path to game, UTF-8 encoded.
                            * Sometimes used as a reference for building other paths.
                            * May be NULL if game was loaded from stdin or similar,
                            * but in this case some cores will be unable to load `data`.
                            * So, it is preferable to fabricate something here instead
                            * of passing NULL, which will help more cores to succeed.
                            * retro_system_info::need_fullpath requires
                            * that this path is valid. */
   const void *data;       /* Memory buffer of loaded game. Will be NULL
                            * if need_fullpath was set. */
   size_t      size;       /* Size of memory buffer. */
   const char *meta;       /* String of implementation specific meta-data. */
};

/* Callbacks */

/* Environment callback. Gives implementations a way of performing
 * uncommon tasks. Extensible. */
typedef bool (RETRO_CALLCONV *retro_environment_t)(unsigned cmd, void *data);

/* Render a frame. Pixel format is 15-bit 0RGB1555 native endian
 * unless changed (see RETRO_ENVIRONMENT_SET_PIXEL_FORMAT).
 *
 * Width and height specify dimensions of buffer.
 * Pitch specifices length in bytes between two lines in buffer.
 *
 * For performance reasons, it is highly recommended to have a frame
 * that is packed in memory, i.e. pitch == width * byte_per_pixel.
 * Certain graphic APIs, such as OpenGL ES, do not like textures
 * that are not packed in memory.
 */
typedef void (RETRO_CALLCONV *retro_video_refresh_t)(const void *data, unsigned width,
      unsigned height, size_t pitch);

/* Renders a single audio frame. Should only be used if implementation
 * generates a single sample at a time.
 * Format is signed 16-bit native endian.
 */
typedef void (RETRO_CALLCONV *retro_audio_sample_t)(int16_t left, int16_t right);

/* Renders multiple audio frames in one go.
 *
 * One frame is defined as a sample of left and right channels, interleaved.
 * I.e. int16_t buf[4] = { l, r, l, r }; would be 2 frames.
 * Only one of the audio callbacks must ever be used.
 */
typedef size_t (RETRO_CALLCONV *retro_audio_sample_batch_t)(const int16_t *data,
      size_t frames);

/* Polls input. */
typedef void (RETRO_CALLCONV *retro_input_poll_t)(void);

/* Queries for input for player 'port'. device will be masked with
 * RETRO_DEVICE_MASK.
 *
 * Specialization of devices such as RETRO_DEVICE_JOYPAD_MULTITAP that
 * have been set with retro_set_controller_port_device()
 * will still use the higher level RETRO_DEVICE_JOYPAD to request input.
 */
typedef int16_t (RETRO_CALLCONV *retro_input_state_t)(unsigned port, unsigned device,
      unsigned index, unsigned id);

/* Sets callbacks. retro_set_environment() is guaranteed to be called
 * before retro_init().
 *
 * The rest of the set_* functions are guaranteed to have been called
 * before the first call to retro_run() is made. */
RETRO_API void retro_set_environment(retro_environment_t);
RETRO_API void retro_set_video_refresh(retro_video_refresh_t);
RETRO_API void retro_set_audio_sample(retro_audio_sample_t);
RETRO_API void retro_set_audio_sample_batch(retro_audio_sample_batch_t);
RETRO_API void retro_set_input_poll(retro_input_poll_t);
RETRO_API void retro_set_input_state(retro_input_state_t);

/* Library global initialization/deinitialization. */
RETRO_API void retro_init(void);
RETRO_API void retro_deinit(void);

/* Must return RETRO_API_VERSION. Used to validate ABI compatibility
 * when the API is revised. */
RETRO_API unsigned retro_api_version(void);

/* Gets statically known system info. Pointers provided in *info
 * must be statically allocated.
 * Can be called at any time, even before retro_init(). */
RETRO_API void retro_get_system_info(struct retro_system_info *info);

/* Gets information about system audio/video timings and geometry.
 * Can be called only after retro_load_game() has successfully completed.
 * NOTE: The implementation of this function might not initialize every
 * variable if needed.
 * E.g. geom.aspect_ratio might not be initialized if core doesn't
 * desire a particular aspect ratio. */
RETRO_API void retro_get_system_av_info(struct retro_system_av_info *info);

/* Sets device to be used for player 'port'.
 * Setting a particular device type is not a guarantee that libretro cores
 * will only poll input based on that particular device type. It is only a
 * hint to the libretro core when a core cannot automatically detect the
 * appropriate input device type on its own. It is also relevant when a
 * core can change its behavior depending on device type. */
RETRO_API void retro_set_controller_port_device(unsigned port, unsigned device);

/* Resets the current game. */
RETRO_API void retro_reset(void);

/* Runs the game for one video frame.
 * During retro_run(), input_poll callback must be called at least once.
 *
 * If a frame is not rendered for reasons where a game "dropped" a frame,
 * this still counts as a frame, and retro_run() should explicitly dupe
 * a frame if GET_CAN_DUPE returns true.
 * In this case, the video callback can take a NULL argument for data.
 */
RETRO_API void retro_run(void);

/* Returns the amount of data the implementation requires to serialize
 * internal state (save states).
 * Between calls to retro_load_game() and retro_unload_game(), the
 * returned size is never allowed to be larger than a previous returned
 * value, to ensure that the frontend can allocate a save state buffer once.
 */
RETRO_API size_t retro_serialize_size(void);

/* Serializes internal state. If failed, or size is lower than
 * retro_serialize_size(), it should return false, true otherwise. */
RETRO_API bool retro_serialize(void *data, size_t size);
RETRO_API bool retro_unserialize(const void *data, size_t size);

/* Loads a game. */
RETRO_API bool retro_load_game(const struct retro_game_info *game);

/* Unloads a currently loaded game. */
RETRO_API void retro_unload_game(void);

/* Gets region of memory. */
RETRO_API void *retro_get_memory_data(unsigned id);
RETRO_API size_t retro_get_memory_size(unsigned id);

#ifdef __cplusplus
}
#endif

#endif
