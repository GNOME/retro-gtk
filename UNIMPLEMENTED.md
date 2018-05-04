# Unimplemented

retro-gtk is a Libretro frontend which reimplements and uses the Libretro API.
This file lists parts of the Libretro API which are not implemented by
retro-gtk, please update this file when such features are implemented or when
updating the reference retro-gtk/libretro.h.

This has been updated to RetroArch 1.7.2.

## Achievements

The achievements support accessor is unimplemented.

```
#define RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS (42 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* const bool * --
                                            * If true, the libretro implementation supports achievements
                                            * either via memory descriptors set with RETRO_ENVIRONMENT_SET_MEMORY_MAPS
                                            * or via retro_get_memory_data/retro_get_memory_size.
                                            *
                                            * This must be called before the first call to retro_run.
                                            */
```

## Analog Buttons

The analog buttons are unimplemented.

```
/* The ANALOG device is an extension to JOYPAD (RetroPad).
 * Similar to DualShock2 it adds two analog sticks and all buttons can
 * be analog. This is treated as a separate device type as it returns
 * axis values in the full analog range of [-0x8000, 0x7fff].
 * Positive X axis is right. Positive Y axis is down.
 * Buttons are returned in the range [0, 0x7fff].
 * Only use ANALOG type when polling for analog values.
 */
#define RETRO_DEVICE_ANALOG       5

#define RETRO_DEVICE_INDEX_ANALOG_BUTTON     2
```

## Audio Callback

The audio callback system is unimplemented.

```
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

/* Notifies libretro that audio data should be written. */
typedef void (RETRO_CALLCONV *retro_audio_callback_t)(void);

/* True: Audio driver in frontend is active, and callback is
 * expected to be called regularily.
 * False: Audio driver in frontend is paused or inactive.
 * Audio callback will not be called until set_state has been
 * called with true.
 * Initial state is false (inactive).
 */
typedef void (RETRO_CALLCONV *retro_audio_set_state_callback_t)(bool enabled);

struct retro_audio_callback
{
   retro_audio_callback_t callback;
   retro_audio_set_state_callback_t set_state;
};
```

## Audio/Video Enablement

The audio/video enablement system is unimplemented.

```
#define RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE (47 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* int * --
                                            * Tells the core if the frontend wants audio or video.
                                            * If disabled, the frontend will discard the audio or video,
                                            * so the core may decide to skip generating a frame or generating audio.
                                            * This is mainly used for increasing performance.
                                            * Bit 0 (value 1): Enable Video
                                            * Bit 1 (value 2): Enable Audio
                                            * Bit 2 (value 4): Use Fast Savestates.
                                            * Bit 3 (value 8): Hard Disable Audio
                                            * Other bits are reserved for future use and will default to zero.
                                            * If video is disabled:
                                            * * The frontend wants the core to not generate any video,
                                            *   including presenting frames via hardware acceleration.
                                            * * The frontend's video frame callback will do nothing.
                                            * * After running the frame, the video output of the next frame should be
                                            *   no different than if video was enabled, and saving and loading state
                                            *   should have no issues.
                                            * If audio is disabled:
                                            * * The frontend wants the core to not generate any audio.
                                            * * The frontend's audio callbacks will do nothing.
                                            * * After running the frame, the audio output of the next frame should be
                                            *   no different than if audio was enabled, and saving and loading state
                                            *   should have no issues.
                                            * Fast Savestates:
                                            * * Guaranteed to be created by the same binary that will load them.
                                            * * Will not be written to or read from the disk.
                                            * * Suggest that the core assumes loading state will succeed.
                                            * * Suggest that the core updates its memory buffers in-place if possible.
                                            * * Suggest that the core skips clearing memory.
                                            * * Suggest that the core skips resetting the system.
                                            * * Suggest that the core may skip validation steps.
                                            * Hard Disable Audio:
                                            * * Used for a secondary core when running ahead.
                                            * * Indicates that the frontend will never need audio from the core.
                                            * * Suggests that the core may stop synthesizing audio, but this should not
                                            *   compromise emulation accuracy.
                                            * * Audio output for the next frame does not matter, and the frontend will
                                            *   never need an accurate audio state in the future.
                                            * * State will never be saved when using Hard Disable Audio.
                                            */
```

## Camera

The camera system is unimplemented.

```
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

enum retro_camera_buffer
{
   RETRO_CAMERA_BUFFER_OPENGL_TEXTURE = 0,
   RETRO_CAMERA_BUFFER_RAW_FRAMEBUFFER,

   RETRO_CAMERA_BUFFER_DUMMY = INT_MAX
};

/* Starts the camera driver. Can only be called in retro_run(). */
typedef bool (RETRO_CALLCONV *retro_camera_start_t)(void);

/* Stops the camera driver. Can only be called in retro_run(). */
typedef void (RETRO_CALLCONV *retro_camera_stop_t)(void);

/* Callback which signals when the camera driver is initialized
 * and/or deinitialized.
 * retro_camera_start_t can be called in initialized callback.
 */
typedef void (RETRO_CALLCONV *retro_camera_lifetime_status_t)(void);

/* A callback for raw framebuffer data. buffer points to an XRGB8888 buffer.
 * Width, height and pitch are similar to retro_video_refresh_t.
 * First pixel is top-left origin.
 */
typedef void (RETRO_CALLCONV *retro_camera_frame_raw_framebuffer_t)(const uint32_t *buffer,
      unsigned width, unsigned height, size_t pitch);

/* A callback for when OpenGL textures are used.
 *
 * texture_id is a texture owned by camera driver.
 * Its state or content should be considered immutable, except for things like
 * texture filtering and clamping.
 *
 * texture_target is the texture target for the GL texture.
 * These can include e.g. GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE, and possibly
 * more depending on extensions.
 *
 * affine points to a packed 3x3 column-major matrix used to apply an affine
 * transform to texture coordinates. (affine_matrix * vec3(coord_x, coord_y, 1.0))
 * After transform, normalized texture coord (0, 0) should be bottom-left
 * and (1, 1) should be top-right (or (width, height) for RECTANGLE).
 *
 * GL-specific typedefs are avoided here to avoid relying on gl.h in
 * the API definition.
 */
typedef void (RETRO_CALLCONV *retro_camera_frame_opengl_texture_t)(unsigned texture_id,
      unsigned texture_target, const float *affine);

struct retro_camera_callback
{
   /* Set by libretro core.
    * Example bitmask: caps = (1 << RETRO_CAMERA_BUFFER_OPENGL_TEXTURE) | (1 << RETRO_CAMERA_BUFFER_RAW_FRAMEBUFFER).
    */
   uint64_t caps;

   /* Desired resolution for camera. Is only used as a hint. */
   unsigned width;
   unsigned height;

   /* Set by frontend. */
   retro_camera_start_t start;
   retro_camera_stop_t stop;

   /* Set by libretro core if raw framebuffer callbacks will be used. */
   retro_camera_frame_raw_framebuffer_t frame_raw_framebuffer;

   /* Set by libretro core if OpenGL texture callbacks will be used. */
   retro_camera_frame_opengl_texture_t frame_opengl_texture;

   /* Set by libretro core. Called after camera driver is initialized and
    * ready to be started.
    * Can be NULL, in which this callback is not called.
    */
   retro_camera_lifetime_status_t initialized;

   /* Set by libretro core. Called right before camera driver is
    * deinitialized.
    * Can be NULL, in which this callback is not called.
    */
   retro_camera_lifetime_status_t deinitialized;
};
```

## Cheat

The cheat system is unimplemented.

```
RETRO_API void retro_cheat_reset(void);
RETRO_API void retro_cheat_set(unsigned index, bool enabled, const char *code);
```

## Controller Info

The controller info system is unimplemented.

```
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

struct retro_controller_description
{
   /* Human-readable description of the controller. Even if using a generic
    * input device type, this can be set to the particular device type the
    * core uses. */
   const char *desc;

   /* Device type passed to retro_set_controller_port_device(). If the device
    * type is a sub-class of a generic input device type, use the
    * RETRO_DEVICE_SUBCLASS macro to create an ID.
    *
    * E.g. RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 1). */
   unsigned id;
};

struct retro_controller_info
{
   const struct retro_controller_description *types;
   unsigned num_types;
};
```

## Device Subclassing

The device subclassing system is unimplemented.

```
#define RETRO_DEVICE_TYPE_SHIFT         8
#define RETRO_DEVICE_MASK               ((1 << RETRO_DEVICE_TYPE_SHIFT) - 1)
#define RETRO_DEVICE_SUBCLASS(base, id) (((id + 1) << RETRO_DEVICE_TYPE_SHIFT) | base)
```

## Framebuffer

The framebuffer system is unimplemented.

```
#define RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER (40 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* struct retro_framebuffer * --
                                            * Returns a preallocated framebuffer which the core can use for rendering
                                            * the frame into when not using SET_HW_RENDER.
                                            * The framebuffer returned from this call must not be used
                                            * after the current call to retro_run() returns.
                                            *
                                            * The goal of this call is to allow zero-copy behavior where a core
                                            * can render directly into video memory, avoiding extra bandwidth cost by copying
                                            * memory from core to video memory.
                                            *
                                            * If this call succeeds and the core renders into it,
                                            * the framebuffer pointer and pitch can be passed to retro_video_refresh_t.
                                            * If the buffer from GET_CURRENT_SOFTWARE_FRAMEBUFFER is to be used,
                                            * the core must pass the exact
                                            * same pointer as returned by GET_CURRENT_SOFTWARE_FRAMEBUFFER;
                                            * i.e. passing a pointer which is offset from the
                                            * buffer is undefined. The width, height and pitch parameters
                                            * must also match exactly to the values obtained from GET_CURRENT_SOFTWARE_FRAMEBUFFER.
                                            *
                                            * It is possible for a frontend to return a different pixel format
                                            * than the one used in SET_PIXEL_FORMAT. This can happen if the frontend
                                            * needs to perform conversion.
                                            *
                                            * It is still valid for a core to render to a different buffer
                                            * even if GET_CURRENT_SOFTWARE_FRAMEBUFFER succeeds.
                                            *
                                            * A frontend must make sure that the pointer obtained from this function is
                                            * writeable (and readable).
                                            */

#define RETRO_MEMORY_ACCESS_WRITE (1 << 0)
   /* The core will write to the buffer provided by retro_framebuffer::data. */
#define RETRO_MEMORY_ACCESS_READ (1 << 1)
   /* The core will read from retro_framebuffer::data. */
#define RETRO_MEMORY_TYPE_CACHED (1 << 0)
   /* The memory in data is cached.
    * If not cached, random writes and/or reading from the buffer is expected to be very slow. */
struct retro_framebuffer
{
   void *data;                      /* The framebuffer which the core can render into.
                                       Set by frontend in GET_CURRENT_SOFTWARE_FRAMEBUFFER.
                                       The initial contents of data are unspecified. */
   unsigned width;                  /* The framebuffer width used by the core. Set by core. */
   unsigned height;                 /* The framebuffer height used by the core. Set by core. */
   size_t pitch;                    /* The number of bytes between the beginning of a scanline,
                                       and beginning of the next scanline.
                                       Set by frontend in GET_CURRENT_SOFTWARE_FRAMEBUFFER. */
   enum retro_pixel_format format;  /* The pixel format the core must use to render into data.
                                       This format could differ from the format used in
                                       SET_PIXEL_FORMAT.
                                       Set by frontend in GET_CURRENT_SOFTWARE_FRAMEBUFFER. */

   unsigned access_flags;           /* How the core will access the memory in the framebuffer.
                                       RETRO_MEMORY_ACCESS_* flags.
                                       Set by core. */
   unsigned memory_flags;           /* Flags telling core how the memory has been mapped.
                                       RETRO_MEMORY_TYPE_* flags.
                                       Set by frontend in GET_CURRENT_SOFTWARE_FRAMEBUFFER. */
};
```

## Frame Time

The frame time system is unimplemented.

```
#define RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK 21
                                           /* const struct retro_frame_time_callback * --
                                            * Lets the core know how much time has passed since last
                                            * invocation of retro_run().
                                            * The frontend can tamper with the timing to fake fast-forward,
                                            * slow-motion, frame stepping, etc.
                                            * In this case the delta time will use the reference value
                                            * in frame_time_callback..
                                            */

/* Notifies a libretro core of time spent since last invocation
 * of retro_run() in microseconds.
 *
 * It will be called right before retro_run() every frame.
 * The frontend can tamper with timing to support cases like
 * fast-forward, slow-motion and framestepping.
 *
 * In those scenarios the reference frame time value will be used. */
typedef int64_t retro_usec_t;
typedef void (RETRO_CALLCONV *retro_frame_time_callback_t)(retro_usec_t usec);
struct retro_frame_time_callback
{
   retro_frame_time_callback_t callback;
   /* Represents the time of one frame. It is computed as
    * 1000000 / fps, but the implementation will resolve the
    * rounding to ensure that framestepping, etc is exact. */
   retro_usec_t reference;
};
```

## Hardware Rendering

The hardware rendering system is unimplemented.

```
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
#define RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE (41 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* const struct retro_hw_render_interface ** --
                                            * Returns an API specific rendering interface for accessing API specific data.
                                            * Not all HW rendering APIs support or need this.
                                            * The contents of the returned pointer is specific to the rendering API
                                            * being used. See the various headers like libretro_vulkan.h, etc.
                                            *
                                            * GET_HW_RENDER_INTERFACE cannot be called before context_reset has been called.
                                            * Similarly, after context_destroyed callback returns,
                                            * the contents of the HW_RENDER_INTERFACE are invalidated.
                                            */
#define RETRO_ENVIRONMENT_SET_HW_RENDER_CONTEXT_NEGOTIATION_INTERFACE (43 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* const struct retro_hw_render_context_negotiation_interface * --
                                            * Sets an interface which lets the libretro core negotiate with frontend how a context is created.
                                            * The semantics of this interface depends on which API is used in SET_HW_RENDER earlier.
                                            * This interface will be used when the frontend is trying to create a HW rendering context,
                                            * so it will be used after SET_HW_RENDER, but before the context_reset callback.
                                            */

#define RETRO_ENVIRONMENT_SET_HW_SHARED_CONTEXT (44 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* N/A (null) * --
                                            * The frontend will try to use a 'shared' hardware context (mostly applicable
                                            * to OpenGL) when a hardware context is being set up.
                                            *
                                            * Returns true if the frontend supports shared hardware contexts and false
                                            * if the frontend does not support shared hardware contexts.
                                            *
                                            * This will do nothing on its own until SET_HW_RENDER env callbacks are
                                            * being used.
                                            */

enum retro_hw_render_interface_type
{
	RETRO_HW_RENDER_INTERFACE_VULKAN = 0,
	RETRO_HW_RENDER_INTERFACE_D3D9   = 1,
	RETRO_HW_RENDER_INTERFACE_D3D10  = 2,
	RETRO_HW_RENDER_INTERFACE_D3D11  = 3,
	RETRO_HW_RENDER_INTERFACE_D3D12  = 4,
   RETRO_HW_RENDER_INTERFACE_DUMMY  = INT_MAX
};

/* Base struct. All retro_hw_render_interface_* types
 * contain at least these fields. */
struct retro_hw_render_interface
{
   enum retro_hw_render_interface_type interface_type;
   unsigned interface_version;
};

enum retro_hw_render_context_negotiation_interface_type
{
   RETRO_HW_RENDER_CONTEXT_NEGOTIATION_INTERFACE_VULKAN = 0,
   RETRO_HW_RENDER_CONTEXT_NEGOTIATION_INTERFACE_DUMMY = INT_MAX
};

/* Base struct. All retro_hw_render_context_negotiation_interface_* types
 * contain at least these fields. */
struct retro_hw_render_context_negotiation_interface
{
   enum retro_hw_render_context_negotiation_interface_type interface_type;
   unsigned interface_version;
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
```

## Input Descriptors

Input descriptors are implemented by `RetroInputDescriptor` and
`retro_core_set_controller_descriptors()` but unused.

## Input Device Capabilities

Input device capabilities are implemented by
`retro_core_get_controller_capabilities()` but unused.

## Keyboard

The following symbols are defined but unused:

```
RETROK_MODE           = 313,
RETROK_COMPOSE        = 314,
RETROK_POWER          = 320,
```

## LED Interface

The LED interface is unimplemented.

```
#define RETRO_ENVIRONMENT_GET_LED_INTERFACE (46 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* struct retro_led_interface * --
                                            * Gets an interface which is used by a libretro core to set
                                            * state of LEDs.
                                            */

typedef void (RETRO_CALLCONV *retro_set_led_state_t)(int led, int state);
struct retro_led_interface
{
    retro_set_led_state_t set_led_state;
};
```

## Lightgun

The lightgun now uses absolute positions rather than relative ones, deprecating
some IDs and renaming some others by doing so.

```
/* LIGHTGUN device is similar to Guncon-2 for PlayStation 2.
 * It reports X/Y coordinates in screen space (similar to the pointer)
 * in the range [-0x8000, 0x7fff] in both axes, with zero being center.
 * As well as reporting on/off screen state. It features a trigger,
 * start/select buttons, auxiliary action buttons and a
 * directional pad. A forced off-screen shot can be requested for
 * auto-reloading function in some games.
 */
#define RETRO_DEVICE_LIGHTGUN     4

/* Id values for LIGHTGUN. */
#define RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X        13 /*Absolute Position*/
#define RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y        14 /*Absolute*/
#define RETRO_DEVICE_ID_LIGHTGUN_IS_OFFSCREEN    15 /*Status Check*/
#define RETRO_DEVICE_ID_LIGHTGUN_TRIGGER          2
#define RETRO_DEVICE_ID_LIGHTGUN_RELOAD          16 /*Forced off-screen shot*/
#define RETRO_DEVICE_ID_LIGHTGUN_AUX_A            3
#define RETRO_DEVICE_ID_LIGHTGUN_AUX_B            4
#define RETRO_DEVICE_ID_LIGHTGUN_START            6
#define RETRO_DEVICE_ID_LIGHTGUN_SELECT           7
#define RETRO_DEVICE_ID_LIGHTGUN_AUX_C            8
#define RETRO_DEVICE_ID_LIGHTGUN_DPAD_UP          9
#define RETRO_DEVICE_ID_LIGHTGUN_DPAD_DOWN       10
#define RETRO_DEVICE_ID_LIGHTGUN_DPAD_LEFT       11
#define RETRO_DEVICE_ID_LIGHTGUN_DPAD_RIGHT      12
/* deprecated */
#define RETRO_DEVICE_ID_LIGHTGUN_X                0 /*Relative Position*/
#define RETRO_DEVICE_ID_LIGHTGUN_Y                1 /*Relative*/
#define RETRO_DEVICE_ID_LIGHTGUN_CURSOR           3 /*Use Aux:A*/
#define RETRO_DEVICE_ID_LIGHTGUN_TURBO            4 /*Use Aux:B*/
#define RETRO_DEVICE_ID_LIGHTGUN_PAUSE            5 /*Use Start*/
```

## Location

The location system is unimplemented.

```
#define RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE 29
                                           /* struct retro_location_callback * --
                                            * Gets access to the location interface.
                                            * The purpose of this interface is to be able to retrieve
                                            * location-based information from the host device,
                                            * such as current latitude / longitude.
                                            */

/* Sets the interval of time and/or distance at which to update/poll
 * location-based data.
 *
 * To ensure compatibility with all location-based implementations,
 * values for both interval_ms and interval_distance should be provided.
 *
 * interval_ms is the interval expressed in milliseconds.
 * interval_distance is the distance interval expressed in meters.
 */
typedef void (RETRO_CALLCONV *retro_location_set_interval_t)(unsigned interval_ms,
      unsigned interval_distance);

/* Start location services. The device will start listening for changes to the
 * current location at regular intervals (which are defined with
 * retro_location_set_interval_t). */
typedef bool (RETRO_CALLCONV *retro_location_start_t)(void);

/* Stop location services. The device will stop listening for changes
 * to the current location. */
typedef void (RETRO_CALLCONV *retro_location_stop_t)(void);

/* Get the position of the current location. Will set parameters to
 * 0 if no new  location update has happened since the last time. */
typedef bool (RETRO_CALLCONV *retro_location_get_position_t)(double *lat, double *lon,
      double *horiz_accuracy, double *vert_accuracy);

/* Callback which signals when the location driver is initialized
 * and/or deinitialized.
 * retro_location_start_t can be called in initialized callback.
 */
typedef void (RETRO_CALLCONV *retro_location_lifetime_status_t)(void);

struct retro_location_callback
{
   retro_location_start_t         start;
   retro_location_stop_t          stop;
   retro_location_get_position_t  get_position;
   retro_location_set_interval_t  set_interval;

   retro_location_lifetime_status_t initialized;
   retro_location_lifetime_status_t deinitialized;
};
```

## Memory

The following symbols are undefined:

```
#define RETRO_MEMORY_MASK        0xff
```

The following symbols are defined but unused:

```
#define RETRO_MEMORY_RTC         1
#define RETRO_MEMORY_SYSTEM_RAM  2
#define RETRO_MEMORY_VIDEO_RAM   3
```

## Memory Maps

The memory maps system is unimplemented.

```
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

#define RETRO_MEMDESC_CONST     (1 << 0)   /* The frontend will never change this memory area once retro_load_game has returned. */
#define RETRO_MEMDESC_BIGENDIAN (1 << 1)   /* The memory area contains big endian data. Default is little endian. */
#define RETRO_MEMDESC_ALIGN_2   (1 << 16)  /* All memory access in this area is aligned to their own size, or 2, whichever is smaller. */
#define RETRO_MEMDESC_ALIGN_4   (2 << 16)
#define RETRO_MEMDESC_ALIGN_8   (3 << 16)
#define RETRO_MEMDESC_MINSIZE_2 (1 << 24)  /* All memory in this region is accessed at least 2 bytes at the time. */
#define RETRO_MEMDESC_MINSIZE_4 (2 << 24)
#define RETRO_MEMDESC_MINSIZE_8 (3 << 24)
struct retro_memory_descriptor
{
   uint64_t flags;

   /* Pointer to the start of the relevant ROM or RAM chip.
    * It's strongly recommended to use 'offset' if possible, rather than
    * doing math on the pointer.
    *
    * If the same byte is mapped my multiple descriptors, their descriptors
    * must have the same pointer.
    * If 'start' does not point to the first byte in the pointer, put the
    * difference in 'offset' instead.
    *
    * May be NULL if there's nothing usable here (e.g. hardware registers and
    * open bus). No flags should be set if the pointer is NULL.
    * It's recommended to minimize the number of descriptors if possible,
    * but not mandatory. */
   void *ptr;
   size_t offset;

   /* This is the location in the emulated address space
    * where the mapping starts. */
   size_t start;

   /* Which bits must be same as in 'start' for this mapping to apply.
    * The first memory descriptor to claim a certain byte is the one
    * that applies.
    * A bit which is set in 'start' must also be set in this.
    * Can be zero, in which case each byte is assumed mapped exactly once.
    * In this case, 'len' must be a power of two. */
   size_t select;

   /* If this is nonzero, the set bits are assumed not connected to the
    * memory chip's address pins. */
   size_t disconnect;

   /* This one tells the size of the current memory area.
    * If, after start+disconnect are applied, the address is higher than
    * this, the highest bit of the address is cleared.
    *
    * If the address is still too high, the next highest bit is cleared.
    * Can be zero, in which case it's assumed to be infinite (as limited
    * by 'select' and 'disconnect'). */
   size_t len;

   /* To go from emulated address to physical address, the following
    * order applies:
    * Subtract 'start', pick off 'disconnect', apply 'len', add 'offset'. */

   /* The address space name must consist of only a-zA-Z0-9_-,
    * should be as short as feasible (maximum length is 8 plus the NUL),
    * and may not be any other address space plus one or more 0-9A-F
    * at the end.
    * However, multiple memory descriptors for the same address space is
    * allowed, and the address space name can be empty. NULL is treated
    * as empty.
    *
    * Address space names are case sensitive, but avoid lowercase if possible.
    * The same pointer may exist in multiple address spaces.
    *
    * Examples:
    * blank+blank - valid (multiple things may be mapped in the same namespace)
    * 'Sp'+'Sp' - valid (multiple things may be mapped in the same namespace)
    * 'A'+'B' - valid (neither is a prefix of each other)
    * 'S'+blank - valid ('S' is not in 0-9A-F)
    * 'a'+blank - valid ('a' is not in 0-9A-F)
    * 'a'+'A' - valid (neither is a prefix of each other)
    * 'AR'+blank - valid ('R' is not in 0-9A-F)
    * 'ARB'+blank - valid (the B can't be part of the address either, because
    *                      there is no namespace 'AR')
    * blank+'B' - not valid, because it's ambigous which address space B1234
    *             would refer to.
    * The length can't be used for that purpose; the frontend may want
    * to append arbitrary data to an address, without a separator. */
   const char *addrspace;

   /* TODO: When finalizing this one, add a description field, which should be
    * "WRAM" or something roughly equally long. */

   /* TODO: When finalizing this one, replace 'select' with 'limit', which tells
    * which bits can vary and still refer to the same address (limit = ~select).
    * TODO: limit? range? vary? something else? */

   /* TODO: When finalizing this one, if 'len' is above what 'select' (or
    * 'limit') allows, it's bankswitched. Bankswitched data must have both 'len'
    * and 'select' != 0, and the mappings don't tell how the system switches the
    * banks. */

   /* TODO: When finalizing this one, fix the 'len' bit removal order.
    * For len=0x1800, pointer 0x1C00 should go to 0x1400, not 0x0C00.
    * Algorithm: Take bits highest to lowest, but if it goes above len, clear
    * the most recent addition and continue on the next bit.
    * TODO: Can the above be optimized? Is "remove the lowest bit set in both
    * pointer and 'len'" equivalent? */

   /* TODO: Some emulators (MAME?) emulate big endian systems by only accessing
    * the emulated memory in 32-bit chunks, native endian. But that's nothing
    * compared to Darek Mihocka <http://www.emulators.com/docs/nx07_vm101.htm>
    * (section Emulation 103 - Nearly Free Byte Reversal) - he flips the ENTIRE
    * RAM backwards! I'll want to represent both of those, via some flags.
    *
    * I suspect MAME either didn't think of that idea, or don't want the #ifdef.
    * Not sure which, nor do I really care. */

   /* TODO: Some of those flags are unused and/or don't really make sense. Clean
    * them up. */
};

/* The frontend may use the largest value of 'start'+'select' in a
 * certain namespace to infer the size of the address space.
 *
 * If the address space is larger than that, a mapping with .ptr=NULL
 * should be at the end of the array, with .select set to all ones for
 * as long as the address space is big.
 *
 * Sample descriptors (minus .ptr, and RETRO_MEMFLAG_ on the flags):
 * SNES WRAM:
 * .start=0x7E0000, .len=0x20000
 * (Note that this must be mapped before the ROM in most cases; some of the
 * ROM mappers
 * try to claim $7E0000, or at least $7E8000.)
 * SNES SPC700 RAM:
 * .addrspace="S", .len=0x10000
 * SNES WRAM mirrors:
 * .flags=MIRROR, .start=0x000000, .select=0xC0E000, .len=0x2000
 * .flags=MIRROR, .start=0x800000, .select=0xC0E000, .len=0x2000
 * SNES WRAM mirrors, alternate equivalent descriptor:
 * .flags=MIRROR, .select=0x40E000, .disconnect=~0x1FFF
 * (Various similar constructions can be created by combining parts of
 * the above two.)
 * SNES LoROM (512KB, mirrored a couple of times):
 * .flags=CONST, .start=0x008000, .select=0x408000, .disconnect=0x8000, .len=512*1024
 * .flags=CONST, .start=0x400000, .select=0x400000, .disconnect=0x8000, .len=512*1024
 * SNES HiROM (4MB):
 * .flags=CONST,                 .start=0x400000, .select=0x400000, .len=4*1024*1024
 * .flags=CONST, .offset=0x8000, .start=0x008000, .select=0x408000, .len=4*1024*1024
 * SNES ExHiROM (8MB):
 * .flags=CONST, .offset=0,                  .start=0xC00000, .select=0xC00000, .len=4*1024*1024
 * .flags=CONST, .offset=4*1024*1024,        .start=0x400000, .select=0xC00000, .len=4*1024*1024
 * .flags=CONST, .offset=0x8000,             .start=0x808000, .select=0xC08000, .len=4*1024*1024
 * .flags=CONST, .offset=4*1024*1024+0x8000, .start=0x008000, .select=0xC08000, .len=4*1024*1024
 * Clarify the size of the address space:
 * .ptr=NULL, .select=0xFFFFFF
 * .len can be implied by .select in many of them, but was included for clarity.
 */

struct retro_memory_map
{
   const struct retro_memory_descriptor *descriptors;
   unsigned num_descriptors;
};
```

## Mouse

The following mouse buttons are unimplemented:

```
#define RETRO_DEVICE_ID_MOUSE_BUTTON_4         9
#define RETRO_DEVICE_ID_MOUSE_BUTTON_5         10
```

## Overscan

Overscan is implemented by `RetroCore.overscan` but is never set and can't be
set by the users, hence it is implemented but unused

## Performances

The performances system is unimplemented.

```
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

#define RETRO_ENVIRONMENT_GET_PERF_INTERFACE 28
                                           /* struct retro_perf_callback * --
                                            * Gets an interface for performance counters. This is useful
                                            * for performance logging in a cross-platform way and for detecting
                                            * architecture-specific features, such as SIMD support.
                                            */

/* ID values for SIMD CPU features */
#define RETRO_SIMD_SSE      (1 << 0)
#define RETRO_SIMD_SSE2     (1 << 1)
#define RETRO_SIMD_VMX      (1 << 2)
#define RETRO_SIMD_VMX128   (1 << 3)
#define RETRO_SIMD_AVX      (1 << 4)
#define RETRO_SIMD_NEON     (1 << 5)
#define RETRO_SIMD_SSE3     (1 << 6)
#define RETRO_SIMD_SSSE3    (1 << 7)
#define RETRO_SIMD_MMX      (1 << 8)
#define RETRO_SIMD_MMXEXT   (1 << 9)
#define RETRO_SIMD_SSE4     (1 << 10)
#define RETRO_SIMD_SSE42    (1 << 11)
#define RETRO_SIMD_AVX2     (1 << 12)
#define RETRO_SIMD_VFPU     (1 << 13)
#define RETRO_SIMD_PS       (1 << 14)
#define RETRO_SIMD_AES      (1 << 15)
#define RETRO_SIMD_VFPV3    (1 << 16)
#define RETRO_SIMD_VFPV4    (1 << 17)
#define RETRO_SIMD_POPCNT   (1 << 18)
#define RETRO_SIMD_MOVBE    (1 << 19)
#define RETRO_SIMD_CMOV     (1 << 20)
#define RETRO_SIMD_ASIMD    (1 << 21)

typedef uint64_t retro_perf_tick_t;
typedef int64_t retro_time_t;

struct retro_perf_counter
{
   const char *ident;
   retro_perf_tick_t start;
   retro_perf_tick_t total;
   retro_perf_tick_t call_cnt;

   bool registered;
};

/* Returns current time in microseconds.
 * Tries to use the most accurate timer available.
 */
typedef retro_time_t (RETRO_CALLCONV *retro_perf_get_time_usec_t)(void);

/* A simple counter. Usually nanoseconds, but can also be CPU cycles.
 * Can be used directly if desired (when creating a more sophisticated
 * performance counter system).
 * */
typedef retro_perf_tick_t (RETRO_CALLCONV *retro_perf_get_counter_t)(void);

/* Returns a bit-mask of detected CPU features (RETRO_SIMD_*). */
typedef uint64_t (RETRO_CALLCONV *retro_get_cpu_features_t)(void);

/* Asks frontend to log and/or display the state of performance counters.
 * Performance counters can always be poked into manually as well.
 */
typedef void (RETRO_CALLCONV *retro_perf_log_t)(void);

/* Register a performance counter.
 * ident field must be set with a discrete value and other values in
 * retro_perf_counter must be 0.
 * Registering can be called multiple times. To avoid calling to
 * frontend redundantly, you can check registered field first. */
typedef void (RETRO_CALLCONV *retro_perf_register_t)(struct retro_perf_counter *counter);

/* Starts a registered counter. */
typedef void (RETRO_CALLCONV *retro_perf_start_t)(struct retro_perf_counter *counter);

/* Stops a registered counter. */
typedef void (RETRO_CALLCONV *retro_perf_stop_t)(struct retro_perf_counter *counter);

/* For convenience it can be useful to wrap register, start and stop in macros.
 * E.g.:
 * #ifdef LOG_PERFORMANCE
 * #define RETRO_PERFORMANCE_INIT(perf_cb, name) static struct retro_perf_counter name = {#name}; if (!name.registered) perf_cb.perf_register(&(name))
 * #define RETRO_PERFORMANCE_START(perf_cb, name) perf_cb.perf_start(&(name))
 * #define RETRO_PERFORMANCE_STOP(perf_cb, name) perf_cb.perf_stop(&(name))
 * #else
 * ... Blank macros ...
 * #endif
 *
 * These can then be used mid-functions around code snippets.
 *
 * extern struct retro_perf_callback perf_cb;  * Somewhere in the core.
 *
 * void do_some_heavy_work(void)
 * {
 *    RETRO_PERFORMANCE_INIT(cb, work_1;
 *    RETRO_PERFORMANCE_START(cb, work_1);
 *    heavy_work_1();
 *    RETRO_PERFORMANCE_STOP(cb, work_1);
 *
 *    RETRO_PERFORMANCE_INIT(cb, work_2);
 *    RETRO_PERFORMANCE_START(cb, work_2);
 *    heavy_work_2();
 *    RETRO_PERFORMANCE_STOP(cb, work_2);
 * }
 *
 * void retro_deinit(void)
 * {
 *    perf_cb.perf_log();  * Log all perf counters here for example.
 * }
 */

struct retro_perf_callback
{
   retro_perf_get_time_usec_t    get_time_usec;
   retro_get_cpu_features_t      get_cpu_features;

   retro_perf_get_counter_t      get_perf_counter;
   retro_perf_register_t         perf_register;
   retro_perf_start_t            perf_start;
   retro_perf_stop_t             perf_stop;
   retro_perf_log_t              perf_log;
};
```

## Proc Address

The proc address system is unimplemented.

```
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

typedef void (RETRO_CALLCONV *retro_proc_address_t)(void);

/* libretro API extension functions:
 * (None here so far).
 *
 * Get a symbol from a libretro core.
 * Cores should only return symbols which are actual
 * extensions to the libretro API.
 *
 * Frontends should not use this to obtain symbols to standard
 * libretro entry points (static linking or dlsym).
 *
 * The symbol name must be equal to the function name,
 * e.g. if void retro_foo(void); exists, the symbol must be called "retro_foo".
 * The returned function pointer must be cast to the corresponding type.
 */
typedef retro_proc_address_t (RETRO_CALLCONV *retro_get_proc_address_t)(const char *sym);

struct retro_get_proc_address_interface
{
   retro_get_proc_address_t get_proc_address;
};
```

## Region

The region system is unimplemented.

```
/* Returned from retro_get_region(). */
#define RETRO_REGION_NTSC  0
#define RETRO_REGION_PAL   1

/* Gets region of game. */
RETRO_API unsigned retro_get_region(void);
```

## Rotation

Rotation is implemented but not used by the displays.

## Sensor

The sensor system is unimplemented.

```
#define RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE (25 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* struct retro_sensor_interface * --
                                            * Gets access to the sensor interface.
                                            * The purpose of this interface is to allow
                                            * setting state related to sensors such as polling rate,
                                            * enabling/disable it entirely, etc.
                                            * Reading sensor state is done via the normal
                                            * input_state_callback API.
                                            */

/* FIXME: Document the sensor API and work out behavior.
 * It will be marked as experimental until then.
 */
enum retro_sensor_action
{
   RETRO_SENSOR_ACCELEROMETER_ENABLE = 0,
   RETRO_SENSOR_ACCELEROMETER_DISABLE,

   RETRO_SENSOR_DUMMY = INT_MAX
};

/* Id values for SENSOR types. */
#define RETRO_SENSOR_ACCELEROMETER_X 0
#define RETRO_SENSOR_ACCELEROMETER_Y 1
#define RETRO_SENSOR_ACCELEROMETER_Z 2

typedef bool (RETRO_CALLCONV *retro_set_sensor_state_t)(unsigned port,
      enum retro_sensor_action action, unsigned rate);

typedef float (RETRO_CALLCONV *retro_sensor_get_input_t)(unsigned port, unsigned id);

struct retro_sensor_interface
{
   retro_set_sensor_state_t set_sensor_state;
   retro_sensor_get_input_t get_sensor_input;
};
```

## Serialization Quirks

The serialization quirks system is unimplemented.

```
/* Serialized state is incomplete in some way. Set if serialization is
 * usable in typical end-user cases but should not be relied upon to
 * implement frame-sensitive frontend features such as netplay or
 * rerecording. */
#define RETRO_SERIALIZATION_QUIRK_INCOMPLETE (1 << 0)
/* The core must spend some time initializing before serialization is
 * supported. retro_serialize() will initially fail; retro_unserialize()
 * and retro_serialize_size() may or may not work correctly either. */
#define RETRO_SERIALIZATION_QUIRK_MUST_INITIALIZE (1 << 1)
/* Serialization size may change within a session. */
#define RETRO_SERIALIZATION_QUIRK_CORE_VARIABLE_SIZE (1 << 2)
/* Set by the frontend to acknowledge that it supports variable-sized
 * states. */
#define RETRO_SERIALIZATION_QUIRK_FRONT_VARIABLE_SIZE (1 << 3)
/* Serialized state can only be loaded during the same session. */
#define RETRO_SERIALIZATION_QUIRK_SINGLE_SESSION (1 << 4)
/* Serialized state cannot be loaded on an architecture with a different
 * endianness from the one it was saved on. */
#define RETRO_SERIALIZATION_QUIRK_ENDIAN_DEPENDENT (1 << 5)
/* Serialized state cannot be loaded on a different platform from the one it
 * was saved on for reasons other than endianness, such as word size
 * dependence */
#define RETRO_SERIALIZATION_QUIRK_PLATFORM_DEPENDENT (1 << 6)

#define RETRO_ENVIRONMENT_SET_SERIALIZATION_QUIRKS 44
                                           /* uint64_t * --
                                            * Sets quirk flags associated with serialization. The frontend will zero any flags it doesn't
                                            * recognize or support. Should be set in either retro_init or retro_load_game, but not both.
                                            */
```

## Subsystem

The subsystem system is unimplemented.

```
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

struct retro_subsystem_memory_info
{
   /* The extension associated with a memory type, e.g. "psram". */
   const char *extension;

   /* The memory type for retro_get_memory(). This should be at
    * least 0x100 to avoid conflict with standardized
    * libretro memory types. */
   unsigned type;
};

struct retro_subsystem_rom_info
{
   /* Describes what the content is (SGB BIOS, GB ROM, etc). */
   const char *desc;

   /* Same definition as retro_get_system_info(). */
   const char *valid_extensions;

   /* Same definition as retro_get_system_info(). */
   bool need_fullpath;

   /* Same definition as retro_get_system_info(). */
   bool block_extract;

   /* This is set if the content is required to load a game.
    * If this is set to false, a zeroed-out retro_game_info can be passed. */
   bool required;

   /* Content can have multiple associated persistent
    * memory types (retro_get_memory()). */
   const struct retro_subsystem_memory_info *memory;
   unsigned num_memory;
};

struct retro_subsystem_info
{
   /* Human-readable string of the subsystem type, e.g. "Super GameBoy" */
   const char *desc;

   /* A computer friendly short string identifier for the subsystem type.
    * This name must be [a-z].
    * E.g. if desc is "Super GameBoy", this can be "sgb".
    * This identifier can be used for command-line interfaces, etc.
    */
   const char *ident;

   /* Infos for each content file. The first entry is assumed to be the
    * "most significant" content for frontend purposes.
    * E.g. with Super GameBoy, the first content should be the GameBoy ROM,
    * as it is the most "significant" content to a user.
    * If a frontend creates new file paths based on the content used
    * (e.g. savestates), it should use the path for the first ROM to do so. */
   const struct retro_subsystem_rom_info *roms;

   /* Number of content files associated with a subsystem. */
   unsigned num_roms;

   /* The type passed to retro_load_game_special(). */
   unsigned id;
};

/* Loads a "special" kind of game. Should not be used,
 * except in extreme cases. */
RETRO_API bool retro_load_game_special(
  unsigned game_type,
  const struct retro_game_info *info, size_t num_info
);
```

## User Name

The user name accessor is unimplemented.

```
#define RETRO_ENVIRONMENT_GET_USERNAME 38
                                           /* const char **
                                            * Returns the specified username of the frontend, if specified by the user.
                                            * This username can be used as a nickname for a core that has online facilities
                                            * or any other mode where personalization of the user is desirable.
                                            * The returned value can be NULL.
                                            * If this environ callback is used by a core that requires a valid username,
                                            * a default username should be specified by the core.
                                            */
```

## Variables

The varibles system is implemented but unused.

## Virtual File System

The virtual file system is unimplemented.

```
#define RETRO_ENVIRONMENT_GET_VFS_INTERFACE (45 | RETRO_ENVIRONMENT_EXPERIMENTAL)
                                           /* struct retro_vfs_interface_info * --
                                            * Gets access to the VFS interface.
                                            * VFS presence needs to be queried prior to load_game or any
                                            * get_system/save/other_directory being called to let front end know
                                            * core supports VFS before it starts handing out paths.
                                            * It is recomended to do so in retro_set_environment */

/* VFS functionality */

/* File paths:
 * File paths passed as parameters when using this api shall be well formed unix-style,
 * using "/" (unquoted forward slash) as directory separator regardless of the platform's native separator.
 * Paths shall also include at least one forward slash ("game.bin" is an invalid path, use "./game.bin" instead).
 * Other than the directory separator, cores shall not make assumptions about path format:
 * "C:/path/game.bin", "http://example.com/game.bin", "#game/game.bin", "./game.bin" (without quotes) are all valid paths.
 * Cores may replace the basename or remove path components from the end, and/or add new components;
 * however, cores shall not append "./", "../" or multiple consecutive forward slashes ("//") to paths they request to front end.
 * The frontend is encouraged to make such paths work as well as it can, but is allowed to give up if the core alters paths too much.
 * Frontends are encouraged, but not required, to support native file system paths (modulo replacing the directory separator, if applicable).
 * Cores are allowed to try using them, but must remain functional if the front rejects such requests.
 * Cores are encouraged to use the libretro-common filestream functions for file I/O,
 * as they seamlessly integrate with VFS, deal with directory separator replacement as appropriate
 * and provide platform-specific fallbacks in cases where front ends do not support VFS. */

/* Opaque file handle
 * Introduced in VFS API v1 */
struct retro_vfs_file_handle;

/* File open flags
 * Introduced in VFS API v1 */
#define RETRO_VFS_FILE_ACCESS_READ            (1 << 0) /* Read only mode */
#define RETRO_VFS_FILE_ACCESS_WRITE           (1 << 1) /* Write only mode, discard contents and overwrites existing file unless RETRO_VFS_FILE_ACCESS_UPDATE is also specified */
#define RETRO_VFS_FILE_ACCESS_READ_WRITE      (RETRO_VFS_FILE_ACCESS_READ | RETRO_VFS_FILE_ACCESS_WRITE) /* Read-write mode, discard contents and overwrites existing file unless RETRO_VFS_FILE_ACCESS_UPDATE is also specified*/
#define RETRO_VFS_FILE_ACCESS_UPDATE_EXISTING (1 << 2) /* Prevents discarding content of existing files opened for writing */

/* These are only hints. The frontend may choose to ignore them. Other than RAM/CPU/etc use,
   and how they react to unlikely external interference (for example someone else writing to that file,
   or the file's server going down), behavior will not change. */
#define RETRO_VFS_FILE_ACCESS_HINT_NONE              (0)
/* Indicate that the file will be accessed many times. The frontend should aggressively cache everything. */
#define RETRO_VFS_FILE_ACCESS_HINT_FREQUENT_ACCESS   (1 << 0)

/* Seek positions */
#define RETRO_VFS_SEEK_POSITION_START    0
#define RETRO_VFS_SEEK_POSITION_CURRENT  1
#define RETRO_VFS_SEEK_POSITION_END      2

/* Get path from opaque handle. Returns the exact same path passed to file_open when getting the handle
 * Introduced in VFS API v1 */
typedef const char *(RETRO_CALLCONV *retro_vfs_get_path_t)(struct retro_vfs_file_handle *stream);

/* Open a file for reading or writing. If path points to a directory, this will
 * fail. Returns the opaque file handle, or NULL for error.
 * Introduced in VFS API v1 */
typedef struct retro_vfs_file_handle *(RETRO_CALLCONV *retro_vfs_open_t)(const char *path, unsigned mode, unsigned hints);

/* Close the file and release its resources. Must be called if open_file returns non-NULL. Returns 0 on succes, -1 on failure.
 * Whether the call succeeds ot not, the handle passed as parameter becomes invalid and should no longer be used.
 * Introduced in VFS API v1 */
typedef int (RETRO_CALLCONV *retro_vfs_close_t)(struct retro_vfs_file_handle *stream);

/* Return the size of the file in bytes, or -1 for error.
 * Introduced in VFS API v1 */
typedef int64_t (RETRO_CALLCONV *retro_vfs_size_t)(struct retro_vfs_file_handle *stream);

/* Get the current read / write position for the file. Returns - 1 for error.
 * Introduced in VFS API v1 */
typedef int64_t (RETRO_CALLCONV *retro_vfs_tell_t)(struct retro_vfs_file_handle *stream);

/* Set the current read/write position for the file. Returns the new position, -1 for error.
 * Introduced in VFS API v1 */
typedef int64_t (RETRO_CALLCONV *retro_vfs_seek_t)(struct retro_vfs_file_handle *stream, int64_t offset, int seek_position);

/* Read data from a file. Returns the number of bytes read, or -1 for error.
 * Introduced in VFS API v1 */
typedef int64_t (RETRO_CALLCONV *retro_vfs_read_t)(struct retro_vfs_file_handle *stream, void *s, uint64_t len);

/* Write data to a file. Returns the number of bytes written, or -1 for error.
 * Introduced in VFS API v1 */
typedef int64_t (RETRO_CALLCONV *retro_vfs_write_t)(struct retro_vfs_file_handle *stream, const void *s, uint64_t len);

/* Flush pending writes to file, if using buffered IO. Returns 0 on sucess, or -1 on failure.
 * Introduced in VFS API v1 */
typedef int (RETRO_CALLCONV *retro_vfs_flush_t)(struct retro_vfs_file_handle *stream);

/* Delete the specified file. Returns 0 on success, -1 on failure
 * Introduced in VFS API v1 */
typedef int (RETRO_CALLCONV *retro_vfs_remove_t)(const char *path);

/* Rename the specified file. Returns 0 on success, -1 on failure
 * Introduced in VFS API v1 */
typedef int (RETRO_CALLCONV *retro_vfs_rename_t)(const char *old_path, const char *new_path);

struct retro_vfs_interface
{
	retro_vfs_get_path_t get_path;
	retro_vfs_open_t open;
	retro_vfs_close_t close;
	retro_vfs_size_t size;
	retro_vfs_tell_t tell;
	retro_vfs_seek_t seek;
	retro_vfs_read_t read;
	retro_vfs_write_t write;
	retro_vfs_flush_t flush;
	retro_vfs_remove_t remove;
	retro_vfs_rename_t rename;
};

struct retro_vfs_interface_info
{
   /* Set by core: should this be higher than the version the front end supports,
    * front end will return false in the RETRO_ENVIRONMENT_GET_VFS_INTERFACE call
    * Introduced in VFS API v1 */
   uint32_t required_interface_version;

   /* Frontend writes interface pointer here. The frontend also sets the actual
    * version, must be at least required_interface_version.
    * Introduced in VFS API v1 */
   struct retro_vfs_interface *iface;
};
```
