// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * A location interface.
 *
 * The purpose of this interface is to be able to retrieve location-based
 * information from the host device, such as current latitude / longitude.
 *
 * TODO Change visibility once the interface have been tested.
 */
internal interface Location: Object {
	/**
	 * Start location services.
	 *
	 * The device will start listening for changes to the current location at
	 * regular intervals (which are defined with {@link set_interval}).
	 *
	 * @return FIXME
	 */
	public abstract bool start ();

	/**
	 * Stop location services.
	 *
	 * The device will stop listening for changes to the current location.
	 */
	public abstract void stop ();

	/**
	 * Get the position of the current location.
	 *
	 * Will set parameters to 0 if no new location update has happened since the
	 * last time.
	 *
	 * @param lat FIXME
	 * @param lon FIXME
	 * @param horiz_accuracy FIXME
	 * @param vert_accuracy FIXME
	 * @return FIXME
	 */
	public abstract bool get_position (out double lat, out double lon, out double horiz_accuracy, out double vert_accuracy);

	/**
	 * Sets the interval of time and/or distance at which to update/poll
	 * location-based data.
	 *
	 * To ensure compatibility with all location-based implementations, values
	 * for both interval_ms and interval_distance should be provided.
	 *
	 * @param interval_ms the interval expressed in milliseconds
	 * @param interval_distance the distance interval expressed in meters
	 */
	public abstract void set_interval (uint interval_ms, uint interval_distance);

	/**
	 * Callback which signals when the location driver is initialized.
	 *
	 * {@link start} can be called from here.
	 */
	public abstract void initialized ();

	/**
	 * Callback which signals when the location driver is deinitialized.
	 */
	public abstract void deinitialized ();
}

}

