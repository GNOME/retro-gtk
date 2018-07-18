// This file is part of retro-gtk. License: GPL-3.0+.

#include "retro-core-private.h"

#include <epoxy/gl.h>
#include <stdbool.h>
#include "retro-input-private.h"
#include "retro-pixdata-private.h"

#define RETRO_ENVIRONMENT_EXPERIMENTAL 0x10000
#define RETRO_ENVIRONMENT_PRIVATE 0x20000
#define RETRO_ENVIRONMENT_SET_ROTATION 1
#define RETRO_ENVIRONMENT_GET_OVERSCAN 2
#define RETRO_ENVIRONMENT_GET_CAN_DUPE 3
#define RETRO_ENVIRONMENT_SET_MESSAGE 6
#define RETRO_ENVIRONMENT_SHUTDOWN 7
#define RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL 8
#define RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY 9
#define RETRO_ENVIRONMENT_SET_PIXEL_FORMAT 10
#define RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS 11
#define RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK 12
#define RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE 13
#define RETRO_ENVIRONMENT_SET_HW_RENDER 14
#define RETRO_ENVIRONMENT_GET_VARIABLE 15
#define RETRO_ENVIRONMENT_SET_VARIABLES 16
#define RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE 17
#define RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME 18
#define RETRO_ENVIRONMENT_GET_LIBRETRO_PATH 19
#define RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK 22
#define RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK 21
#define RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE 23
#define RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES 24
#define RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE (25 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_GET_CAMERA_INTERFACE (26 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_GET_LOG_INTERFACE 27
#define RETRO_ENVIRONMENT_GET_PERF_INTERFACE 28
#define RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE 29
#define RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY 30 /* Old name, kept for compatibility. */
#define RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY 30
#define RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY 31
#define RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO 32
#define RETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK 33
#define RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO 34
#define RETRO_ENVIRONMENT_SET_CONTROLLER_INFO 35
#define RETRO_ENVIRONMENT_SET_MEMORY_MAPS (36 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_SET_GEOMETRY 37
#define RETRO_ENVIRONMENT_GET_USERNAME 38
#define RETRO_ENVIRONMENT_GET_LANGUAGE 39
#define RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER (40 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE (41 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS (42 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_SET_HW_RENDER_CONTEXT_NEGOTIATION_INTERFACE (43 | RETRO_ENVIRONMENT_EXPERIMENTAL)
#define RETRO_ENVIRONMENT_SET_SERIALIZATION_QUIRKS 44

enum RetroLanguage {
  RETRO_LANGUAGE_ENGLISH = 0,
  RETRO_LANGUAGE_JAPANESE,
  RETRO_LANGUAGE_FRENCH,
  RETRO_LANGUAGE_SPANISH,
  RETRO_LANGUAGE_GERMAN,
  RETRO_LANGUAGE_ITALIAN,
  RETRO_LANGUAGE_DUTCH,
  RETRO_LANGUAGE_PORTUGUESE_BRAZIL,
  RETRO_LANGUAGE_PORTUGUESE_PORTUGAL,
  RETRO_LANGUAGE_RUSSIAN,
  RETRO_LANGUAGE_KOREAN,
  RETRO_LANGUAGE_CHINESE_TRADITIONAL,
  RETRO_LANGUAGE_CHINESE_SIMPLIFIED,
  RETRO_LANGUAGE_ESPERANTO,
  RETRO_LANGUAGE_POLISH,
  RETRO_LANGUAGE_VIETNAMESE,
  RETRO_LANGUAGE_ARABIC,
  RETRO_LANGUAGE_DEFAULT = RETRO_LANGUAGE_ENGLISH,
};

enum RetroLogLevel {
  RETRO_LOG_LEVEL_DEBUG = 0,
  RETRO_LOG_LEVEL_INFO,
  RETRO_LOG_LEVEL_WARN,
  RETRO_LOG_LEVEL_ERROR,
};

typedef struct {
  gpointer log;
} RetroLogCallback;

typedef struct {
  const gchar *msg;
  guint frames;
} RetroMessage;

typedef struct {
  gpointer set_rumble_state;
} RetroRumbleCallback;

/* Pass this to retro_video_refresh_t if rendering to hardware.
 * Passing NULL to retro_video_refresh_t is still a frame dupe as normal.
 */
#define RETRO_HW_FRAME_BUFFER_VALID ((void*)-1)

static gboolean
rumble_callback_set_rumble_state (guint             port,
                                  RetroRumbleEffect effect,
                                  guint16           strength)
{
  RetroCore *self;
  RetroController *controller;

  self = retro_core_get_cb_data ();

  g_return_val_if_fail (RETRO_IS_CORE (self), FALSE);

  if (!g_hash_table_contains (self->controllers, &port))
    return FALSE;

  controller = g_hash_table_lookup (self->controllers, &port);

  if (controller == NULL)
    return FALSE;

  return retro_controller_set_rumble_state (controller, effect, strength);
}

static void
on_log (guint level, const gchar *format, ...)
{
  RetroCore *self;
  const gchar *log_domain;
  GLogLevelFlags log_level;
  gchar *message;
  va_list args;

  self = retro_core_get_cb_data ();
  if (!self)
    g_return_if_reached ();

  switch (level) {
  case RETRO_LOG_LEVEL_DEBUG:
    log_level = G_LOG_LEVEL_DEBUG;

    break;
  case RETRO_LOG_LEVEL_INFO:
    log_level = G_LOG_LEVEL_MESSAGE;

    break;
  case RETRO_LOG_LEVEL_WARN:
    log_level = G_LOG_LEVEL_WARNING;

    break;
  case RETRO_LOG_LEVEL_ERROR:
    log_level = G_LOG_LEVEL_CRITICAL;

    break;
  default:
    g_debug ("Unexpected log level: %d", level);

    return;
  }

  // Get the arguments, set up the formatted message, pass it to the logging
  // method and free it.
  va_start (args, format);
  message = g_strdup_vprintf (format, args);

  log_domain = retro_core_get_name (self);
  g_signal_emit_by_name (self, "log", log_domain, log_level, message);

  g_free (message);
}

static uintptr_t
get_current_framebuffer ()
{
  RetroCore *self;
  GLuint framebuffer;
  GLuint texture;

  g_message ("get current framebuffer");

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_return_val_if_reached (0);

  /* TODO */
  /* Set by frontend.
   * TODO: This is rather obsolete. The frontend should not
   * be providing preallocated framebuffers. */

  glGenFramebuffers(1, &framebuffer);

  glGenTextures (1, &texture);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, texture, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);


  g_message ("framebuffer %u", framebuffer);
  /* glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); */

  return framebuffer;
}

static RetroProcAddress
get_proc_address (const gchar *symbol_name)
{

  GModule *module;
  gpointer symbol = NULL;

  module = g_module_open (NULL, G_MODULE_BIND_LAZY);
  g_module_symbol (module, symbol_name, &symbol);

  if (symbol == NULL)
    g_message ("Requested function %s not found.", symbol_name);

  g_module_close (module);

  if (symbol != NULL)
    return symbol;

  if (g_str_equal (symbol_name, "glAccumxOES")) return glAccumxOES;
  if (g_str_equal (symbol_name, "glActiveShaderProgram")) return glActiveShaderProgram;
  if (g_str_equal (symbol_name, "glActiveTextureARB")) return glActiveTextureARB;
  if (g_str_equal (symbol_name, "glActiveTexture")) return glActiveTexture;
  if (g_str_equal (symbol_name, "glAlphaFuncxOES")) return glAlphaFuncxOES;
  if (g_str_equal (symbol_name, "glAttachObjectARB")) return glAttachObjectARB;
  if (g_str_equal (symbol_name, "glAttachShader")) return glAttachShader;
  if (g_str_equal (symbol_name, "glBeginConditionalRender")) return glBeginConditionalRender;
  if (g_str_equal (symbol_name, "glBeginQueryARB")) return glBeginQueryARB;
  if (g_str_equal (symbol_name, "glBeginQueryIndexed")) return glBeginQueryIndexed;
  if (g_str_equal (symbol_name, "glBeginQuery")) return glBeginQuery;
  if (g_str_equal (symbol_name, "glBeginTransformFeedback")) return glBeginTransformFeedback;
  if (g_str_equal (symbol_name, "glBindAttribLocationARB")) return glBindAttribLocationARB;
  if (g_str_equal (symbol_name, "glBindAttribLocation")) return glBindAttribLocation;
  if (g_str_equal (symbol_name, "glBindBufferARB")) return glBindBufferARB;
  if (g_str_equal (symbol_name, "glBindBufferBase")) return glBindBufferBase;
  if (g_str_equal (symbol_name, "glBindBufferRange")) return glBindBufferRange;
  if (g_str_equal (symbol_name, "glBindBuffer")) return glBindBuffer;
  if (g_str_equal (symbol_name, "glBindBuffersBase")) return glBindBuffersBase;
  if (g_str_equal (symbol_name, "glBindBuffersRange")) return glBindBuffersRange;
  if (g_str_equal (symbol_name, "glBindFragDataLocationIndexed")) return glBindFragDataLocationIndexed;
  if (g_str_equal (symbol_name, "glBindFragDataLocation")) return glBindFragDataLocation;
  if (g_str_equal (symbol_name, "glBindFramebuffer")) return glBindFramebuffer;
  if (g_str_equal (symbol_name, "glBindImageTexture")) return glBindImageTexture;
  if (g_str_equal (symbol_name, "glBindImageTextures")) return glBindImageTextures;
  if (g_str_equal (symbol_name, "glBindProgramARB")) return glBindProgramARB;
  if (g_str_equal (symbol_name, "glBindProgramPipeline")) return glBindProgramPipeline;
  if (g_str_equal (symbol_name, "glBindRenderbuffer")) return glBindRenderbuffer;
  if (g_str_equal (symbol_name, "glBindSampler")) return glBindSampler;
  if (g_str_equal (symbol_name, "glBindSamplers")) return glBindSamplers;
  if (g_str_equal (symbol_name, "glBindTextures")) return glBindTextures;
  if (g_str_equal (symbol_name, "glBindTransformFeedback")) return glBindTransformFeedback;
  if (g_str_equal (symbol_name, "glBindVertexArray")) return glBindVertexArray;
  if (g_str_equal (symbol_name, "glBindVertexBuffer")) return glBindVertexBuffer;
  if (g_str_equal (symbol_name, "glBindVertexBuffers")) return glBindVertexBuffers;
  if (g_str_equal (symbol_name, "glBitmapxOES")) return glBitmapxOES;
  if (g_str_equal (symbol_name, "glBlendColor")) return glBlendColor;
  if (g_str_equal (symbol_name, "glBlendColorxOES")) return glBlendColorxOES;
  if (g_str_equal (symbol_name, "glBlendEquationiARB")) return glBlendEquationiARB;
  if (g_str_equal (symbol_name, "glBlendEquationi")) return glBlendEquationi;
  if (g_str_equal (symbol_name, "glBlendEquation")) return glBlendEquation;
  if (g_str_equal (symbol_name, "glBlendEquationSeparateiARB")) return glBlendEquationSeparateiARB;
  if (g_str_equal (symbol_name, "glBlendEquationSeparatei")) return glBlendEquationSeparatei;
  if (g_str_equal (symbol_name, "glBlendEquationSeparate")) return glBlendEquationSeparate;
  if (g_str_equal (symbol_name, "glBlendFunciARB")) return glBlendFunciARB;
  if (g_str_equal (symbol_name, "glBlendFunci")) return glBlendFunci;
  if (g_str_equal (symbol_name, "glBlendFuncSeparateiARB")) return glBlendFuncSeparateiARB;
  if (g_str_equal (symbol_name, "glBlendFuncSeparatei")) return glBlendFuncSeparatei;
  if (g_str_equal (symbol_name, "glBlendFuncSeparate")) return glBlendFuncSeparate;
  if (g_str_equal (symbol_name, "glBlitFramebuffer")) return glBlitFramebuffer;
  if (g_str_equal (symbol_name, "glBufferDataARB")) return glBufferDataARB;
  if (g_str_equal (symbol_name, "glBufferData")) return glBufferData;
  if (g_str_equal (symbol_name, "glBufferStorage")) return glBufferStorage;
  if (g_str_equal (symbol_name, "glBufferSubDataARB")) return glBufferSubDataARB;
  if (g_str_equal (symbol_name, "glBufferSubData")) return glBufferSubData;
  if (g_str_equal (symbol_name, "glCheckFramebufferStatus")) return glCheckFramebufferStatus;
  if (g_str_equal (symbol_name, "glClampColorARB")) return glClampColorARB;
  if (g_str_equal (symbol_name, "glClampColor")) return glClampColor;
  if (g_str_equal (symbol_name, "glClearAccumxOES")) return glClearAccumxOES;
  if (g_str_equal (symbol_name, "glClearBufferData")) return glClearBufferData;
  if (g_str_equal (symbol_name, "glClearBufferfi")) return glClearBufferfi;
  if (g_str_equal (symbol_name, "glClearBufferfv")) return glClearBufferfv;
  if (g_str_equal (symbol_name, "glClearBufferiv")) return glClearBufferiv;
  if (g_str_equal (symbol_name, "glClearBufferSubData")) return glClearBufferSubData;
  if (g_str_equal (symbol_name, "glClearBufferuiv")) return glClearBufferuiv;
  if (g_str_equal (symbol_name, "glClearColorxOES")) return glClearColorxOES;
  if (g_str_equal (symbol_name, "glClearDepthfOES")) return glClearDepthfOES;
  if (g_str_equal (symbol_name, "glClearDepthf")) return glClearDepthf;
  if (g_str_equal (symbol_name, "glClearDepthxOES")) return glClearDepthxOES;
  if (g_str_equal (symbol_name, "glClearTexImage")) return glClearTexImage;
  if (g_str_equal (symbol_name, "glClearTexSubImage")) return glClearTexSubImage;
  if (g_str_equal (symbol_name, "glClientActiveTextureARB")) return glClientActiveTextureARB;
  if (g_str_equal (symbol_name, "glClientActiveTexture")) return glClientActiveTexture;
  if (g_str_equal (symbol_name, "glClientWaitSync")) return glClientWaitSync;
  if (g_str_equal (symbol_name, "glClipPlanefOES")) return glClipPlanefOES;
  if (g_str_equal (symbol_name, "glClipPlanexOES")) return glClipPlanexOES;
  if (g_str_equal (symbol_name, "glColor3xOES")) return glColor3xOES;
  if (g_str_equal (symbol_name, "glColor3xvOES")) return glColor3xvOES;
  if (g_str_equal (symbol_name, "glColor4xOES")) return glColor4xOES;
  if (g_str_equal (symbol_name, "glColor4xvOES")) return glColor4xvOES;
  if (g_str_equal (symbol_name, "glColorMaski")) return glColorMaski;
  if (g_str_equal (symbol_name, "glColorP3ui")) return glColorP3ui;
  if (g_str_equal (symbol_name, "glColorP3uiv")) return glColorP3uiv;
  if (g_str_equal (symbol_name, "glColorP4ui")) return glColorP4ui;
  if (g_str_equal (symbol_name, "glColorP4uiv")) return glColorP4uiv;
  if (g_str_equal (symbol_name, "glColorSubTable")) return glColorSubTable;
  if (g_str_equal (symbol_name, "glColorTableParameterfv")) return glColorTableParameterfv;
  if (g_str_equal (symbol_name, "glColorTableParameteriv")) return glColorTableParameteriv;
  if (g_str_equal (symbol_name, "glColorTable")) return glColorTable;
  if (g_str_equal (symbol_name, "glCompileShaderARB")) return glCompileShaderARB;
  if (g_str_equal (symbol_name, "glCompileShaderIncludeARB")) return glCompileShaderIncludeARB;
  if (g_str_equal (symbol_name, "glCompileShader")) return glCompileShader;
  if (g_str_equal (symbol_name, "glCompressedTexImage1DARB")) return glCompressedTexImage1DARB;
  if (g_str_equal (symbol_name, "glCompressedTexImage1D")) return glCompressedTexImage1D;
  if (g_str_equal (symbol_name, "glCompressedTexImage2DARB")) return glCompressedTexImage2DARB;
  if (g_str_equal (symbol_name, "glCompressedTexImage2D")) return glCompressedTexImage2D;
  if (g_str_equal (symbol_name, "glCompressedTexImage3DARB")) return glCompressedTexImage3DARB;
  if (g_str_equal (symbol_name, "glCompressedTexImage3D")) return glCompressedTexImage3D;
  if (g_str_equal (symbol_name, "glCompressedTexSubImage1DARB")) return glCompressedTexSubImage1DARB;
  if (g_str_equal (symbol_name, "glCompressedTexSubImage1D")) return glCompressedTexSubImage1D;
  if (g_str_equal (symbol_name, "glCompressedTexSubImage2DARB")) return glCompressedTexSubImage2DARB;
  if (g_str_equal (symbol_name, "glCompressedTexSubImage2D")) return glCompressedTexSubImage2D;
  if (g_str_equal (symbol_name, "glCompressedTexSubImage3DARB")) return glCompressedTexSubImage3DARB;
  if (g_str_equal (symbol_name, "glCompressedTexSubImage3D")) return glCompressedTexSubImage3D;
  if (g_str_equal (symbol_name, "glConvolutionFilter1D")) return glConvolutionFilter1D;
  if (g_str_equal (symbol_name, "glConvolutionFilter2D")) return glConvolutionFilter2D;
  if (g_str_equal (symbol_name, "glConvolutionParameterf")) return glConvolutionParameterf;
  if (g_str_equal (symbol_name, "glConvolutionParameterfv")) return glConvolutionParameterfv;
  if (g_str_equal (symbol_name, "glConvolutionParameteri")) return glConvolutionParameteri;
  if (g_str_equal (symbol_name, "glConvolutionParameteriv")) return glConvolutionParameteriv;
  if (g_str_equal (symbol_name, "glConvolutionParameterxOES")) return glConvolutionParameterxOES;
  if (g_str_equal (symbol_name, "glConvolutionParameterxvOES")) return glConvolutionParameterxvOES;
  if (g_str_equal (symbol_name, "glCopyBufferSubData")) return glCopyBufferSubData;
  if (g_str_equal (symbol_name, "glCopyColorSubTable")) return glCopyColorSubTable;
  if (g_str_equal (symbol_name, "glCopyColorTable")) return glCopyColorTable;
  if (g_str_equal (symbol_name, "glCopyConvolutionFilter1D")) return glCopyConvolutionFilter1D;
  if (g_str_equal (symbol_name, "glCopyConvolutionFilter2D")) return glCopyConvolutionFilter2D;
  if (g_str_equal (symbol_name, "glCopyImageSubData")) return glCopyImageSubData;
  if (g_str_equal (symbol_name, "glCopyTexSubImage3D")) return glCopyTexSubImage3D;
  if (g_str_equal (symbol_name, "glCreateProgramObjectARB")) return glCreateProgramObjectARB;
  if (g_str_equal (symbol_name, "glCreateProgram")) return glCreateProgram;
  if (g_str_equal (symbol_name, "glCreateShaderObjectARB")) return glCreateShaderObjectARB;
  if (g_str_equal (symbol_name, "glCreateShaderProgramv")) return glCreateShaderProgramv;
  if (g_str_equal (symbol_name, "glCreateShader")) return glCreateShader;
  if (g_str_equal (symbol_name, "glCreateSyncFromCLeventARB")) return glCreateSyncFromCLeventARB;
  if (g_str_equal (symbol_name, "glCurrentPaletteMatrixARB")) return glCurrentPaletteMatrixARB;
  if (g_str_equal (symbol_name, "glDebugMessageCallbackARB")) return glDebugMessageCallbackARB;
  if (g_str_equal (symbol_name, "glDebugMessageCallback")) return glDebugMessageCallback;
  if (g_str_equal (symbol_name, "glDebugMessageControlARB")) return glDebugMessageControlARB;
  if (g_str_equal (symbol_name, "glDebugMessageControl")) return glDebugMessageControl;
  if (g_str_equal (symbol_name, "glDebugMessageInsertARB")) return glDebugMessageInsertARB;
  if (g_str_equal (symbol_name, "glDebugMessageInsert")) return glDebugMessageInsert;
  if (g_str_equal (symbol_name, "glDeleteBuffersARB")) return glDeleteBuffersARB;
  if (g_str_equal (symbol_name, "glDeleteBuffers")) return glDeleteBuffers;
  if (g_str_equal (symbol_name, "glDeleteFramebuffers")) return glDeleteFramebuffers;
  if (g_str_equal (symbol_name, "glDeleteNamedStringARB")) return glDeleteNamedStringARB;
  if (g_str_equal (symbol_name, "glDeleteObjectARB")) return glDeleteObjectARB;
  if (g_str_equal (symbol_name, "glDeleteProgramPipelines")) return glDeleteProgramPipelines;
  if (g_str_equal (symbol_name, "glDeleteProgram")) return glDeleteProgram;
  if (g_str_equal (symbol_name, "glDeleteProgramsARB")) return glDeleteProgramsARB;
  if (g_str_equal (symbol_name, "glDeleteQueriesARB")) return glDeleteQueriesARB;
  if (g_str_equal (symbol_name, "glDeleteQueries")) return glDeleteQueries;
  if (g_str_equal (symbol_name, "glDeleteRenderbuffers")) return glDeleteRenderbuffers;
  if (g_str_equal (symbol_name, "glDeleteSamplers")) return glDeleteSamplers;
  if (g_str_equal (symbol_name, "glDeleteShader")) return glDeleteShader;
  if (g_str_equal (symbol_name, "glDeleteSync")) return glDeleteSync;
  if (g_str_equal (symbol_name, "glDeleteTransformFeedbacks")) return glDeleteTransformFeedbacks;
  if (g_str_equal (symbol_name, "glDeleteVertexArrays")) return glDeleteVertexArrays;
  if (g_str_equal (symbol_name, "glDepthRangeArrayv")) return glDepthRangeArrayv;
  if (g_str_equal (symbol_name, "glDepthRangefOES")) return glDepthRangefOES;
  if (g_str_equal (symbol_name, "glDepthRangef")) return glDepthRangef;
  if (g_str_equal (symbol_name, "glDepthRangeIndexed")) return glDepthRangeIndexed;
  if (g_str_equal (symbol_name, "glDepthRangexOES")) return glDepthRangexOES;
  if (g_str_equal (symbol_name, "glDetachObjectARB")) return glDetachObjectARB;
  if (g_str_equal (symbol_name, "glDetachShader")) return glDetachShader;
  if (g_str_equal (symbol_name, "glDisablei")) return glDisablei;
  if (g_str_equal (symbol_name, "glDisableVertexAttribArrayARB")) return glDisableVertexAttribArrayARB;
  if (g_str_equal (symbol_name, "glDisableVertexAttribArray")) return glDisableVertexAttribArray;
  if (g_str_equal (symbol_name, "glDispatchComputeGroupSizeARB")) return glDispatchComputeGroupSizeARB;
  if (g_str_equal (symbol_name, "glDispatchComputeIndirect")) return glDispatchComputeIndirect;
  if (g_str_equal (symbol_name, "glDispatchCompute")) return glDispatchCompute;
  if (g_str_equal (symbol_name, "glDrawArraysIndirect")) return glDrawArraysIndirect;
  if (g_str_equal (symbol_name, "glDrawArraysInstancedARB")) return glDrawArraysInstancedARB;
  if (g_str_equal (symbol_name, "glDrawArraysInstancedBaseInstance")) return glDrawArraysInstancedBaseInstance;
  if (g_str_equal (symbol_name, "glDrawArraysInstanced")) return glDrawArraysInstanced;
  if (g_str_equal (symbol_name, "glDrawBuffersARB")) return glDrawBuffersARB;
  if (g_str_equal (symbol_name, "glDrawBuffers")) return glDrawBuffers;
  if (g_str_equal (symbol_name, "glDrawElementsBaseVertex")) return glDrawElementsBaseVertex;
  if (g_str_equal (symbol_name, "glDrawElementsIndirect")) return glDrawElementsIndirect;
  if (g_str_equal (symbol_name, "glDrawElementsInstancedARB")) return glDrawElementsInstancedARB;
  if (g_str_equal (symbol_name, "glDrawElementsInstancedBaseInstance")) return glDrawElementsInstancedBaseInstance;
  if (g_str_equal (symbol_name, "glDrawElementsInstancedBaseVertexBaseInstance")) return glDrawElementsInstancedBaseVertexBaseInstance;
  if (g_str_equal (symbol_name, "glDrawElementsInstancedBaseVertex")) return glDrawElementsInstancedBaseVertex;
  if (g_str_equal (symbol_name, "glDrawElementsInstanced")) return glDrawElementsInstanced;
  if (g_str_equal (symbol_name, "glDrawRangeElementsBaseVertex")) return glDrawRangeElementsBaseVertex;
  if (g_str_equal (symbol_name, "glDrawRangeElements")) return glDrawRangeElements;
  if (g_str_equal (symbol_name, "glDrawTransformFeedbackInstanced")) return glDrawTransformFeedbackInstanced;
  if (g_str_equal (symbol_name, "glDrawTransformFeedback")) return glDrawTransformFeedback;
  if (g_str_equal (symbol_name, "glDrawTransformFeedbackStreamInstanced")) return glDrawTransformFeedbackStreamInstanced;
  if (g_str_equal (symbol_name, "glDrawTransformFeedbackStream")) return glDrawTransformFeedbackStream;
  if (g_str_equal (symbol_name, "glEnablei")) return glEnablei;
  if (g_str_equal (symbol_name, "glEnableVertexAttribArrayARB")) return glEnableVertexAttribArrayARB;
  if (g_str_equal (symbol_name, "glEnableVertexAttribArray")) return glEnableVertexAttribArray;
  if (g_str_equal (symbol_name, "glEndConditionalRender")) return glEndConditionalRender;
  if (g_str_equal (symbol_name, "glEndQueryARB")) return glEndQueryARB;
  if (g_str_equal (symbol_name, "glEndQueryIndexed")) return glEndQueryIndexed;
  if (g_str_equal (symbol_name, "glEndQuery")) return glEndQuery;
  if (g_str_equal (symbol_name, "glEndTransformFeedback")) return glEndTransformFeedback;
  if (g_str_equal (symbol_name, "glEvalCoord1xOES")) return glEvalCoord1xOES;
  if (g_str_equal (symbol_name, "glEvalCoord1xvOES")) return glEvalCoord1xvOES;
  if (g_str_equal (symbol_name, "glEvalCoord2xOES")) return glEvalCoord2xOES;
  if (g_str_equal (symbol_name, "glEvalCoord2xvOES")) return glEvalCoord2xvOES;
  if (g_str_equal (symbol_name, "glFeedbackBufferxOES")) return glFeedbackBufferxOES;
  if (g_str_equal (symbol_name, "glFenceSync")) return glFenceSync;
  if (g_str_equal (symbol_name, "glFlushMappedBufferRange")) return glFlushMappedBufferRange;
  if (g_str_equal (symbol_name, "glFogCoordd")) return glFogCoordd;
  if (g_str_equal (symbol_name, "glFogCoorddv")) return glFogCoorddv;
  if (g_str_equal (symbol_name, "glFogCoordf")) return glFogCoordf;
  if (g_str_equal (symbol_name, "glFogCoordfv")) return glFogCoordfv;
  if (g_str_equal (symbol_name, "glFogCoordPointer")) return glFogCoordPointer;
  if (g_str_equal (symbol_name, "glFogxOES")) return glFogxOES;
  if (g_str_equal (symbol_name, "glFogxvOES")) return glFogxvOES;
  if (g_str_equal (symbol_name, "glFramebufferParameteri")) return glFramebufferParameteri;
  if (g_str_equal (symbol_name, "glFramebufferRenderbuffer")) return glFramebufferRenderbuffer;
  if (g_str_equal (symbol_name, "glFramebufferTexture1D")) return glFramebufferTexture1D;
  if (g_str_equal (symbol_name, "glFramebufferTexture2D")) return glFramebufferTexture2D;
  if (g_str_equal (symbol_name, "glFramebufferTexture3D")) return glFramebufferTexture3D;
  if (g_str_equal (symbol_name, "glFramebufferTextureARB")) return glFramebufferTextureARB;
  if (g_str_equal (symbol_name, "glFramebufferTextureFaceARB")) return glFramebufferTextureFaceARB;
  if (g_str_equal (symbol_name, "glFramebufferTextureLayerARB")) return glFramebufferTextureLayerARB;
  if (g_str_equal (symbol_name, "glFramebufferTextureLayer")) return glFramebufferTextureLayer;
  if (g_str_equal (symbol_name, "glFramebufferTexture")) return glFramebufferTexture;
  if (g_str_equal (symbol_name, "glFrustumfOES")) return glFrustumfOES;
  if (g_str_equal (symbol_name, "glFrustumxOES")) return glFrustumxOES;
  if (g_str_equal (symbol_name, "glGenBuffersARB")) return glGenBuffersARB;
  if (g_str_equal (symbol_name, "glGenBuffers")) return glGenBuffers;
  if (g_str_equal (symbol_name, "glGenerateMipmap")) return glGenerateMipmap;
  if (g_str_equal (symbol_name, "glGenFramebuffers")) return glGenFramebuffers;
  if (g_str_equal (symbol_name, "glGenProgramPipelines")) return glGenProgramPipelines;
  if (g_str_equal (symbol_name, "glGenProgramsARB")) return glGenProgramsARB;
  if (g_str_equal (symbol_name, "glGenQueriesARB")) return glGenQueriesARB;
  if (g_str_equal (symbol_name, "glGenQueries")) return glGenQueries;
  if (g_str_equal (symbol_name, "glGenRenderbuffers")) return glGenRenderbuffers;
  if (g_str_equal (symbol_name, "glGenSamplers")) return glGenSamplers;
  if (g_str_equal (symbol_name, "glGenTransformFeedbacks")) return glGenTransformFeedbacks;
  if (g_str_equal (symbol_name, "glGenVertexArrays")) return glGenVertexArrays;
  if (g_str_equal (symbol_name, "glGetActiveAtomicCounterBufferiv")) return glGetActiveAtomicCounterBufferiv;
  if (g_str_equal (symbol_name, "glGetActiveAttribARB")) return glGetActiveAttribARB;
  if (g_str_equal (symbol_name, "glGetActiveAttrib")) return glGetActiveAttrib;
  if (g_str_equal (symbol_name, "glGetActiveSubroutineName")) return glGetActiveSubroutineName;
  if (g_str_equal (symbol_name, "glGetActiveSubroutineUniformiv")) return glGetActiveSubroutineUniformiv;
  if (g_str_equal (symbol_name, "glGetActiveSubroutineUniformName")) return glGetActiveSubroutineUniformName;
  if (g_str_equal (symbol_name, "glGetActiveUniformARB")) return glGetActiveUniformARB;
  if (g_str_equal (symbol_name, "glGetActiveUniformBlockiv")) return glGetActiveUniformBlockiv;
  if (g_str_equal (symbol_name, "glGetActiveUniformBlockName")) return glGetActiveUniformBlockName;
  if (g_str_equal (symbol_name, "glGetActiveUniformName")) return glGetActiveUniformName;
  if (g_str_equal (symbol_name, "glGetActiveUniform")) return glGetActiveUniform;
  if (g_str_equal (symbol_name, "glGetActiveUniformsiv")) return glGetActiveUniformsiv;
  if (g_str_equal (symbol_name, "glGetAttachedObjectsARB")) return glGetAttachedObjectsARB;
  if (g_str_equal (symbol_name, "glGetAttachedShaders")) return glGetAttachedShaders;
  if (g_str_equal (symbol_name, "glGetAttribLocationARB")) return glGetAttribLocationARB;
  if (g_str_equal (symbol_name, "glGetAttribLocation")) return glGetAttribLocation;
  if (g_str_equal (symbol_name, "glGetBooleani_v")) return glGetBooleani_v;
  if (g_str_equal (symbol_name, "glGetBufferParameteri64v")) return glGetBufferParameteri64v;
  if (g_str_equal (symbol_name, "glGetBufferParameterivARB")) return glGetBufferParameterivARB;
  if (g_str_equal (symbol_name, "glGetBufferParameteriv")) return glGetBufferParameteriv;
  if (g_str_equal (symbol_name, "glGetBufferPointervARB")) return glGetBufferPointervARB;
  if (g_str_equal (symbol_name, "glGetBufferPointerv")) return glGetBufferPointerv;
  if (g_str_equal (symbol_name, "glGetBufferSubDataARB")) return glGetBufferSubDataARB;
  if (g_str_equal (symbol_name, "glGetBufferSubData")) return glGetBufferSubData;
  if (g_str_equal (symbol_name, "glGetClipPlanefOES")) return glGetClipPlanefOES;
  if (g_str_equal (symbol_name, "glGetClipPlanexOES")) return glGetClipPlanexOES;
  if (g_str_equal (symbol_name, "glGetColorTableParameterfv")) return glGetColorTableParameterfv;
  if (g_str_equal (symbol_name, "glGetColorTableParameteriv")) return glGetColorTableParameteriv;
  if (g_str_equal (symbol_name, "glGetColorTable")) return glGetColorTable;
  if (g_str_equal (symbol_name, "glGetCompressedTexImageARB")) return glGetCompressedTexImageARB;
  if (g_str_equal (symbol_name, "glGetCompressedTexImage")) return glGetCompressedTexImage;
  if (g_str_equal (symbol_name, "glGetConvolutionFilter")) return glGetConvolutionFilter;
  if (g_str_equal (symbol_name, "glGetConvolutionParameterfv")) return glGetConvolutionParameterfv;
  if (g_str_equal (symbol_name, "glGetConvolutionParameteriv")) return glGetConvolutionParameteriv;
  if (g_str_equal (symbol_name, "glGetConvolutionParameterxvOES")) return glGetConvolutionParameterxvOES;
  if (g_str_equal (symbol_name, "glGetDebugMessageLogARB")) return glGetDebugMessageLogARB;
  if (g_str_equal (symbol_name, "glGetDebugMessageLog")) return glGetDebugMessageLog;
  if (g_str_equal (symbol_name, "glGetDoublei_v")) return glGetDoublei_v;
  if (g_str_equal (symbol_name, "glGetFixedvOES")) return glGetFixedvOES;
  if (g_str_equal (symbol_name, "glGetFloati_v")) return glGetFloati_v;
  if (g_str_equal (symbol_name, "glGetFragDataIndex")) return glGetFragDataIndex;
  if (g_str_equal (symbol_name, "glGetFragDataLocation")) return glGetFragDataLocation;
  if (g_str_equal (symbol_name, "glGetFramebufferAttachmentParameteriv")) return glGetFramebufferAttachmentParameteriv;
  if (g_str_equal (symbol_name, "glGetFramebufferParameteriv")) return glGetFramebufferParameteriv;
  if (g_str_equal (symbol_name, "glGetGraphicsResetStatusARB")) return glGetGraphicsResetStatusARB;
  if (g_str_equal (symbol_name, "glGetHandleARB")) return glGetHandleARB;
  if (g_str_equal (symbol_name, "glGetHistogramParameterfv")) return glGetHistogramParameterfv;
  if (g_str_equal (symbol_name, "glGetHistogramParameteriv")) return glGetHistogramParameteriv;
  if (g_str_equal (symbol_name, "glGetHistogramParameterxvOES")) return glGetHistogramParameterxvOES;
  if (g_str_equal (symbol_name, "glGetHistogram")) return glGetHistogram;
  if (g_str_equal (symbol_name, "glGetImageHandleARB")) return glGetImageHandleARB;
  if (g_str_equal (symbol_name, "glGetImageTransformParameterfvHP")) return glGetImageTransformParameterfvHP;
  if (g_str_equal (symbol_name, "glGetImageTransformParameterivHP")) return glGetImageTransformParameterivHP;
  if (g_str_equal (symbol_name, "glGetInfoLogARB")) return glGetInfoLogARB;
  if (g_str_equal (symbol_name, "glGetInteger64i_v")) return glGetInteger64i_v;
  if (g_str_equal (symbol_name, "glGetInteger64v")) return glGetInteger64v;
  if (g_str_equal (symbol_name, "glGetIntegeri_v")) return glGetIntegeri_v;
  if (g_str_equal (symbol_name, "glGetInternalformati64v")) return glGetInternalformati64v;
  if (g_str_equal (symbol_name, "glGetInternalformativ")) return glGetInternalformativ;
  if (g_str_equal (symbol_name, "glGetLightxOES")) return glGetLightxOES;
  if (g_str_equal (symbol_name, "glGetMapxvOES")) return glGetMapxvOES;
  if (g_str_equal (symbol_name, "glGetMaterialxOES")) return glGetMaterialxOES;
  if (g_str_equal (symbol_name, "glGetMinmaxParameterfv")) return glGetMinmaxParameterfv;
  if (g_str_equal (symbol_name, "glGetMinmaxParameteriv")) return glGetMinmaxParameteriv;
  if (g_str_equal (symbol_name, "glGetMinmax")) return glGetMinmax;
  if (g_str_equal (symbol_name, "glGetMultisamplefv")) return glGetMultisamplefv;
  if (g_str_equal (symbol_name, "glGetNamedStringARB")) return glGetNamedStringARB;
  if (g_str_equal (symbol_name, "glGetNamedStringivARB")) return glGetNamedStringivARB;
  if (g_str_equal (symbol_name, "glGetnColorTableARB")) return glGetnColorTableARB;
  if (g_str_equal (symbol_name, "glGetnCompressedTexImageARB")) return glGetnCompressedTexImageARB;
  if (g_str_equal (symbol_name, "glGetnConvolutionFilterARB")) return glGetnConvolutionFilterARB;
  if (g_str_equal (symbol_name, "glGetnHistogramARB")) return glGetnHistogramARB;
  if (g_str_equal (symbol_name, "glGetnMapdvARB")) return glGetnMapdvARB;
  if (g_str_equal (symbol_name, "glGetnMapfvARB")) return glGetnMapfvARB;
  if (g_str_equal (symbol_name, "glGetnMapivARB")) return glGetnMapivARB;
  if (g_str_equal (symbol_name, "glGetnMinmaxARB")) return glGetnMinmaxARB;
  if (g_str_equal (symbol_name, "glGetnPixelMapfvARB")) return glGetnPixelMapfvARB;
  if (g_str_equal (symbol_name, "glGetnPixelMapuivARB")) return glGetnPixelMapuivARB;
  if (g_str_equal (symbol_name, "glGetnPixelMapusvARB")) return glGetnPixelMapusvARB;
  if (g_str_equal (symbol_name, "glGetnPolygonStippleARB")) return glGetnPolygonStippleARB;
  if (g_str_equal (symbol_name, "glGetnSeparableFilterARB")) return glGetnSeparableFilterARB;
  if (g_str_equal (symbol_name, "glGetnTexImageARB")) return glGetnTexImageARB;
  if (g_str_equal (symbol_name, "glGetnUniformdvARB")) return glGetnUniformdvARB;
  if (g_str_equal (symbol_name, "glGetnUniformfvARB")) return glGetnUniformfvARB;
  if (g_str_equal (symbol_name, "glGetnUniformivARB")) return glGetnUniformivARB;
  if (g_str_equal (symbol_name, "glGetnUniformuivARB")) return glGetnUniformuivARB;
  if (g_str_equal (symbol_name, "glGetObjectLabel")) return glGetObjectLabel;
  if (g_str_equal (symbol_name, "glGetObjectParameterfvARB")) return glGetObjectParameterfvARB;
  if (g_str_equal (symbol_name, "glGetObjectParameterivARB")) return glGetObjectParameterivARB;
  if (g_str_equal (symbol_name, "glGetObjectPtrLabel")) return glGetObjectPtrLabel;
  if (g_str_equal (symbol_name, "glGetPixelMapxv")) return glGetPixelMapxv;
  if (g_str_equal (symbol_name, "glGetProgramBinary")) return glGetProgramBinary;
  if (g_str_equal (symbol_name, "glGetProgramEnvParameterdvARB")) return glGetProgramEnvParameterdvARB;
  if (g_str_equal (symbol_name, "glGetProgramEnvParameterfvARB")) return glGetProgramEnvParameterfvARB;
  if (g_str_equal (symbol_name, "glGetProgramInfoLog")) return glGetProgramInfoLog;
  if (g_str_equal (symbol_name, "glGetProgramInterfaceiv")) return glGetProgramInterfaceiv;
  if (g_str_equal (symbol_name, "glGetProgramivARB")) return glGetProgramivARB;
  if (g_str_equal (symbol_name, "glGetProgramiv")) return glGetProgramiv;
  if (g_str_equal (symbol_name, "glGetProgramLocalParameterdvARB")) return glGetProgramLocalParameterdvARB;
  if (g_str_equal (symbol_name, "glGetProgramLocalParameterfvARB")) return glGetProgramLocalParameterfvARB;
  if (g_str_equal (symbol_name, "glGetProgramPipelineInfoLog")) return glGetProgramPipelineInfoLog;
  if (g_str_equal (symbol_name, "glGetProgramPipelineiv")) return glGetProgramPipelineiv;
  if (g_str_equal (symbol_name, "glGetProgramResourceIndex")) return glGetProgramResourceIndex;
  if (g_str_equal (symbol_name, "glGetProgramResourceiv")) return glGetProgramResourceiv;
  if (g_str_equal (symbol_name, "glGetProgramResourceLocationIndex")) return glGetProgramResourceLocationIndex;
  if (g_str_equal (symbol_name, "glGetProgramResourceLocation")) return glGetProgramResourceLocation;
  if (g_str_equal (symbol_name, "glGetProgramResourceName")) return glGetProgramResourceName;
  if (g_str_equal (symbol_name, "glGetProgramStageiv")) return glGetProgramStageiv;
  if (g_str_equal (symbol_name, "glGetProgramStringARB")) return glGetProgramStringARB;
  if (g_str_equal (symbol_name, "glGetQueryIndexediv")) return glGetQueryIndexediv;
  if (g_str_equal (symbol_name, "glGetQueryivARB")) return glGetQueryivARB;
  if (g_str_equal (symbol_name, "glGetQueryiv")) return glGetQueryiv;
  if (g_str_equal (symbol_name, "glGetQueryObjecti64v")) return glGetQueryObjecti64v;
  if (g_str_equal (symbol_name, "glGetQueryObjectivARB")) return glGetQueryObjectivARB;
  if (g_str_equal (symbol_name, "glGetQueryObjectiv")) return glGetQueryObjectiv;
  if (g_str_equal (symbol_name, "glGetQueryObjectui64v")) return glGetQueryObjectui64v;
  if (g_str_equal (symbol_name, "glGetQueryObjectuivARB")) return glGetQueryObjectuivARB;
  if (g_str_equal (symbol_name, "glGetQueryObjectuiv")) return glGetQueryObjectuiv;
  if (g_str_equal (symbol_name, "glGetRenderbufferParameteriv")) return glGetRenderbufferParameteriv;
  if (g_str_equal (symbol_name, "glGetSamplerParameterfv")) return glGetSamplerParameterfv;
  if (g_str_equal (symbol_name, "glGetSamplerParameterIiv")) return glGetSamplerParameterIiv;
  if (g_str_equal (symbol_name, "glGetSamplerParameterIuiv")) return glGetSamplerParameterIuiv;
  if (g_str_equal (symbol_name, "glGetSamplerParameteriv")) return glGetSamplerParameteriv;
  if (g_str_equal (symbol_name, "glGetSeparableFilter")) return glGetSeparableFilter;
  if (g_str_equal (symbol_name, "glGetShaderInfoLog")) return glGetShaderInfoLog;
  if (g_str_equal (symbol_name, "glGetShaderiv")) return glGetShaderiv;
  if (g_str_equal (symbol_name, "glGetShaderPrecisionFormat")) return glGetShaderPrecisionFormat;
  if (g_str_equal (symbol_name, "glGetShaderSourceARB")) return glGetShaderSourceARB;
  if (g_str_equal (symbol_name, "glGetShaderSource")) return glGetShaderSource;
  if (g_str_equal (symbol_name, "glGetStringi")) return glGetStringi;
  if (g_str_equal (symbol_name, "glGetSubroutineIndex")) return glGetSubroutineIndex;
  if (g_str_equal (symbol_name, "glGetSubroutineUniformLocation")) return glGetSubroutineUniformLocation;
  if (g_str_equal (symbol_name, "glGetSynciv")) return glGetSynciv;
  if (g_str_equal (symbol_name, "glGetTexEnvxvOES")) return glGetTexEnvxvOES;
  if (g_str_equal (symbol_name, "glGetTexGenxvOES")) return glGetTexGenxvOES;
  if (g_str_equal (symbol_name, "glGetTexLevelParameterxvOES")) return glGetTexLevelParameterxvOES;
  if (g_str_equal (symbol_name, "glGetTexParameterIiv")) return glGetTexParameterIiv;
  if (g_str_equal (symbol_name, "glGetTexParameterIuiv")) return glGetTexParameterIuiv;
  if (g_str_equal (symbol_name, "glGetTexParameterxvOES")) return glGetTexParameterxvOES;
  if (g_str_equal (symbol_name, "glGetTextureHandleARB")) return glGetTextureHandleARB;
  if (g_str_equal (symbol_name, "glGetTextureSamplerHandleARB")) return glGetTextureSamplerHandleARB;
  if (g_str_equal (symbol_name, "glGetTransformFeedbackVarying")) return glGetTransformFeedbackVarying;
  if (g_str_equal (symbol_name, "glGetUniformBlockIndex")) return glGetUniformBlockIndex;
  if (g_str_equal (symbol_name, "glGetUniformdv")) return glGetUniformdv;
  if (g_str_equal (symbol_name, "glGetUniformfvARB")) return glGetUniformfvARB;
  if (g_str_equal (symbol_name, "glGetUniformfv")) return glGetUniformfv;
  if (g_str_equal (symbol_name, "glGetUniformIndices")) return glGetUniformIndices;
  if (g_str_equal (symbol_name, "glGetUniformivARB")) return glGetUniformivARB;
  if (g_str_equal (symbol_name, "glGetUniformiv")) return glGetUniformiv;
  if (g_str_equal (symbol_name, "glGetUniformLocationARB")) return glGetUniformLocationARB;
  if (g_str_equal (symbol_name, "glGetUniformLocation")) return glGetUniformLocation;
  if (g_str_equal (symbol_name, "glGetUniformSubroutineuiv")) return glGetUniformSubroutineuiv;
  if (g_str_equal (symbol_name, "glGetUniformuiv")) return glGetUniformuiv;
  if (g_str_equal (symbol_name, "glGetVertexAttribdvARB")) return glGetVertexAttribdvARB;
  if (g_str_equal (symbol_name, "glGetVertexAttribdv")) return glGetVertexAttribdv;
  if (g_str_equal (symbol_name, "glGetVertexAttribfvARB")) return glGetVertexAttribfvARB;
  if (g_str_equal (symbol_name, "glGetVertexAttribfv")) return glGetVertexAttribfv;
  if (g_str_equal (symbol_name, "glGetVertexAttribIiv")) return glGetVertexAttribIiv;
  if (g_str_equal (symbol_name, "glGetVertexAttribIuiv")) return glGetVertexAttribIuiv;
  if (g_str_equal (symbol_name, "glGetVertexAttribivARB")) return glGetVertexAttribivARB;
  if (g_str_equal (symbol_name, "glGetVertexAttribiv")) return glGetVertexAttribiv;
  if (g_str_equal (symbol_name, "glGetVertexAttribLdv")) return glGetVertexAttribLdv;
  if (g_str_equal (symbol_name, "glGetVertexAttribLui64vARB")) return glGetVertexAttribLui64vARB;
  if (g_str_equal (symbol_name, "glGetVertexAttribPointervARB")) return glGetVertexAttribPointervARB;
  if (g_str_equal (symbol_name, "glGetVertexAttribPointerv")) return glGetVertexAttribPointerv;
  if (g_str_equal (symbol_name, "glHistogram")) return glHistogram;
  if (g_str_equal (symbol_name, "glImageTransformParameterfHP")) return glImageTransformParameterfHP;
  if (g_str_equal (symbol_name, "glImageTransformParameterfvHP")) return glImageTransformParameterfvHP;
  if (g_str_equal (symbol_name, "glImageTransformParameteriHP")) return glImageTransformParameteriHP;
  if (g_str_equal (symbol_name, "glImageTransformParameterivHP")) return glImageTransformParameterivHP;
  if (g_str_equal (symbol_name, "glIndexxOES")) return glIndexxOES;
  if (g_str_equal (symbol_name, "glIndexxvOES")) return glIndexxvOES;
  if (g_str_equal (symbol_name, "glInvalidateBufferData")) return glInvalidateBufferData;
  if (g_str_equal (symbol_name, "glInvalidateBufferSubData")) return glInvalidateBufferSubData;
  if (g_str_equal (symbol_name, "glInvalidateFramebuffer")) return glInvalidateFramebuffer;
  if (g_str_equal (symbol_name, "glInvalidateSubFramebuffer")) return glInvalidateSubFramebuffer;
  if (g_str_equal (symbol_name, "glInvalidateTexImage")) return glInvalidateTexImage;
  if (g_str_equal (symbol_name, "glInvalidateTexSubImage")) return glInvalidateTexSubImage;
  if (g_str_equal (symbol_name, "glIsBufferARB")) return glIsBufferARB;
  if (g_str_equal (symbol_name, "glIsBuffer")) return glIsBuffer;
  if (g_str_equal (symbol_name, "glIsEnabledi")) return glIsEnabledi;
  if (g_str_equal (symbol_name, "glIsFramebuffer")) return glIsFramebuffer;
  if (g_str_equal (symbol_name, "glIsImageHandleResidentARB")) return glIsImageHandleResidentARB;
  if (g_str_equal (symbol_name, "glIsNamedStringARB")) return glIsNamedStringARB;
  if (g_str_equal (symbol_name, "glIsProgramARB")) return glIsProgramARB;
  if (g_str_equal (symbol_name, "glIsProgramPipeline")) return glIsProgramPipeline;
  if (g_str_equal (symbol_name, "glIsProgram")) return glIsProgram;
  if (g_str_equal (symbol_name, "glIsQueryARB")) return glIsQueryARB;
  if (g_str_equal (symbol_name, "glIsQuery")) return glIsQuery;
  if (g_str_equal (symbol_name, "glIsRenderbuffer")) return glIsRenderbuffer;
  if (g_str_equal (symbol_name, "glIsSampler")) return glIsSampler;
  if (g_str_equal (symbol_name, "glIsShader")) return glIsShader;
  if (g_str_equal (symbol_name, "glIsSync")) return glIsSync;
  if (g_str_equal (symbol_name, "glIsTextureHandleResidentARB")) return glIsTextureHandleResidentARB;
  if (g_str_equal (symbol_name, "glIsTransformFeedback")) return glIsTransformFeedback;
  if (g_str_equal (symbol_name, "glIsVertexArray")) return glIsVertexArray;
  if (g_str_equal (symbol_name, "glLightModelxOES")) return glLightModelxOES;
  if (g_str_equal (symbol_name, "glLightModelxvOES")) return glLightModelxvOES;
  if (g_str_equal (symbol_name, "glLightxOES")) return glLightxOES;
  if (g_str_equal (symbol_name, "glLightxvOES")) return glLightxvOES;
  if (g_str_equal (symbol_name, "glLineWidthxOES")) return glLineWidthxOES;
  if (g_str_equal (symbol_name, "glLinkProgramARB")) return glLinkProgramARB;
  if (g_str_equal (symbol_name, "glLinkProgram")) return glLinkProgram;
  if (g_str_equal (symbol_name, "glLoadMatrixxOES")) return glLoadMatrixxOES;
  if (g_str_equal (symbol_name, "glLoadTransposeMatrixdARB")) return glLoadTransposeMatrixdARB;
  if (g_str_equal (symbol_name, "glLoadTransposeMatrixd")) return glLoadTransposeMatrixd;
  if (g_str_equal (symbol_name, "glLoadTransposeMatrixfARB")) return glLoadTransposeMatrixfARB;
  if (g_str_equal (symbol_name, "glLoadTransposeMatrixf")) return glLoadTransposeMatrixf;
  if (g_str_equal (symbol_name, "glLoadTransposeMatrixxOES")) return glLoadTransposeMatrixxOES;
  if (g_str_equal (symbol_name, "glMakeImageHandleNonResidentARB")) return glMakeImageHandleNonResidentARB;
  if (g_str_equal (symbol_name, "glMakeImageHandleResidentARB")) return glMakeImageHandleResidentARB;
  if (g_str_equal (symbol_name, "glMakeTextureHandleNonResidentARB")) return glMakeTextureHandleNonResidentARB;
  if (g_str_equal (symbol_name, "glMakeTextureHandleResidentARB")) return glMakeTextureHandleResidentARB;
  if (g_str_equal (symbol_name, "glMap1xOES")) return glMap1xOES;
  if (g_str_equal (symbol_name, "glMap2xOES")) return glMap2xOES;
  if (g_str_equal (symbol_name, "glMapBufferARB")) return glMapBufferARB;
  if (g_str_equal (symbol_name, "glMapBufferRange")) return glMapBufferRange;
  if (g_str_equal (symbol_name, "glMapBuffer")) return glMapBuffer;
  if (g_str_equal (symbol_name, "glMapGrid1xOES")) return glMapGrid1xOES;
  if (g_str_equal (symbol_name, "glMapGrid2xOES")) return glMapGrid2xOES;
  if (g_str_equal (symbol_name, "glMaterialxOES")) return glMaterialxOES;
  if (g_str_equal (symbol_name, "glMaterialxvOES")) return glMaterialxvOES;
  if (g_str_equal (symbol_name, "glMatrixIndexPointerARB")) return glMatrixIndexPointerARB;
  if (g_str_equal (symbol_name, "glMatrixIndexubvARB")) return glMatrixIndexubvARB;
  if (g_str_equal (symbol_name, "glMatrixIndexuivARB")) return glMatrixIndexuivARB;
  if (g_str_equal (symbol_name, "glMatrixIndexusvARB")) return glMatrixIndexusvARB;
  if (g_str_equal (symbol_name, "glMemoryBarrier")) return glMemoryBarrier;
  if (g_str_equal (symbol_name, "glMinmax")) return glMinmax;
  if (g_str_equal (symbol_name, "glMinSampleShadingARB")) return glMinSampleShadingARB;
  if (g_str_equal (symbol_name, "glMinSampleShading")) return glMinSampleShading;
  if (g_str_equal (symbol_name, "glMultiDrawArraysIndirectCountARB")) return glMultiDrawArraysIndirectCountARB;
  if (g_str_equal (symbol_name, "glMultiDrawArraysIndirect")) return glMultiDrawArraysIndirect;
  if (g_str_equal (symbol_name, "glMultiDrawArrays")) return glMultiDrawArrays;
  if (g_str_equal (symbol_name, "glMultiDrawElementsBaseVertex")) return glMultiDrawElementsBaseVertex;
  if (g_str_equal (symbol_name, "glMultiDrawElementsIndirectCountARB")) return glMultiDrawElementsIndirectCountARB;
  if (g_str_equal (symbol_name, "glMultiDrawElementsIndirect")) return glMultiDrawElementsIndirect;
  if (g_str_equal (symbol_name, "glMultiDrawElements")) return glMultiDrawElements;
  if (g_str_equal (symbol_name, "glMultiTexCoord1bOES")) return glMultiTexCoord1bOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord1bvOES")) return glMultiTexCoord1bvOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord1dARB")) return glMultiTexCoord1dARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord1d")) return glMultiTexCoord1d;
  if (g_str_equal (symbol_name, "glMultiTexCoord1dvARB")) return glMultiTexCoord1dvARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord1dv")) return glMultiTexCoord1dv;
  if (g_str_equal (symbol_name, "glMultiTexCoord1fARB")) return glMultiTexCoord1fARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord1f")) return glMultiTexCoord1f;
  if (g_str_equal (symbol_name, "glMultiTexCoord1fvARB")) return glMultiTexCoord1fvARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord1fv")) return glMultiTexCoord1fv;
  if (g_str_equal (symbol_name, "glMultiTexCoord1iARB")) return glMultiTexCoord1iARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord1i")) return glMultiTexCoord1i;
  if (g_str_equal (symbol_name, "glMultiTexCoord1ivARB")) return glMultiTexCoord1ivARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord1iv")) return glMultiTexCoord1iv;
  if (g_str_equal (symbol_name, "glMultiTexCoord1sARB")) return glMultiTexCoord1sARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord1s")) return glMultiTexCoord1s;
  if (g_str_equal (symbol_name, "glMultiTexCoord1svARB")) return glMultiTexCoord1svARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord1sv")) return glMultiTexCoord1sv;
  if (g_str_equal (symbol_name, "glMultiTexCoord1xOES")) return glMultiTexCoord1xOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord1xvOES")) return glMultiTexCoord1xvOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord2bOES")) return glMultiTexCoord2bOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord2bvOES")) return glMultiTexCoord2bvOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord2dARB")) return glMultiTexCoord2dARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord2d")) return glMultiTexCoord2d;
  if (g_str_equal (symbol_name, "glMultiTexCoord2dvARB")) return glMultiTexCoord2dvARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord2dv")) return glMultiTexCoord2dv;
  if (g_str_equal (symbol_name, "glMultiTexCoord2fARB")) return glMultiTexCoord2fARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord2f")) return glMultiTexCoord2f;
  if (g_str_equal (symbol_name, "glMultiTexCoord2fvARB")) return glMultiTexCoord2fvARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord2fv")) return glMultiTexCoord2fv;
  if (g_str_equal (symbol_name, "glMultiTexCoord2iARB")) return glMultiTexCoord2iARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord2i")) return glMultiTexCoord2i;
  if (g_str_equal (symbol_name, "glMultiTexCoord2ivARB")) return glMultiTexCoord2ivARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord2iv")) return glMultiTexCoord2iv;
  if (g_str_equal (symbol_name, "glMultiTexCoord2sARB")) return glMultiTexCoord2sARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord2s")) return glMultiTexCoord2s;
  if (g_str_equal (symbol_name, "glMultiTexCoord2svARB")) return glMultiTexCoord2svARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord2sv")) return glMultiTexCoord2sv;
  if (g_str_equal (symbol_name, "glMultiTexCoord2xOES")) return glMultiTexCoord2xOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord2xvOES")) return glMultiTexCoord2xvOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord3bOES")) return glMultiTexCoord3bOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord3bvOES")) return glMultiTexCoord3bvOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord3dARB")) return glMultiTexCoord3dARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord3d")) return glMultiTexCoord3d;
  if (g_str_equal (symbol_name, "glMultiTexCoord3dvARB")) return glMultiTexCoord3dvARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord3dv")) return glMultiTexCoord3dv;
  if (g_str_equal (symbol_name, "glMultiTexCoord3fARB")) return glMultiTexCoord3fARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord3f")) return glMultiTexCoord3f;
  if (g_str_equal (symbol_name, "glMultiTexCoord3fvARB")) return glMultiTexCoord3fvARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord3fv")) return glMultiTexCoord3fv;
  if (g_str_equal (symbol_name, "glMultiTexCoord3iARB")) return glMultiTexCoord3iARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord3i")) return glMultiTexCoord3i;
  if (g_str_equal (symbol_name, "glMultiTexCoord3ivARB")) return glMultiTexCoord3ivARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord3iv")) return glMultiTexCoord3iv;
  if (g_str_equal (symbol_name, "glMultiTexCoord3sARB")) return glMultiTexCoord3sARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord3s")) return glMultiTexCoord3s;
  if (g_str_equal (symbol_name, "glMultiTexCoord3svARB")) return glMultiTexCoord3svARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord3sv")) return glMultiTexCoord3sv;
  if (g_str_equal (symbol_name, "glMultiTexCoord3xOES")) return glMultiTexCoord3xOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord3xvOES")) return glMultiTexCoord3xvOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord4bOES")) return glMultiTexCoord4bOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord4bvOES")) return glMultiTexCoord4bvOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord4dARB")) return glMultiTexCoord4dARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord4d")) return glMultiTexCoord4d;
  if (g_str_equal (symbol_name, "glMultiTexCoord4dvARB")) return glMultiTexCoord4dvARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord4dv")) return glMultiTexCoord4dv;
  if (g_str_equal (symbol_name, "glMultiTexCoord4fARB")) return glMultiTexCoord4fARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord4f")) return glMultiTexCoord4f;
  if (g_str_equal (symbol_name, "glMultiTexCoord4fvARB")) return glMultiTexCoord4fvARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord4fv")) return glMultiTexCoord4fv;
  if (g_str_equal (symbol_name, "glMultiTexCoord4iARB")) return glMultiTexCoord4iARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord4i")) return glMultiTexCoord4i;
  if (g_str_equal (symbol_name, "glMultiTexCoord4ivARB")) return glMultiTexCoord4ivARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord4iv")) return glMultiTexCoord4iv;
  if (g_str_equal (symbol_name, "glMultiTexCoord4sARB")) return glMultiTexCoord4sARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord4s")) return glMultiTexCoord4s;
  if (g_str_equal (symbol_name, "glMultiTexCoord4svARB")) return glMultiTexCoord4svARB;
  if (g_str_equal (symbol_name, "glMultiTexCoord4sv")) return glMultiTexCoord4sv;
  if (g_str_equal (symbol_name, "glMultiTexCoord4xOES")) return glMultiTexCoord4xOES;
  if (g_str_equal (symbol_name, "glMultiTexCoord4xvOES")) return glMultiTexCoord4xvOES;
  if (g_str_equal (symbol_name, "glMultiTexCoordP1ui")) return glMultiTexCoordP1ui;
  if (g_str_equal (symbol_name, "glMultiTexCoordP1uiv")) return glMultiTexCoordP1uiv;
  if (g_str_equal (symbol_name, "glMultiTexCoordP2ui")) return glMultiTexCoordP2ui;
  if (g_str_equal (symbol_name, "glMultiTexCoordP2uiv")) return glMultiTexCoordP2uiv;
  if (g_str_equal (symbol_name, "glMultiTexCoordP3ui")) return glMultiTexCoordP3ui;
  if (g_str_equal (symbol_name, "glMultiTexCoordP3uiv")) return glMultiTexCoordP3uiv;
  if (g_str_equal (symbol_name, "glMultiTexCoordP4ui")) return glMultiTexCoordP4ui;
  if (g_str_equal (symbol_name, "glMultiTexCoordP4uiv")) return glMultiTexCoordP4uiv;
  if (g_str_equal (symbol_name, "glMultMatrixxOES")) return glMultMatrixxOES;
  if (g_str_equal (symbol_name, "glMultTransposeMatrixdARB")) return glMultTransposeMatrixdARB;
  if (g_str_equal (symbol_name, "glMultTransposeMatrixd")) return glMultTransposeMatrixd;
  if (g_str_equal (symbol_name, "glMultTransposeMatrixfARB")) return glMultTransposeMatrixfARB;
  if (g_str_equal (symbol_name, "glMultTransposeMatrixf")) return glMultTransposeMatrixf;
  if (g_str_equal (symbol_name, "glMultTransposeMatrixxOES")) return glMultTransposeMatrixxOES;
  if (g_str_equal (symbol_name, "glNamedStringARB")) return glNamedStringARB;
  if (g_str_equal (symbol_name, "glNormal3xOES")) return glNormal3xOES;
  if (g_str_equal (symbol_name, "glNormal3xvOES")) return glNormal3xvOES;
  if (g_str_equal (symbol_name, "glNormalP3ui")) return glNormalP3ui;
  if (g_str_equal (symbol_name, "glNormalP3uiv")) return glNormalP3uiv;
  if (g_str_equal (symbol_name, "glObjectLabel")) return glObjectLabel;
  if (g_str_equal (symbol_name, "glObjectPtrLabel")) return glObjectPtrLabel;
  if (g_str_equal (symbol_name, "glOrthofOES")) return glOrthofOES;
  if (g_str_equal (symbol_name, "glOrthoxOES")) return glOrthoxOES;
  if (g_str_equal (symbol_name, "glPassThroughxOES")) return glPassThroughxOES;
  if (g_str_equal (symbol_name, "glPatchParameterfv")) return glPatchParameterfv;
  if (g_str_equal (symbol_name, "glPatchParameteri")) return glPatchParameteri;
  if (g_str_equal (symbol_name, "glPauseTransformFeedback")) return glPauseTransformFeedback;
  if (g_str_equal (symbol_name, "glPixelMapx")) return glPixelMapx;
  if (g_str_equal (symbol_name, "glPixelStorex")) return glPixelStorex;
  if (g_str_equal (symbol_name, "glPixelTransferxOES")) return glPixelTransferxOES;
  if (g_str_equal (symbol_name, "glPixelZoomxOES")) return glPixelZoomxOES;
  if (g_str_equal (symbol_name, "glPointParameterfARB")) return glPointParameterfARB;
  if (g_str_equal (symbol_name, "glPointParameterf")) return glPointParameterf;
  if (g_str_equal (symbol_name, "glPointParameterfvARB")) return glPointParameterfvARB;
  if (g_str_equal (symbol_name, "glPointParameterfv")) return glPointParameterfv;
  if (g_str_equal (symbol_name, "glPointParameteri")) return glPointParameteri;
  if (g_str_equal (symbol_name, "glPointParameteriv")) return glPointParameteriv;
  if (g_str_equal (symbol_name, "glPointParameterxvOES")) return glPointParameterxvOES;
  if (g_str_equal (symbol_name, "glPointSizexOES")) return glPointSizexOES;
  if (g_str_equal (symbol_name, "glPolygonOffsetxOES")) return glPolygonOffsetxOES;
  if (g_str_equal (symbol_name, "glPopDebugGroup")) return glPopDebugGroup;
  if (g_str_equal (symbol_name, "glPrimitiveRestartIndex")) return glPrimitiveRestartIndex;
  if (g_str_equal (symbol_name, "glPrioritizeTexturesxOES")) return glPrioritizeTexturesxOES;
  if (g_str_equal (symbol_name, "glProgramBinary")) return glProgramBinary;
  if (g_str_equal (symbol_name, "glProgramEnvParameter4dARB")) return glProgramEnvParameter4dARB;
  if (g_str_equal (symbol_name, "glProgramEnvParameter4dvARB")) return glProgramEnvParameter4dvARB;
  if (g_str_equal (symbol_name, "glProgramEnvParameter4fARB")) return glProgramEnvParameter4fARB;
  if (g_str_equal (symbol_name, "glProgramEnvParameter4fvARB")) return glProgramEnvParameter4fvARB;
  if (g_str_equal (symbol_name, "glProgramLocalParameter4dARB")) return glProgramLocalParameter4dARB;
  if (g_str_equal (symbol_name, "glProgramLocalParameter4dvARB")) return glProgramLocalParameter4dvARB;
  if (g_str_equal (symbol_name, "glProgramLocalParameter4fARB")) return glProgramLocalParameter4fARB;
  if (g_str_equal (symbol_name, "glProgramLocalParameter4fvARB")) return glProgramLocalParameter4fvARB;
  if (g_str_equal (symbol_name, "glProgramParameteriARB")) return glProgramParameteriARB;
  if (g_str_equal (symbol_name, "glProgramParameteri")) return glProgramParameteri;
  if (g_str_equal (symbol_name, "glProgramStringARB")) return glProgramStringARB;
  if (g_str_equal (symbol_name, "glProgramUniform1d")) return glProgramUniform1d;
  if (g_str_equal (symbol_name, "glProgramUniform1dv")) return glProgramUniform1dv;
  if (g_str_equal (symbol_name, "glProgramUniform1f")) return glProgramUniform1f;
  if (g_str_equal (symbol_name, "glProgramUniform1fv")) return glProgramUniform1fv;
  if (g_str_equal (symbol_name, "glProgramUniform1i")) return glProgramUniform1i;
  if (g_str_equal (symbol_name, "glProgramUniform1iv")) return glProgramUniform1iv;
  if (g_str_equal (symbol_name, "glProgramUniform1ui")) return glProgramUniform1ui;
  if (g_str_equal (symbol_name, "glProgramUniform1uiv")) return glProgramUniform1uiv;
  if (g_str_equal (symbol_name, "glProgramUniform2d")) return glProgramUniform2d;
  if (g_str_equal (symbol_name, "glProgramUniform2dv")) return glProgramUniform2dv;
  if (g_str_equal (symbol_name, "glProgramUniform2f")) return glProgramUniform2f;
  if (g_str_equal (symbol_name, "glProgramUniform2fv")) return glProgramUniform2fv;
  if (g_str_equal (symbol_name, "glProgramUniform2i")) return glProgramUniform2i;
  if (g_str_equal (symbol_name, "glProgramUniform2iv")) return glProgramUniform2iv;
  if (g_str_equal (symbol_name, "glProgramUniform2ui")) return glProgramUniform2ui;
  if (g_str_equal (symbol_name, "glProgramUniform2uiv")) return glProgramUniform2uiv;
  if (g_str_equal (symbol_name, "glProgramUniform3d")) return glProgramUniform3d;
  if (g_str_equal (symbol_name, "glProgramUniform3dv")) return glProgramUniform3dv;
  if (g_str_equal (symbol_name, "glProgramUniform3f")) return glProgramUniform3f;
  if (g_str_equal (symbol_name, "glProgramUniform3fv")) return glProgramUniform3fv;
  if (g_str_equal (symbol_name, "glProgramUniform3i")) return glProgramUniform3i;
  if (g_str_equal (symbol_name, "glProgramUniform3iv")) return glProgramUniform3iv;
  if (g_str_equal (symbol_name, "glProgramUniform3ui")) return glProgramUniform3ui;
  if (g_str_equal (symbol_name, "glProgramUniform3uiv")) return glProgramUniform3uiv;
  if (g_str_equal (symbol_name, "glProgramUniform4d")) return glProgramUniform4d;
  if (g_str_equal (symbol_name, "glProgramUniform4dv")) return glProgramUniform4dv;
  if (g_str_equal (symbol_name, "glProgramUniform4f")) return glProgramUniform4f;
  if (g_str_equal (symbol_name, "glProgramUniform4fv")) return glProgramUniform4fv;
  if (g_str_equal (symbol_name, "glProgramUniform4i")) return glProgramUniform4i;
  if (g_str_equal (symbol_name, "glProgramUniform4iv")) return glProgramUniform4iv;
  if (g_str_equal (symbol_name, "glProgramUniform4ui")) return glProgramUniform4ui;
  if (g_str_equal (symbol_name, "glProgramUniform4uiv")) return glProgramUniform4uiv;
  if (g_str_equal (symbol_name, "glProgramUniformHandleui64ARB")) return glProgramUniformHandleui64ARB;
  if (g_str_equal (symbol_name, "glProgramUniformHandleui64vARB")) return glProgramUniformHandleui64vARB;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix2dv")) return glProgramUniformMatrix2dv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix2fv")) return glProgramUniformMatrix2fv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix2x3dv")) return glProgramUniformMatrix2x3dv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix2x3fv")) return glProgramUniformMatrix2x3fv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix2x4dv")) return glProgramUniformMatrix2x4dv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix2x4fv")) return glProgramUniformMatrix2x4fv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix3dv")) return glProgramUniformMatrix3dv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix3fv")) return glProgramUniformMatrix3fv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix3x2dv")) return glProgramUniformMatrix3x2dv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix3x2fv")) return glProgramUniformMatrix3x2fv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix3x4dv")) return glProgramUniformMatrix3x4dv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix3x4fv")) return glProgramUniformMatrix3x4fv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix4dv")) return glProgramUniformMatrix4dv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix4fv")) return glProgramUniformMatrix4fv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix4x2dv")) return glProgramUniformMatrix4x2dv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix4x2fv")) return glProgramUniformMatrix4x2fv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix4x3dv")) return glProgramUniformMatrix4x3dv;
  if (g_str_equal (symbol_name, "glProgramUniformMatrix4x3fv")) return glProgramUniformMatrix4x3fv;
  if (g_str_equal (symbol_name, "glProvokingVertex")) return glProvokingVertex;
  if (g_str_equal (symbol_name, "glPushDebugGroup")) return glPushDebugGroup;
  if (g_str_equal (symbol_name, "glQueryCounter")) return glQueryCounter;
  if (g_str_equal (symbol_name, "glQueryMatrixxOES")) return glQueryMatrixxOES;
  if (g_str_equal (symbol_name, "glRasterPos2xOES")) return glRasterPos2xOES;
  if (g_str_equal (symbol_name, "glRasterPos2xvOES")) return glRasterPos2xvOES;
  if (g_str_equal (symbol_name, "glRasterPos3xOES")) return glRasterPos3xOES;
  if (g_str_equal (symbol_name, "glRasterPos3xvOES")) return glRasterPos3xvOES;
  if (g_str_equal (symbol_name, "glRasterPos4xOES")) return glRasterPos4xOES;
  if (g_str_equal (symbol_name, "glRasterPos4xvOES")) return glRasterPos4xvOES;
  if (g_str_equal (symbol_name, "glReadnPixelsARB")) return glReadnPixelsARB;
  if (g_str_equal (symbol_name, "glRectxOES")) return glRectxOES;
  if (g_str_equal (symbol_name, "glRectxvOES")) return glRectxvOES;
  if (g_str_equal (symbol_name, "glReleaseShaderCompiler")) return glReleaseShaderCompiler;
  if (g_str_equal (symbol_name, "glRenderbufferStorageMultisample")) return glRenderbufferStorageMultisample;
  if (g_str_equal (symbol_name, "glRenderbufferStorage")) return glRenderbufferStorage;
  if (g_str_equal (symbol_name, "glResetHistogram")) return glResetHistogram;
  if (g_str_equal (symbol_name, "glResetMinmax")) return glResetMinmax;
  if (g_str_equal (symbol_name, "glResumeTransformFeedback")) return glResumeTransformFeedback;
  if (g_str_equal (symbol_name, "glRotatexOES")) return glRotatexOES;
  if (g_str_equal (symbol_name, "glSampleCoverageARB")) return glSampleCoverageARB;
  /* if (g_str_equal (symbol_name, "glSampleCoverageOES")) return glSampleCoverageOES; */
  if (g_str_equal (symbol_name, "glSampleCoverage")) return glSampleCoverage;
  if (g_str_equal (symbol_name, "glSampleMaski")) return glSampleMaski;
  if (g_str_equal (symbol_name, "glSamplerParameterf")) return glSamplerParameterf;
  if (g_str_equal (symbol_name, "glSamplerParameterfv")) return glSamplerParameterfv;
  if (g_str_equal (symbol_name, "glSamplerParameterIiv")) return glSamplerParameterIiv;
  if (g_str_equal (symbol_name, "glSamplerParameteri")) return glSamplerParameteri;
  if (g_str_equal (symbol_name, "glSamplerParameterIuiv")) return glSamplerParameterIuiv;
  if (g_str_equal (symbol_name, "glSamplerParameteriv")) return glSamplerParameteriv;
  if (g_str_equal (symbol_name, "glScalexOES")) return glScalexOES;
  if (g_str_equal (symbol_name, "glScissorArrayv")) return glScissorArrayv;
  if (g_str_equal (symbol_name, "glScissorIndexed")) return glScissorIndexed;
  if (g_str_equal (symbol_name, "glScissorIndexedv")) return glScissorIndexedv;
  if (g_str_equal (symbol_name, "glSecondaryColor3b")) return glSecondaryColor3b;
  if (g_str_equal (symbol_name, "glSecondaryColor3bv")) return glSecondaryColor3bv;
  if (g_str_equal (symbol_name, "glSecondaryColor3d")) return glSecondaryColor3d;
  if (g_str_equal (symbol_name, "glSecondaryColor3dv")) return glSecondaryColor3dv;
  if (g_str_equal (symbol_name, "glSecondaryColor3f")) return glSecondaryColor3f;
  if (g_str_equal (symbol_name, "glSecondaryColor3fv")) return glSecondaryColor3fv;
  if (g_str_equal (symbol_name, "glSecondaryColor3i")) return glSecondaryColor3i;
  if (g_str_equal (symbol_name, "glSecondaryColor3iv")) return glSecondaryColor3iv;
  if (g_str_equal (symbol_name, "glSecondaryColor3s")) return glSecondaryColor3s;
  if (g_str_equal (symbol_name, "glSecondaryColor3sv")) return glSecondaryColor3sv;
  if (g_str_equal (symbol_name, "glSecondaryColor3ub")) return glSecondaryColor3ub;
  if (g_str_equal (symbol_name, "glSecondaryColor3ubv")) return glSecondaryColor3ubv;
  if (g_str_equal (symbol_name, "glSecondaryColor3ui")) return glSecondaryColor3ui;
  if (g_str_equal (symbol_name, "glSecondaryColor3uiv")) return glSecondaryColor3uiv;
  if (g_str_equal (symbol_name, "glSecondaryColor3us")) return glSecondaryColor3us;
  if (g_str_equal (symbol_name, "glSecondaryColor3usv")) return glSecondaryColor3usv;
  if (g_str_equal (symbol_name, "glSecondaryColorP3ui")) return glSecondaryColorP3ui;
  if (g_str_equal (symbol_name, "glSecondaryColorP3uiv")) return glSecondaryColorP3uiv;
  if (g_str_equal (symbol_name, "glSecondaryColorPointer")) return glSecondaryColorPointer;
  if (g_str_equal (symbol_name, "glSeparableFilter2D")) return glSeparableFilter2D;
  if (g_str_equal (symbol_name, "glShaderBinary")) return glShaderBinary;
  if (g_str_equal (symbol_name, "glShaderSourceARB")) return glShaderSourceARB;
  if (g_str_equal (symbol_name, "glShaderSource")) return glShaderSource;
  if (g_str_equal (symbol_name, "glShaderStorageBlockBinding")) return glShaderStorageBlockBinding;
  if (g_str_equal (symbol_name, "glStencilFuncSeparate")) return glStencilFuncSeparate;
  if (g_str_equal (symbol_name, "glStencilMaskSeparate")) return glStencilMaskSeparate;
  if (g_str_equal (symbol_name, "glStencilOpSeparate")) return glStencilOpSeparate;
  if (g_str_equal (symbol_name, "glTexBufferARB")) return glTexBufferARB;
  if (g_str_equal (symbol_name, "glTexBufferRange")) return glTexBufferRange;
  if (g_str_equal (symbol_name, "glTexBuffer")) return glTexBuffer;
  if (g_str_equal (symbol_name, "glTexCoord1bOES")) return glTexCoord1bOES;
  if (g_str_equal (symbol_name, "glTexCoord1bvOES")) return glTexCoord1bvOES;
  if (g_str_equal (symbol_name, "glTexCoord1xOES")) return glTexCoord1xOES;
  if (g_str_equal (symbol_name, "glTexCoord1xvOES")) return glTexCoord1xvOES;
  if (g_str_equal (symbol_name, "glTexCoord2bOES")) return glTexCoord2bOES;
  if (g_str_equal (symbol_name, "glTexCoord2bvOES")) return glTexCoord2bvOES;
  if (g_str_equal (symbol_name, "glTexCoord2xOES")) return glTexCoord2xOES;
  if (g_str_equal (symbol_name, "glTexCoord2xvOES")) return glTexCoord2xvOES;
  if (g_str_equal (symbol_name, "glTexCoord3bOES")) return glTexCoord3bOES;
  if (g_str_equal (symbol_name, "glTexCoord3bvOES")) return glTexCoord3bvOES;
  if (g_str_equal (symbol_name, "glTexCoord3xOES")) return glTexCoord3xOES;
  if (g_str_equal (symbol_name, "glTexCoord3xvOES")) return glTexCoord3xvOES;
  if (g_str_equal (symbol_name, "glTexCoord4bOES")) return glTexCoord4bOES;
  if (g_str_equal (symbol_name, "glTexCoord4bvOES")) return glTexCoord4bvOES;
  if (g_str_equal (symbol_name, "glTexCoord4xOES")) return glTexCoord4xOES;
  if (g_str_equal (symbol_name, "glTexCoord4xvOES")) return glTexCoord4xvOES;
  if (g_str_equal (symbol_name, "glTexCoordP1ui")) return glTexCoordP1ui;
  if (g_str_equal (symbol_name, "glTexCoordP1uiv")) return glTexCoordP1uiv;
  if (g_str_equal (symbol_name, "glTexCoordP2ui")) return glTexCoordP2ui;
  if (g_str_equal (symbol_name, "glTexCoordP2uiv")) return glTexCoordP2uiv;
  if (g_str_equal (symbol_name, "glTexCoordP3ui")) return glTexCoordP3ui;
  if (g_str_equal (symbol_name, "glTexCoordP3uiv")) return glTexCoordP3uiv;
  if (g_str_equal (symbol_name, "glTexCoordP4ui")) return glTexCoordP4ui;
  if (g_str_equal (symbol_name, "glTexCoordP4uiv")) return glTexCoordP4uiv;
  if (g_str_equal (symbol_name, "glTexEnvxOES")) return glTexEnvxOES;
  if (g_str_equal (symbol_name, "glTexEnvxvOES")) return glTexEnvxvOES;
  if (g_str_equal (symbol_name, "glTexGenxOES")) return glTexGenxOES;
  if (g_str_equal (symbol_name, "glTexGenxvOES")) return glTexGenxvOES;
  if (g_str_equal (symbol_name, "glTexImage2DMultisample")) return glTexImage2DMultisample;
  if (g_str_equal (symbol_name, "glTexImage3DMultisample")) return glTexImage3DMultisample;
  if (g_str_equal (symbol_name, "glTexImage3D")) return glTexImage3D;
  if (g_str_equal (symbol_name, "glTexPageCommitmentARB")) return glTexPageCommitmentARB;
  if (g_str_equal (symbol_name, "glTexParameterIiv")) return glTexParameterIiv;
  if (g_str_equal (symbol_name, "glTexParameterIuiv")) return glTexParameterIuiv;
  if (g_str_equal (symbol_name, "glTexParameterxOES")) return glTexParameterxOES;
  if (g_str_equal (symbol_name, "glTexParameterxvOES")) return glTexParameterxvOES;
  if (g_str_equal (symbol_name, "glTexStorage1D")) return glTexStorage1D;
  if (g_str_equal (symbol_name, "glTexStorage2DMultisample")) return glTexStorage2DMultisample;
  if (g_str_equal (symbol_name, "glTexStorage2D")) return glTexStorage2D;
  if (g_str_equal (symbol_name, "glTexStorage3DMultisample")) return glTexStorage3DMultisample;
  if (g_str_equal (symbol_name, "glTexStorage3D")) return glTexStorage3D;
  if (g_str_equal (symbol_name, "glTexSubImage3D")) return glTexSubImage3D;
  if (g_str_equal (symbol_name, "glTextureView")) return glTextureView;
  if (g_str_equal (symbol_name, "glTransformFeedbackVaryings")) return glTransformFeedbackVaryings;
  if (g_str_equal (symbol_name, "glTranslatexOES")) return glTranslatexOES;
  if (g_str_equal (symbol_name, "glUniform1d")) return glUniform1d;
  if (g_str_equal (symbol_name, "glUniform1dv")) return glUniform1dv;
  if (g_str_equal (symbol_name, "glUniform1fARB")) return glUniform1fARB;
  if (g_str_equal (symbol_name, "glUniform1f")) return glUniform1f;
  if (g_str_equal (symbol_name, "glUniform1fvARB")) return glUniform1fvARB;
  if (g_str_equal (symbol_name, "glUniform1fv")) return glUniform1fv;
  if (g_str_equal (symbol_name, "glUniform1iARB")) return glUniform1iARB;
  if (g_str_equal (symbol_name, "glUniform1i")) return glUniform1i;
  if (g_str_equal (symbol_name, "glUniform1ivARB")) return glUniform1ivARB;
  if (g_str_equal (symbol_name, "glUniform1iv")) return glUniform1iv;
  if (g_str_equal (symbol_name, "glUniform1ui")) return glUniform1ui;
  if (g_str_equal (symbol_name, "glUniform1uiv")) return glUniform1uiv;
  if (g_str_equal (symbol_name, "glUniform2d")) return glUniform2d;
  if (g_str_equal (symbol_name, "glUniform2dv")) return glUniform2dv;
  if (g_str_equal (symbol_name, "glUniform2fARB")) return glUniform2fARB;
  if (g_str_equal (symbol_name, "glUniform2f")) return glUniform2f;
  if (g_str_equal (symbol_name, "glUniform2fvARB")) return glUniform2fvARB;
  if (g_str_equal (symbol_name, "glUniform2fv")) return glUniform2fv;
  if (g_str_equal (symbol_name, "glUniform2iARB")) return glUniform2iARB;
  if (g_str_equal (symbol_name, "glUniform2i")) return glUniform2i;
  if (g_str_equal (symbol_name, "glUniform2ivARB")) return glUniform2ivARB;
  if (g_str_equal (symbol_name, "glUniform2iv")) return glUniform2iv;
  if (g_str_equal (symbol_name, "glUniform2ui")) return glUniform2ui;
  if (g_str_equal (symbol_name, "glUniform2uiv")) return glUniform2uiv;
  if (g_str_equal (symbol_name, "glUniform3d")) return glUniform3d;
  if (g_str_equal (symbol_name, "glUniform3dv")) return glUniform3dv;
  if (g_str_equal (symbol_name, "glUniform3fARB")) return glUniform3fARB;
  if (g_str_equal (symbol_name, "glUniform3f")) return glUniform3f;
  if (g_str_equal (symbol_name, "glUniform3fvARB")) return glUniform3fvARB;
  if (g_str_equal (symbol_name, "glUniform3fv")) return glUniform3fv;
  if (g_str_equal (symbol_name, "glUniform3iARB")) return glUniform3iARB;
  if (g_str_equal (symbol_name, "glUniform3i")) return glUniform3i;
  if (g_str_equal (symbol_name, "glUniform3ivARB")) return glUniform3ivARB;
  if (g_str_equal (symbol_name, "glUniform3iv")) return glUniform3iv;
  if (g_str_equal (symbol_name, "glUniform3ui")) return glUniform3ui;
  if (g_str_equal (symbol_name, "glUniform3uiv")) return glUniform3uiv;
  if (g_str_equal (symbol_name, "glUniform4d")) return glUniform4d;
  if (g_str_equal (symbol_name, "glUniform4dv")) return glUniform4dv;
  if (g_str_equal (symbol_name, "glUniform4fARB")) return glUniform4fARB;
  if (g_str_equal (symbol_name, "glUniform4f")) return glUniform4f;
  if (g_str_equal (symbol_name, "glUniform4fvARB")) return glUniform4fvARB;
  if (g_str_equal (symbol_name, "glUniform4fv")) return glUniform4fv;
  if (g_str_equal (symbol_name, "glUniform4iARB")) return glUniform4iARB;
  if (g_str_equal (symbol_name, "glUniform4i")) return glUniform4i;
  if (g_str_equal (symbol_name, "glUniform4ivARB")) return glUniform4ivARB;
  if (g_str_equal (symbol_name, "glUniform4iv")) return glUniform4iv;
  if (g_str_equal (symbol_name, "glUniform4ui")) return glUniform4ui;
  if (g_str_equal (symbol_name, "glUniform4uiv")) return glUniform4uiv;
  if (g_str_equal (symbol_name, "glUniformBlockBinding")) return glUniformBlockBinding;
  if (g_str_equal (symbol_name, "glUniformHandleui64ARB")) return glUniformHandleui64ARB;
  if (g_str_equal (symbol_name, "glUniformHandleui64vARB")) return glUniformHandleui64vARB;
  if (g_str_equal (symbol_name, "glUniformMatrix2dv")) return glUniformMatrix2dv;
  if (g_str_equal (symbol_name, "glUniformMatrix2fvARB")) return glUniformMatrix2fvARB;
  if (g_str_equal (symbol_name, "glUniformMatrix2fv")) return glUniformMatrix2fv;
  if (g_str_equal (symbol_name, "glUniformMatrix2x3dv")) return glUniformMatrix2x3dv;
  if (g_str_equal (symbol_name, "glUniformMatrix2x3fv")) return glUniformMatrix2x3fv;
  if (g_str_equal (symbol_name, "glUniformMatrix2x4dv")) return glUniformMatrix2x4dv;
  if (g_str_equal (symbol_name, "glUniformMatrix2x4fv")) return glUniformMatrix2x4fv;
  if (g_str_equal (symbol_name, "glUniformMatrix3dv")) return glUniformMatrix3dv;
  if (g_str_equal (symbol_name, "glUniformMatrix3fvARB")) return glUniformMatrix3fvARB;
  if (g_str_equal (symbol_name, "glUniformMatrix3fv")) return glUniformMatrix3fv;
  if (g_str_equal (symbol_name, "glUniformMatrix3x2dv")) return glUniformMatrix3x2dv;
  if (g_str_equal (symbol_name, "glUniformMatrix3x2fv")) return glUniformMatrix3x2fv;
  if (g_str_equal (symbol_name, "glUniformMatrix3x4dv")) return glUniformMatrix3x4dv;
  if (g_str_equal (symbol_name, "glUniformMatrix3x4fv")) return glUniformMatrix3x4fv;
  if (g_str_equal (symbol_name, "glUniformMatrix4dv")) return glUniformMatrix4dv;
  if (g_str_equal (symbol_name, "glUniformMatrix4fvARB")) return glUniformMatrix4fvARB;
  if (g_str_equal (symbol_name, "glUniformMatrix4fv")) return glUniformMatrix4fv;
  if (g_str_equal (symbol_name, "glUniformMatrix4x2dv")) return glUniformMatrix4x2dv;
  if (g_str_equal (symbol_name, "glUniformMatrix4x2fv")) return glUniformMatrix4x2fv;
  if (g_str_equal (symbol_name, "glUniformMatrix4x3dv")) return glUniformMatrix4x3dv;
  if (g_str_equal (symbol_name, "glUniformMatrix4x3fv")) return glUniformMatrix4x3fv;
  if (g_str_equal (symbol_name, "glUniformSubroutinesuiv")) return glUniformSubroutinesuiv;
  if (g_str_equal (symbol_name, "glUnmapBufferARB")) return glUnmapBufferARB;
  if (g_str_equal (symbol_name, "glUnmapBuffer")) return glUnmapBuffer;
  if (g_str_equal (symbol_name, "glUseProgramObjectARB")) return glUseProgramObjectARB;
  if (g_str_equal (symbol_name, "glUseProgram")) return glUseProgram;
  if (g_str_equal (symbol_name, "glUseProgramStages")) return glUseProgramStages;
  if (g_str_equal (symbol_name, "glValidateProgramARB")) return glValidateProgramARB;
  if (g_str_equal (symbol_name, "glValidateProgramPipeline")) return glValidateProgramPipeline;
  if (g_str_equal (symbol_name, "glValidateProgram")) return glValidateProgram;
  if (g_str_equal (symbol_name, "glVertex2bOES")) return glVertex2bOES;
  if (g_str_equal (symbol_name, "glVertex2bvOES")) return glVertex2bvOES;
  if (g_str_equal (symbol_name, "glVertex2xOES")) return glVertex2xOES;
  if (g_str_equal (symbol_name, "glVertex2xvOES")) return glVertex2xvOES;
  if (g_str_equal (symbol_name, "glVertex3bOES")) return glVertex3bOES;
  if (g_str_equal (symbol_name, "glVertex3bvOES")) return glVertex3bvOES;
  if (g_str_equal (symbol_name, "glVertex3xOES")) return glVertex3xOES;
  if (g_str_equal (symbol_name, "glVertex3xvOES")) return glVertex3xvOES;
  if (g_str_equal (symbol_name, "glVertex4bOES")) return glVertex4bOES;
  if (g_str_equal (symbol_name, "glVertex4bvOES")) return glVertex4bvOES;
  if (g_str_equal (symbol_name, "glVertex4xOES")) return glVertex4xOES;
  if (g_str_equal (symbol_name, "glVertex4xvOES")) return glVertex4xvOES;
  if (g_str_equal (symbol_name, "glVertexAttrib1dARB")) return glVertexAttrib1dARB;
  if (g_str_equal (symbol_name, "glVertexAttrib1d")) return glVertexAttrib1d;
  if (g_str_equal (symbol_name, "glVertexAttrib1dvARB")) return glVertexAttrib1dvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib1dv")) return glVertexAttrib1dv;
  if (g_str_equal (symbol_name, "glVertexAttrib1fARB")) return glVertexAttrib1fARB;
  if (g_str_equal (symbol_name, "glVertexAttrib1f")) return glVertexAttrib1f;
  if (g_str_equal (symbol_name, "glVertexAttrib1fvARB")) return glVertexAttrib1fvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib1fv")) return glVertexAttrib1fv;
  if (g_str_equal (symbol_name, "glVertexAttrib1sARB")) return glVertexAttrib1sARB;
  if (g_str_equal (symbol_name, "glVertexAttrib1s")) return glVertexAttrib1s;
  if (g_str_equal (symbol_name, "glVertexAttrib1svARB")) return glVertexAttrib1svARB;
  if (g_str_equal (symbol_name, "glVertexAttrib1sv")) return glVertexAttrib1sv;
  if (g_str_equal (symbol_name, "glVertexAttrib2dARB")) return glVertexAttrib2dARB;
  if (g_str_equal (symbol_name, "glVertexAttrib2d")) return glVertexAttrib2d;
  if (g_str_equal (symbol_name, "glVertexAttrib2dvARB")) return glVertexAttrib2dvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib2dv")) return glVertexAttrib2dv;
  if (g_str_equal (symbol_name, "glVertexAttrib2fARB")) return glVertexAttrib2fARB;
  if (g_str_equal (symbol_name, "glVertexAttrib2f")) return glVertexAttrib2f;
  if (g_str_equal (symbol_name, "glVertexAttrib2fvARB")) return glVertexAttrib2fvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib2fv")) return glVertexAttrib2fv;
  if (g_str_equal (symbol_name, "glVertexAttrib2sARB")) return glVertexAttrib2sARB;
  if (g_str_equal (symbol_name, "glVertexAttrib2s")) return glVertexAttrib2s;
  if (g_str_equal (symbol_name, "glVertexAttrib2svARB")) return glVertexAttrib2svARB;
  if (g_str_equal (symbol_name, "glVertexAttrib2sv")) return glVertexAttrib2sv;
  if (g_str_equal (symbol_name, "glVertexAttrib3dARB")) return glVertexAttrib3dARB;
  if (g_str_equal (symbol_name, "glVertexAttrib3d")) return glVertexAttrib3d;
  if (g_str_equal (symbol_name, "glVertexAttrib3dvARB")) return glVertexAttrib3dvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib3dv")) return glVertexAttrib3dv;
  if (g_str_equal (symbol_name, "glVertexAttrib3fARB")) return glVertexAttrib3fARB;
  if (g_str_equal (symbol_name, "glVertexAttrib3f")) return glVertexAttrib3f;
  if (g_str_equal (symbol_name, "glVertexAttrib3fvARB")) return glVertexAttrib3fvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib3fv")) return glVertexAttrib3fv;
  if (g_str_equal (symbol_name, "glVertexAttrib3sARB")) return glVertexAttrib3sARB;
  if (g_str_equal (symbol_name, "glVertexAttrib3s")) return glVertexAttrib3s;
  if (g_str_equal (symbol_name, "glVertexAttrib3svARB")) return glVertexAttrib3svARB;
  if (g_str_equal (symbol_name, "glVertexAttrib3sv")) return glVertexAttrib3sv;
  if (g_str_equal (symbol_name, "glVertexAttrib4bvARB")) return glVertexAttrib4bvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4bv")) return glVertexAttrib4bv;
  if (g_str_equal (symbol_name, "glVertexAttrib4dARB")) return glVertexAttrib4dARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4d")) return glVertexAttrib4d;
  if (g_str_equal (symbol_name, "glVertexAttrib4dvARB")) return glVertexAttrib4dvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4dv")) return glVertexAttrib4dv;
  if (g_str_equal (symbol_name, "glVertexAttrib4fARB")) return glVertexAttrib4fARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4f")) return glVertexAttrib4f;
  if (g_str_equal (symbol_name, "glVertexAttrib4fvARB")) return glVertexAttrib4fvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4fv")) return glVertexAttrib4fv;
  if (g_str_equal (symbol_name, "glVertexAttrib4ivARB")) return glVertexAttrib4ivARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4iv")) return glVertexAttrib4iv;
  if (g_str_equal (symbol_name, "glVertexAttrib4NbvARB")) return glVertexAttrib4NbvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4Nbv")) return glVertexAttrib4Nbv;
  if (g_str_equal (symbol_name, "glVertexAttrib4NivARB")) return glVertexAttrib4NivARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4Niv")) return glVertexAttrib4Niv;
  if (g_str_equal (symbol_name, "glVertexAttrib4NsvARB")) return glVertexAttrib4NsvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4Nsv")) return glVertexAttrib4Nsv;
  if (g_str_equal (symbol_name, "glVertexAttrib4NubARB")) return glVertexAttrib4NubARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4Nub")) return glVertexAttrib4Nub;
  if (g_str_equal (symbol_name, "glVertexAttrib4NubvARB")) return glVertexAttrib4NubvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4Nubv")) return glVertexAttrib4Nubv;
  if (g_str_equal (symbol_name, "glVertexAttrib4NuivARB")) return glVertexAttrib4NuivARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4Nuiv")) return glVertexAttrib4Nuiv;
  if (g_str_equal (symbol_name, "glVertexAttrib4NusvARB")) return glVertexAttrib4NusvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4Nusv")) return glVertexAttrib4Nusv;
  if (g_str_equal (symbol_name, "glVertexAttrib4sARB")) return glVertexAttrib4sARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4s")) return glVertexAttrib4s;
  if (g_str_equal (symbol_name, "glVertexAttrib4svARB")) return glVertexAttrib4svARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4sv")) return glVertexAttrib4sv;
  if (g_str_equal (symbol_name, "glVertexAttrib4ubvARB")) return glVertexAttrib4ubvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4ubv")) return glVertexAttrib4ubv;
  if (g_str_equal (symbol_name, "glVertexAttrib4uivARB")) return glVertexAttrib4uivARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4uiv")) return glVertexAttrib4uiv;
  if (g_str_equal (symbol_name, "glVertexAttrib4usvARB")) return glVertexAttrib4usvARB;
  if (g_str_equal (symbol_name, "glVertexAttrib4usv")) return glVertexAttrib4usv;
  if (g_str_equal (symbol_name, "glVertexAttribBinding")) return glVertexAttribBinding;
  if (g_str_equal (symbol_name, "glVertexAttribDivisorARB")) return glVertexAttribDivisorARB;
  if (g_str_equal (symbol_name, "glVertexAttribDivisor")) return glVertexAttribDivisor;
  if (g_str_equal (symbol_name, "glVertexAttribFormat")) return glVertexAttribFormat;
  if (g_str_equal (symbol_name, "glVertexAttribI1i")) return glVertexAttribI1i;
  if (g_str_equal (symbol_name, "glVertexAttribI1iv")) return glVertexAttribI1iv;
  if (g_str_equal (symbol_name, "glVertexAttribI1ui")) return glVertexAttribI1ui;
  if (g_str_equal (symbol_name, "glVertexAttribI1uiv")) return glVertexAttribI1uiv;
  if (g_str_equal (symbol_name, "glVertexAttribI2i")) return glVertexAttribI2i;
  if (g_str_equal (symbol_name, "glVertexAttribI2iv")) return glVertexAttribI2iv;
  if (g_str_equal (symbol_name, "glVertexAttribI2ui")) return glVertexAttribI2ui;
  if (g_str_equal (symbol_name, "glVertexAttribI2uiv")) return glVertexAttribI2uiv;
  if (g_str_equal (symbol_name, "glVertexAttribI3i")) return glVertexAttribI3i;
  if (g_str_equal (symbol_name, "glVertexAttribI3iv")) return glVertexAttribI3iv;
  if (g_str_equal (symbol_name, "glVertexAttribI3ui")) return glVertexAttribI3ui;
  if (g_str_equal (symbol_name, "glVertexAttribI3uiv")) return glVertexAttribI3uiv;
  if (g_str_equal (symbol_name, "glVertexAttribI4bv")) return glVertexAttribI4bv;
  if (g_str_equal (symbol_name, "glVertexAttribI4i")) return glVertexAttribI4i;
  if (g_str_equal (symbol_name, "glVertexAttribI4iv")) return glVertexAttribI4iv;
  if (g_str_equal (symbol_name, "glVertexAttribI4sv")) return glVertexAttribI4sv;
  if (g_str_equal (symbol_name, "glVertexAttribI4ubv")) return glVertexAttribI4ubv;
  if (g_str_equal (symbol_name, "glVertexAttribI4ui")) return glVertexAttribI4ui;
  if (g_str_equal (symbol_name, "glVertexAttribI4uiv")) return glVertexAttribI4uiv;
  if (g_str_equal (symbol_name, "glVertexAttribI4usv")) return glVertexAttribI4usv;
  if (g_str_equal (symbol_name, "glVertexAttribIFormat")) return glVertexAttribIFormat;
  if (g_str_equal (symbol_name, "glVertexAttribIPointer")) return glVertexAttribIPointer;
  if (g_str_equal (symbol_name, "glVertexAttribL1d")) return glVertexAttribL1d;
  if (g_str_equal (symbol_name, "glVertexAttribL1dv")) return glVertexAttribL1dv;
  if (g_str_equal (symbol_name, "glVertexAttribL1ui64ARB")) return glVertexAttribL1ui64ARB;
  if (g_str_equal (symbol_name, "glVertexAttribL1ui64vARB")) return glVertexAttribL1ui64vARB;
  if (g_str_equal (symbol_name, "glVertexAttribL2d")) return glVertexAttribL2d;
  if (g_str_equal (symbol_name, "glVertexAttribL2dv")) return glVertexAttribL2dv;
  if (g_str_equal (symbol_name, "glVertexAttribL3d")) return glVertexAttribL3d;
  if (g_str_equal (symbol_name, "glVertexAttribL3dv")) return glVertexAttribL3dv;
  if (g_str_equal (symbol_name, "glVertexAttribL4d")) return glVertexAttribL4d;
  if (g_str_equal (symbol_name, "glVertexAttribL4dv")) return glVertexAttribL4dv;
  if (g_str_equal (symbol_name, "glVertexAttribLFormat")) return glVertexAttribLFormat;
  if (g_str_equal (symbol_name, "glVertexAttribLPointer")) return glVertexAttribLPointer;
  if (g_str_equal (symbol_name, "glVertexAttribP1ui")) return glVertexAttribP1ui;
  if (g_str_equal (symbol_name, "glVertexAttribP1uiv")) return glVertexAttribP1uiv;
  if (g_str_equal (symbol_name, "glVertexAttribP2ui")) return glVertexAttribP2ui;
  if (g_str_equal (symbol_name, "glVertexAttribP2uiv")) return glVertexAttribP2uiv;
  if (g_str_equal (symbol_name, "glVertexAttribP3ui")) return glVertexAttribP3ui;
  if (g_str_equal (symbol_name, "glVertexAttribP3uiv")) return glVertexAttribP3uiv;
  if (g_str_equal (symbol_name, "glVertexAttribP4ui")) return glVertexAttribP4ui;
  if (g_str_equal (symbol_name, "glVertexAttribP4uiv")) return glVertexAttribP4uiv;
  if (g_str_equal (symbol_name, "glVertexAttribPointerARB")) return glVertexAttribPointerARB;
  if (g_str_equal (symbol_name, "glVertexAttribPointer")) return glVertexAttribPointer;
  if (g_str_equal (symbol_name, "glVertexBindingDivisor")) return glVertexBindingDivisor;
  if (g_str_equal (symbol_name, "glVertexBlendARB")) return glVertexBlendARB;
  if (g_str_equal (symbol_name, "glVertexP2ui")) return glVertexP2ui;
  if (g_str_equal (symbol_name, "glVertexP2uiv")) return glVertexP2uiv;
  if (g_str_equal (symbol_name, "glVertexP3ui")) return glVertexP3ui;
  if (g_str_equal (symbol_name, "glVertexP3uiv")) return glVertexP3uiv;
  if (g_str_equal (symbol_name, "glVertexP4ui")) return glVertexP4ui;
  if (g_str_equal (symbol_name, "glVertexP4uiv")) return glVertexP4uiv;
  if (g_str_equal (symbol_name, "glViewportArrayv")) return glViewportArrayv;
  if (g_str_equal (symbol_name, "glViewportIndexedf")) return glViewportIndexedf;
  if (g_str_equal (symbol_name, "glViewportIndexedfv")) return glViewportIndexedfv;
  if (g_str_equal (symbol_name, "glWaitSync")) return glWaitSync;
  if (g_str_equal (symbol_name, "glWeightbvARB")) return glWeightbvARB;
  if (g_str_equal (symbol_name, "glWeightdvARB")) return glWeightdvARB;
  if (g_str_equal (symbol_name, "glWeightfvARB")) return glWeightfvARB;
  if (g_str_equal (symbol_name, "glWeightivARB")) return glWeightivARB;
  if (g_str_equal (symbol_name, "glWeightPointerARB")) return glWeightPointerARB;
  if (g_str_equal (symbol_name, "glWeightsvARB")) return glWeightsvARB;
  if (g_str_equal (symbol_name, "glWeightubvARB")) return glWeightubvARB;
  if (g_str_equal (symbol_name, "glWeightuivARB")) return glWeightuivARB;
  if (g_str_equal (symbol_name, "glWeightusvARB")) return glWeightusvARB;
  if (g_str_equal (symbol_name, "glWindowPos2dARB")) return glWindowPos2dARB;
  if (g_str_equal (symbol_name, "glWindowPos2d")) return glWindowPos2d;
  if (g_str_equal (symbol_name, "glWindowPos2dvARB")) return glWindowPos2dvARB;
  if (g_str_equal (symbol_name, "glWindowPos2dv")) return glWindowPos2dv;
  if (g_str_equal (symbol_name, "glWindowPos2fARB")) return glWindowPos2fARB;
  if (g_str_equal (symbol_name, "glWindowPos2f")) return glWindowPos2f;
  if (g_str_equal (symbol_name, "glWindowPos2fvARB")) return glWindowPos2fvARB;
  if (g_str_equal (symbol_name, "glWindowPos2fv")) return glWindowPos2fv;
  if (g_str_equal (symbol_name, "glWindowPos2iARB")) return glWindowPos2iARB;
  if (g_str_equal (symbol_name, "glWindowPos2i")) return glWindowPos2i;
  if (g_str_equal (symbol_name, "glWindowPos2ivARB")) return glWindowPos2ivARB;
  if (g_str_equal (symbol_name, "glWindowPos2iv")) return glWindowPos2iv;
  if (g_str_equal (symbol_name, "glWindowPos2sARB")) return glWindowPos2sARB;
  if (g_str_equal (symbol_name, "glWindowPos2s")) return glWindowPos2s;
  if (g_str_equal (symbol_name, "glWindowPos2svARB")) return glWindowPos2svARB;
  if (g_str_equal (symbol_name, "glWindowPos2sv")) return glWindowPos2sv;
  if (g_str_equal (symbol_name, "glWindowPos3dARB")) return glWindowPos3dARB;
  if (g_str_equal (symbol_name, "glWindowPos3d")) return glWindowPos3d;
  if (g_str_equal (symbol_name, "glWindowPos3dvARB")) return glWindowPos3dvARB;
  if (g_str_equal (symbol_name, "glWindowPos3dv")) return glWindowPos3dv;
  if (g_str_equal (symbol_name, "glWindowPos3fARB")) return glWindowPos3fARB;
  if (g_str_equal (symbol_name, "glWindowPos3f")) return glWindowPos3f;
  if (g_str_equal (symbol_name, "glWindowPos3fvARB")) return glWindowPos3fvARB;
  if (g_str_equal (symbol_name, "glWindowPos3fv")) return glWindowPos3fv;
  if (g_str_equal (symbol_name, "glWindowPos3iARB")) return glWindowPos3iARB;
  if (g_str_equal (symbol_name, "glWindowPos3i")) return glWindowPos3i;
  if (g_str_equal (symbol_name, "glWindowPos3ivARB")) return glWindowPos3ivARB;
  if (g_str_equal (symbol_name, "glWindowPos3iv")) return glWindowPos3iv;
  if (g_str_equal (symbol_name, "glWindowPos3sARB")) return glWindowPos3sARB;
  if (g_str_equal (symbol_name, "glWindowPos3s")) return glWindowPos3s;
  if (g_str_equal (symbol_name, "glWindowPos3svARB")) return glWindowPos3svARB;
  if (g_str_equal (symbol_name, "glWindowPos3sv")) return glWindowPos3sv;

  g_critical ("Requested function %s not found.", symbol_name);

  return NULL;
}

/* Environment commands */

static gboolean
get_can_dupe (RetroCore *self,
              gboolean  *can_dupe)
{
  *can_dupe = TRUE;

  return TRUE;
}

static gboolean
get_content_directory (RetroCore    *self,
                       const gchar **content_directory)
{
  *(content_directory) = retro_core_get_content_directory (self);

  return TRUE;
}

static gboolean
get_input_device_capabilities (RetroCore *self,
                               guint64   *capabilities)
{
  *capabilities = retro_core_get_controller_capabilities (self);

  return TRUE;
}

static gboolean
get_language (RetroCore *self,
              unsigned  *language)
{
  static const struct { const gchar *locale; enum RetroLanguage language; } values[] = {
    { "ar", RETRO_LANGUAGE_ARABIC },
    { "de", RETRO_LANGUAGE_GERMAN },
    { "en", RETRO_LANGUAGE_ENGLISH },
    { "eo", RETRO_LANGUAGE_ESPERANTO },
    { "es", RETRO_LANGUAGE_SPANISH },
    { "fr", RETRO_LANGUAGE_FRENCH },
    { "it", RETRO_LANGUAGE_ITALIAN },
    { "jp", RETRO_LANGUAGE_JAPANESE },
    { "ko", RETRO_LANGUAGE_KOREAN },
    { "nl", RETRO_LANGUAGE_DUTCH },
    { "pl", RETRO_LANGUAGE_POLISH },
    { "pt_BR", RETRO_LANGUAGE_PORTUGUESE_BRAZIL },
    { "pt_PT", RETRO_LANGUAGE_PORTUGUESE_PORTUGAL },
    { "ru", RETRO_LANGUAGE_RUSSIAN },
    { "vi", RETRO_LANGUAGE_VIETNAMESE },
    { "zh_CN", RETRO_LANGUAGE_CHINESE_SIMPLIFIED },
    { "zh_HK", RETRO_LANGUAGE_CHINESE_TRADITIONAL },
    { "zh_SG", RETRO_LANGUAGE_CHINESE_SIMPLIFIED },
    { "zh_TW", RETRO_LANGUAGE_CHINESE_TRADITIONAL },
    { "C", RETRO_LANGUAGE_DEFAULT },
  };

  const gchar * const *locales = g_get_language_names ();
  gsize locale_i, language_i = 0;

  for (locale_i = 0; locales[locale_i] != NULL; locale_i++) {
    for (language_i = 0;
         !g_str_equal (values[language_i].locale, "C") &&
         !g_str_equal (locales[locale_i], values[language_i].locale);
         language_i++);
    if (!g_str_equal (values[language_i].locale, "C"))
      break;
  }

  *language = values[language_i].language;

  return TRUE;
}

static gboolean
get_libretro_path (RetroCore    *self,
                   const gchar **libretro_directory)
{
  *(libretro_directory) = retro_core_get_libretro_path (self);

  return TRUE;
}

static gboolean
get_log_callback (RetroCore        *self,
                  RetroLogCallback *cb)
{
  cb->log = on_log;

  return TRUE;
}

static gboolean
get_overscan (RetroCore *self,
              gboolean  *overscan)
{
  *overscan = self->overscan;

  return TRUE;
}

static gboolean
get_rumble_callback (RetroCore           *self,
                     RetroRumbleCallback *cb)
{
  cb->set_rumble_state = rumble_callback_set_rumble_state;

  return TRUE;
}

static gboolean
get_save_directory (RetroCore    *self,
                    const gchar **save_directory)
{
  *(save_directory) = retro_core_get_save_directory (self);

  return TRUE;
}

static gboolean
get_system_directory (RetroCore    *self,
                      const gchar **system_directory)
{
  *(system_directory) = retro_core_get_system_directory (self);

  return TRUE;
}

static gboolean
get_variable (RetroCore     *self,
              RetroVariable *variable)
{
  RetroOption *option;
  const gchar *value;

  if (!retro_core_has_option (self, variable->key))
    return FALSE;

  option = retro_core_get_option (self, variable->key);
  value = retro_option_get_value (option);
  variable->value = value;

  return TRUE;
}

// The data must be bool, not gboolean, the sizes can be different.
static gboolean
get_variable_update (RetroCore *self,
                     bool      *update)
{
  *update = retro_core_get_variable_update (self);

  return TRUE;
}

static gboolean
set_disk_control_interface (RetroCore                *self,
                            RetroDiskControlCallback *callback)
{
  self->disk_control_callback = callback;

  return TRUE;
}

static gboolean
set_geometry (RetroCore         *self,
              RetroGameGeometry *geometry)
{
  retro_core_set_geometry (self, geometry);

  return TRUE;
}

static gboolean
set_hw_render (RetroCore                   *self,
               RetroHardwareRenderCallback *callback)
{
  retro_core_set_hardware_render_callback (self, callback);
  callback->get_current_framebuffer = get_current_framebuffer;
  callback->get_proc_address = get_proc_address;

  return TRUE;
}

static gboolean
set_input_descriptors (RetroCore            *self,
                       RetroInputDescriptor *descriptors)
{
  int length;

  for (length = 0 ; descriptors[length].description ; length++);
  retro_core_set_controller_descriptors (self, descriptors, length);

  return TRUE;
}

static gboolean
set_keyboard_callback (RetroCore             *self,
                       RetroKeyboardCallback *callback)
{
  self->keyboard_callback = *callback;

  return TRUE;
}

static gboolean
set_message (RetroCore          *self,
             const RetroMessage *message)
{
  gboolean result = FALSE;
  g_signal_emit_by_name (self, "message", message->msg, message->frames, &result);

  return result;
}

static gboolean
set_pixel_format (RetroCore              *self,
                  const RetroPixelFormat *pixel_format)
{
  self->pixel_format = *pixel_format;

  return TRUE;
}

static gboolean
set_rotation (RetroCore           *self,
              const RetroRotation *rotation)
{
  self->rotation = *rotation;

  return TRUE;
}

static gboolean
set_support_no_game (RetroCore *self,
                     gboolean  *support_no_game)
{
  retro_core_set_support_no_game (self, *support_no_game);

  return TRUE;
}

static gboolean
set_system_av_info (RetroCore         *self,
                    RetroSystemAvInfo *system_av_info)
{
  retro_core_set_system_av_info (self, system_av_info);

  return TRUE;
}

static gboolean
set_variables (RetroCore     *self,
               RetroVariable *variable_array)
{
  int i;

  for (i = 0 ; variable_array[i].key && variable_array[i].value ; i++)
    retro_core_insert_variable (self, &variable_array[i]);

  g_signal_emit_by_name (self, "options-set", 0);

  return TRUE;
}

static gboolean
shutdown (RetroCore *self)
{
  gboolean result = FALSE;
  g_signal_emit_by_name (self, "shutdown", &result);

  return result;
}

static gboolean
environment_core_command (RetroCore *self,
                          unsigned   cmd,
                          gpointer   data)
{
  if (!self)
    return FALSE;

  switch (cmd) {
  case RETRO_ENVIRONMENT_GET_CAN_DUPE:
    return get_can_dupe (self, (gboolean *) data);

  case RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY:
    return get_content_directory (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES:
    return get_input_device_capabilities (self, (guint64 *) data);

  case RETRO_ENVIRONMENT_GET_LANGUAGE:
    return get_language (self, (unsigned *) data);

  case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
    return get_libretro_path (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
    return get_log_callback (self, (RetroLogCallback *) data);

  case RETRO_ENVIRONMENT_GET_OVERSCAN:
    return get_overscan (self, (gboolean *) data);

  case RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE:
    return get_rumble_callback (self, (RetroRumbleCallback *) data);

  case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
    return get_save_directory (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
    return get_system_directory (self, (const gchar **) data);

  case RETRO_ENVIRONMENT_GET_VARIABLE:
    return get_variable (self, (RetroVariable *) data);

  case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
    return get_variable_update (self, (bool *) data);

  case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
    return set_disk_control_interface (self, (RetroDiskControlCallback *) data);

  case RETRO_ENVIRONMENT_SET_GEOMETRY:
    return set_geometry (self, (RetroGameGeometry *) data);

  case RETRO_ENVIRONMENT_SET_HW_RENDER:
    return set_hw_render (self, (RetroHardwareRenderCallback *) data);

  case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
    return set_input_descriptors (self, (RetroInputDescriptor *) data);

  case RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK:
    return set_keyboard_callback (self, (RetroKeyboardCallback *) data);

  case RETRO_ENVIRONMENT_SET_MESSAGE:
    return set_message (self, (RetroMessage *) data);

  case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
    return set_pixel_format (self, (RetroPixelFormat *) data);

  case RETRO_ENVIRONMENT_SET_ROTATION:
    return set_rotation (self, (RetroRotation *) data);

  case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
    return set_support_no_game (self, (gboolean *) data);

  case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
    return set_system_av_info (self, (RetroSystemAvInfo *) data);

  case RETRO_ENVIRONMENT_SET_VARIABLES:
    return set_variables (self, (RetroVariable *) data);

  case RETRO_ENVIRONMENT_SHUTDOWN:
    return shutdown (self);

  case RETRO_ENVIRONMENT_GET_CAMERA_INTERFACE:
  case RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER:
  case RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE:
  case RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE:
  case RETRO_ENVIRONMENT_GET_PERF_INTERFACE:
  case RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE:
  case RETRO_ENVIRONMENT_GET_USERNAME:
  case RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK:
  case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
  case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK:
  case RETRO_ENVIRONMENT_SET_HW_RENDER_CONTEXT_NEGOTIATION_INTERFACE:
  case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
  case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
  case RETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK:
  case RETRO_ENVIRONMENT_SET_SERIALIZATION_QUIRKS:
  case RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO:
  case RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS:
  default:
    return FALSE;
  }
}

/* Core callbacks */

static gboolean
on_environment_interface (unsigned cmd,
                          gpointer data)
{
  RetroCore *self;

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_assert_not_reached ();

  return environment_core_command (self, cmd, data);
}

static void
on_video_refresh (guint8 *data,
                  guint   width,
                  guint   height,
                  gsize   pitch)
{
  RetroCore *self;
  RetroPixdata pixdata;

  if (data == NULL)
    return;

  /* TODO Should render the frame buffer instead. */
  if (data == RETRO_HW_FRAME_BUFFER_VALID)
    return;

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_return_if_reached ();

  if (retro_core_is_running_ahead (self))
    return;

  retro_pixdata_init (&pixdata,
                      data, self->pixel_format,
                      pitch, width, height,
                      self->aspect_ratio);

  g_signal_emit_by_name (self, "video-output", &pixdata);
}

// TODO This is internal, make it private as soon as possible.
gpointer
retro_core_get_module_video_refresh_cb (RetroCore *self)
{
  return on_video_refresh;
}

static void
on_audio_sample (gint16 left,
                 gint16 right)
{
  RetroCore *self;
  gint16 samples[] = { left, right };

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_return_if_reached ();

  if (retro_core_is_running_ahead (self))
    return;

  if (self->sample_rate <= 0.0)
    return;

  g_signal_emit_by_name (self, "audio_output", samples, 2, self->sample_rate);
}

static gsize
on_audio_sample_batch (gint16 *data,
                       int     frames)
{
  RetroCore *self;

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_return_val_if_reached (0);

  if (retro_core_is_running_ahead (self))
    // FIXME What should be returned?
    return 0;

  if (self->sample_rate <= 0.0)
    return 0;

  g_signal_emit_by_name (self, "audio_output", data, frames * 2, self->sample_rate);

  // FIXME What should be returned?
  return 0;
}

static void
on_input_poll ()
{
  RetroCore *self;

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_return_if_reached ();

  retro_core_poll_controllers (self);
}

static gint16
on_input_state (guint port,
                guint device,
                guint index,
                guint id)
{
  RetroCore *self;
  RetroInput input;

  self = retro_core_get_cb_data ();

  if (self == NULL)
    g_return_val_if_reached (0);

  retro_input_init (&input, device, id, index);

  return retro_core_get_controller_input_state (self, port, &input);
}

// TODO This is internal, make it private as soon as possible.
void
retro_core_set_environment_interface (RetroCore *self)
{
  RetroModule *module;
  RetroCallbackSetter set_environment;

  module = self->module;
  set_environment = retro_module_get_set_environment (module);

  retro_core_push_cb_data (self);
  set_environment (on_environment_interface);
  retro_core_pop_cb_data ();
}

// TODO This is internal, make it private as soon as possible.
void
retro_core_set_callbacks (RetroCore *self)
{
  RetroModule *module;
  RetroCallbackSetter set_video_refresh;
  RetroCallbackSetter set_audio_sample;
  RetroCallbackSetter set_audio_sample_batch;
  RetroCallbackSetter set_input_poll;
  RetroCallbackSetter set_input_state;

  module = self->module;
  set_video_refresh = retro_module_get_set_video_refresh (module);
  set_audio_sample = retro_module_get_set_audio_sample (module);
  set_audio_sample_batch = retro_module_get_set_audio_sample_batch (module);
  set_input_poll = retro_module_get_set_input_poll (module);
  set_input_state = retro_module_get_set_input_state (module);

  retro_core_push_cb_data (self);
  set_video_refresh (on_video_refresh);
  set_audio_sample (on_audio_sample);
  set_audio_sample_batch (on_audio_sample_batch);
  set_input_poll (on_input_poll);
  set_input_state (on_input_state);
  retro_core_pop_cb_data ();
}
