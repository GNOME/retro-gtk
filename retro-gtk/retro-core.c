// This file is part of Retro. License: GPLv3

#include "retro-gtk-internal.h"

gboolean retro_core_supports_serialization (RetroCore *self) {
	RetroSerializeSize serialize_size = NULL;
	gsize size;

	g_return_val_if_fail (self != NULL, FALSE);

	retro_core_push_cb_data (self);
	serialize_size = retro_module_get_serialize_size (self->module);
	size = serialize_size ();
	retro_core_pop_cb_data ();

	return size > 0;
}

guint8 *retro_core_serialize_state (RetroCore *self, gsize *length, GError **error) {
	RetroSerializeSize serialize_size = NULL;
	RetroSerialize serialize = NULL;
	guint8 *data;
	gsize size;
	gboolean success;

	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (length != NULL, NULL);

	serialize_size = retro_module_get_serialize_size (self->module);

	retro_core_push_cb_data (self);
	size = serialize_size ();
	retro_core_pop_cb_data ();

	if (size <= 0) {
		g_set_error (error,
		             RETRO_CORE_ERROR,
		             RETRO_CORE_ERROR_SERIALIZATION_NOT_SUPPORTED,
		             "Couldn't serialize the internal state: serialization not supported.");

		return NULL;
	}

	serialize = retro_module_get_serialize (self->module);
	data = g_new0 (guint8, size);

	retro_core_push_cb_data (self);
	success = serialize (data, size);
	retro_core_pop_cb_data ();

	if (!success) {
		g_set_error (error,
		             RETRO_CORE_ERROR,
		             RETRO_CORE_ERROR_COULDNT_SERIALIZE,
		             "Couldn't serialize the internal state: serialization failed.");
		g_free (data);

		return NULL;
	}

	*length = size;

	return data;
}

void retro_core_deserialize_state (RetroCore *self, guint8 *data, gsize length, GError **error) {
	RetroSerializeSize serialize_size = NULL;
	RetroUnserialize unserialize = NULL;
	gsize size;
	gboolean success;

	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (data != NULL, NULL);

	serialize_size = retro_module_get_serialize_size (self->module);

	retro_core_push_cb_data (self);
	size = serialize_size ();
	retro_core_pop_cb_data ();

	if (size <= 0) {
		g_set_error (error,
		             RETRO_CORE_ERROR,
		             RETRO_CORE_ERROR_SERIALIZATION_NOT_SUPPORTED,
		             "Couldn't deserialize the internal state: serialization not supported.");

		return NULL;
	}

	if (length > size) {
		g_set_error (error,
		             RETRO_CORE_ERROR,
		             RETRO_CORE_ERROR_COULDNT_DESERIALIZE,
		             "Couldn't deserialize the internal state: expected at most %llu bytes, got %llu.", size, length);

		return NULL;
	}

	unserialize = retro_module_get_unserialize (self->module);

	retro_core_push_cb_data (self);
	success = unserialize (data, length);
	retro_core_pop_cb_data ();

	if (!success) {
		g_set_error (error,
		             RETRO_CORE_ERROR,
		             RETRO_CORE_ERROR_COULDNT_DESERIALIZE,
		             "Couldn't deserialize the internal state: deserialization failed.");

		return NULL;
	}
}

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
