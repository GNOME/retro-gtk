// This file is part of retro-gtk. License: GPLv3

namespace Retro {

/**
 * Sends log messages from a {@link Core} to a FileStream.
 */
public class FileStreamLog: Object, Retro.Log {
	private const string default_color = "\033[39m";

	/**
	 * The FileStream to log the core's messages to.
	 */
	public unowned FileStream stream { get; construct set; }

	/**
	 * Creates a FileStreamLog.
	 *
	 * @param stream the FileStream to log messages to
	 */
	public FileStreamLog (FileStream stream = GLib.stderr) {
		Object (stream: stream);
	}

	/**
	 * Logs a message to the {@link stream}.
	 *
	 * @param level the importance of the message
	 * @param message the message to log
	 * @return true if the message have been logged, false otherwise
	 */
	public bool log (LogLevel level, string message) {
		stream.printf ("%s%s%s", get_color (level), message, default_color);
		return true;
	}

	private string get_color (LogLevel level) {
		switch (level) {
			case LogLevel.DEBUG:
				// Green
				return "\033[32m";
			case LogLevel.WARN:
				// Yellow
				return "\033[33m";
			case LogLevel.ERROR:
				// Red
				return "\033[31m";
			default:
				return default_color;
		}
	}
}

}

