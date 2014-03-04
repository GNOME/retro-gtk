#include <glib.h>

struct retro_variable {
   const char *key;        // Variable to query in RETRO_ENVIRONMENT_GET_VARIABLE.
                           // If NULL, obtains the complete environment string if more complex parsing is necessary.
                           // The environment string is formatted as key-value pairs delimited by semicolons as so:
                           // "key1=value1;key2=value2;..."
   const char *value;      // Value to be obtained. If key does not exist, it is set to NULL.
};

void retro_environment_get_variable (void *data, const char *value) {
	struct retro_variable *variable = (struct retro_variable *) data;
	if (value) variable->value = g_strdup (value);
}
