ent_conf = configuration_data()
ent_conf.set('PACKAGE', 'retro-gtk')
ent_conf.set('PACKAGE_BUGREPORT', 'https://gitlab.gnome.org/GNOME/retro-gtk/issues')
ent_conf.set('PACKAGE_NAME', 'RetroGTK')
ent_conf.set('PACKAGE_TARNAME', 'retro-gtk-' + retro_gtk_version)
ent_conf.set('PACKAGE_URL', 'https://gitlab.gnome.org/GNOME/retro-gtk')
ent_conf.set('PACKAGE_VERSION', retro_gtk_version)
ent_conf.set('PACKAGE_API_VERSION', retro_gtk_api_version)
ent_conf.set('PACKAGE_API_NAME', retro_gtk_module)
configure_file(input: 'gtkdocentities.ent.in', output: 'gtkdocentities.ent', configuration: ent_conf)

