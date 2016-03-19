// This file is part of Retro. License: GPLv3

namespace Retro {

/**
 * TODO Change visibility once the interface have been tested.
 */
internal enum SensorAccelerometer {
	X,
	Y,
	Z
}

/**
 * TODO Change visibility once the interface have been tested.
 */
internal enum SensorAction {
	ACCELEROMETER_ENABLE,
	ACCELEROMETER_DISABLE
}

/**
 * TODO Change visibility once the interface have been tested.
 */
internal interface Sensor: Object {
	public abstract bool set_sensor_state (uint port, SensorAction action, uint rate);
	public abstract float get_sensor_input (uint port, SensorAccelerometer id);
}

}

