// This file is part of retro-gtk. License: GPL-3.0+.

#ifndef RETRO_CORE_DESCRIPTOR_H
#define RETRO_CORE_DESCRIPTOR_H

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define RETRO_TYPE_CORE_DESCRIPTOR (retro_core_descriptor_get_type())

G_DECLARE_FINAL_TYPE (RetroCoreDescriptor, retro_core_descriptor, RETRO, CORE_DESCRIPTOR, GObject)

RetroCoreDescriptor *retro_core_descriptor_new (const gchar  *filename,
                                                GError      **error);
gboolean retro_core_descriptor_has_icon (RetroCoreDescriptor  *self,
                                         GError              **error);
gchar *retro_core_descriptor_get_uri (RetroCoreDescriptor *self);
gchar *retro_core_descriptor_get_id (RetroCoreDescriptor *self);
gboolean retro_core_descriptor_get_is_game (RetroCoreDescriptor  *self,
                                            GError              **error);
gboolean retro_core_descriptor_get_is_emulator (RetroCoreDescriptor  *self,
                                                GError              **error);
char *retro_core_descriptor_get_name (RetroCoreDescriptor  *self,
                                      GError              **error);
GIcon *retro_core_descriptor_get_icon (RetroCoreDescriptor  *self,
                                       GError              **error);
char *retro_core_descriptor_get_module (RetroCoreDescriptor  *self,
                                        GError              **error);
GFile *retro_core_descriptor_get_module_file (RetroCoreDescriptor  *self,
                                              GError              **error);
gboolean retro_core_descriptor_has_platform (RetroCoreDescriptor *self,
                                             const gchar         *platform);
gboolean retro_core_descriptor_has_firmwares (RetroCoreDescriptor  *self,
                                              const gchar          *platform,
                                              GError              **error);
gboolean retro_core_descriptor_has_firmware_md5 (RetroCoreDescriptor  *self,
                                                 const gchar          *firmware,
                                                 GError              **error);
gboolean retro_core_descriptor_has_firmware_sha512 (RetroCoreDescriptor  *self,
                                                    const gchar          *firmware,
                                                    GError              **error);
gchar **retro_core_descriptor_get_mime_type (RetroCoreDescriptor  *self,
                                             const gchar          *platform,
                                             gsize                *length,
                                             GError              **error);
gchar **retro_core_descriptor_get_firmwares (RetroCoreDescriptor  *self,
                                             const gchar          *platform,
                                             gsize                *length,
                                             GError              **error);
gchar *retro_core_descriptor_get_firmware_path (RetroCoreDescriptor  *self,
                                                const gchar          *firmware,
                                                GError              **error);
gchar * retro_core_descriptor_get_firmware_md5 (RetroCoreDescriptor  *self,
                                                const gchar          *firmware,
                                                GError              **error);
gchar * retro_core_descriptor_get_firmware_sha512 (RetroCoreDescriptor  *self,
                                                   const gchar          *firmware,
                                                   GError              **error);
gboolean retro_core_descriptor_get_is_firmware_mandatory (RetroCoreDescriptor  *self,
                                                          const gchar          *firmware,
                                                          GError              **error);
gboolean retro_core_descriptor_get_platform_supports_mime_types (RetroCoreDescriptor  *self,
                                                                 const gchar          *platform,
                                                                 const gchar * const  *mime_types,
                                                                 GError              **error);

G_END_DECLS

#endif /* RETRO_CORE_DESCRIPTOR_H */
