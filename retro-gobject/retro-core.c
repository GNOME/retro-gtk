// This file is part of Retro. License: GPLv3

#include "retro-gobject-internal.h"

guint8 *retro_core_get_memory (RetroCore *self, RetroMemoryType id, gint *length) {
	RetroGetMemoryData get_mem_data;
	RetroGetMemorySize get_mem_size;
	guint8 *data;
	gsize size;

	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (length != NULL, NULL);

	get_mem_data = retro_module_get_get_memory_data (self->module);
	get_mem_size = retro_module_get_get_memory_size (self->module);

	retro_core_push_cb_data (self);
	data = get_mem_data (id);
	size = get_mem_size (id);
	retro_core_pop_cb_data ();

	data = g_memdup (data, size);
	*length = (gint) (data != NULL ? size : 0);

	return data;
}

void retro_core_set_memory (RetroCore *self, RetroMemoryType id, guint8 *data, gint length) {
	RetroGetMemoryData get_mem_region;
	RetroGetMemorySize get_mem_region_size;
	guint8 *memory_region;
	gsize memory_region_size;

	g_return_if_fail (self != NULL);
	g_return_if_fail (data != NULL);
	g_return_if_fail (length > 0);

	get_mem_region = retro_module_get_get_memory_data (self->module);
	get_mem_region_size = retro_module_get_get_memory_size (self->module);

	retro_core_push_cb_data (self);
	memory_region = get_mem_region (id);
	memory_region_size = get_mem_region_size (id);
	retro_core_pop_cb_data ();

	g_return_if_fail (memory_region != NULL);
	g_return_if_fail (memory_region_size == length);

	memcpy (memory_region, data, length);
}
