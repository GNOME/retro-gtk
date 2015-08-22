// This file is part of Retro. License: GPLv3

#include "retro-gobject-internal.h"

guint8* retro_core_get_memory (RetroCore* self, RetroMemoryType id, int* length) {
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (length != NULL, NULL);

	retro_core_push_cb_data (self);

	RetroModule* module = self->module;

	RetroGetMemoryData get_mem_data = retro_module_get_get_memory_data (module);
	RetroGetMemorySize get_mem_size = retro_module_get_get_memory_size (module);

	guint8* data = get_mem_data (id);
	gsize size = get_mem_size (id);

	retro_core_pop_cb_data (self);

	data = g_memdup (data, size);
	*length = (gint) (data ? size : 0);

	return data;
}

void retro_core_set_memory (RetroCore* self, RetroMemoryType id, guint8* data, int length) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (data != NULL);
	g_return_if_fail (length > 0);

	retro_core_push_cb_data (self);

	RetroModule* module = self->module;

	RetroGetMemoryData get_mem_region = retro_module_get_get_memory_data (module);
	RetroGetMemorySize get_mem_region_size = retro_module_get_get_memory_size (module);

	guint8* memory_region = get_mem_region (id);
	gsize memory_region_size = get_mem_region_size (id);

	retro_core_pop_cb_data (self);

	g_return_if_fail (memory_region != NULL);
	g_return_if_fail (memory_region_size == length);

	memcpy (memory_region, data, length);
}
