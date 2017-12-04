project('retro-gtk','c',
  version: '0.13.2',
  meson_version: '>= 0.43.0',
)

cc = meson.get_compiler('c')
gnome = import('gnome')

prefix = get_option('prefix')
libdir = join_paths(prefix, get_option('libdir'))
libretrodir = join_paths(libdir, 'libretro')

confinc = include_directories('.')
srcinc = include_directories('retro-gtk')

glib_version = '>= 2.50'
gtk_version = '>= 3.22'

epoxy = dependency ('epoxy')
gio = dependency ('gio-2.0', version: glib_version)
glib = dependency ('glib-2.0', version: glib_version)
gmodule = dependency ('gmodule-2.0', version: glib_version)
gobject = dependency ('gobject-2.0', version: glib_version)
gtk = dependency ('gtk+-3.0', version: gtk_version)
libpulse_simple = dependency ('libpulse-simple')
m = cc.find_library('m', required : false)

config_h = configuration_data()
config_h.set_quoted ('RETRO_PLUGIN_PATH', ':'.join ([libretrodir, libdir]))

configure_file(
  output: 'retro-gtk-config.h',
  configuration: config_h,
)

subdir('retro-gtk')
subdir('demos')
