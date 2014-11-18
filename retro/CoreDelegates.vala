/* Retro  GObject libretro wrapper.
 * Copyright (C) 2014  Adrien Plazas
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

namespace Retro {

/**
 * Render a frame.
 * 
 * Pixel format is 15-bit 0RGB1555 native endian unless changed
 * (see {@link Environment.pixel_format}).
 * 
 * For performance reasons, it is highly recommended to have a frame that is
 * packed in memory, i.e. pitch == width * byte_per_pixel.
 * Certain graphic APIs, such as OpenGL ES, do not like textures that are
 * not packed in memory.
 * 
 * @param data the frame data
 * @param width width of the frame
 * @param height height of the frame
 * @param pitch length in bytes between two lines in buffer
 */
public delegate void VideoRefresh ([CCode (array_length_type = "gsize")] uint8[] data, uint width, uint height, size_t pitch);

/**
 * Renders a single audio frame.
 * 
 * Should only be used if implementation generates a single sample at a
 * time. Format is signed 16-bit native endian.
 * 
 * @param left the left channel of the audio frame
 * @param right the right channel of the audio frame
 */
public delegate void AudioSample (int16 left, int16 right);

/**
 * Renders multiple audio frames in one go.
 * 
 * One frame is defined as a sample of left and right channels, interleaved.
 * I.e. int16[] buf = { l, r, l, r }; would be 2 frames.
 * 
 * Only one of the audio callbacks must ever be used.
 * 
 * @return the number of audio frames read
 * @param data the audio sample batch
 * @param frames the number of frames in the batch
 */
public delegate size_t AudioSampleBatch ([CCode (array_length_type = "gsize")] int16[] data, size_t frames);

/**
 * Polls input.
 */
public delegate void InputPoll ();

/**
 * Queries for input for player 'port'.
 * 
 * Device will be masked with {@link DeviceType.TYPE_MASK}.
 * Specialization of devices such as
 * {@link DeviceType.JOYPAD_MULTITAP} that have been set with
 * {@link set_controller_port_device} will still use the
 * higher level {@link DeviceType.JOYPAD} to request input.
 * 
 * @return the state of the input
 * @param port the port number
 * @param device the type of device
 * @param index the index, its definition changes with the device
 * @param id the id, its definition changes with the device
 */
public delegate int16 InputState (uint port, DeviceType device, uint index, uint id);

// Types of the module's functions

[CCode (has_target = false)]
private delegate void CallbackSetter (void *cb);

[CCode (has_target = false)]
private delegate void Init ();
[CCode (has_target = false)]
private delegate void Deinit ();

[CCode (has_target = false)]
private delegate uint ApiVersion ();

[CCode (has_target = false)]
private delegate void GetSystemInfo (out unowned SystemInfo info);
[CCode (has_target = false)]
private delegate void GetSystemAvInfo (out unowned SystemAvInfo info);

[CCode (has_target = false)]
private delegate void SetControllerPortDevice (uint port, DeviceType device);

[CCode (has_target = false)]
private delegate void Reset ();
[CCode (has_target = false)]
private delegate void Run ();

[CCode (has_target = false)]
private delegate size_t SerializeSize ();
[CCode (has_target = false)]
private delegate bool Serialize ([CCode (array_length_type = "gsize")] out uint8[] data);
[CCode (has_target = false)]
private delegate bool Unserialize ([CCode (array_length_type = "gsize")] uint8[] data);

[CCode (has_target = false)]
private delegate void CheatReset ();
[CCode (has_target = false)]
private delegate void CheatSet (uint index, bool enabled, string code);

[CCode (has_target = false)]
private delegate bool LoadGame (GameInfo game);
[CCode (has_target = false)]
private delegate bool LoadGameSpecial (GameType game_type, [CCode (array_length_type = "gsize")] GameInfo[] info);
[CCode (has_target = false)]
private delegate void UnloadGame ();

[CCode (has_target = false)]
private delegate Region GetRegion ();

[CCode (has_target = false)]
private delegate void *GetMemoryData (MemoryType id);
[CCode (has_target = false)]
private delegate size_t GetMemorySize (MemoryType id);

}
