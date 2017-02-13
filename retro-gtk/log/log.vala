// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * The level of importance of a message.
 */
public enum LogLevel {
	DEBUG = 0,
	INFO,
	WARN,
	ERROR
}

/**
 * An interface for logging.
 */
public interface Log: Object {
	/**
	 * Logs a message.
	 *
	 * @param level the importance of the message
	 * @param message the message to log
	 * @return true if the message have been logged, false otherwise
	 */
	public abstract bool log (LogLevel level, string message);
}

}

