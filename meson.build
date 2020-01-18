project('retro-gtk','c',
  version: '0.18.1',
  meson_version: '>= 0.50.0',
)

retro_gtk_prefix = get_option('prefix')
retro_gtk_datadir = join_paths(retro_gtk_prefix, get_option('datadir'))

cc = meson.get_compiler('c')
gnome = import('gnome')

prefix = get_option('prefix')
libdir = join_paths(prefix, get_option('libdir'))
libretrodir = join_paths(libdir, 'libretro')

confinc = include_directories('.')

retro_gtk_version = meson.project_version()
retro_gtk_api_version = '0.14'
retro_gtk_module = 'retro-gtk-' + retro_gtk_api_version

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
if get_option('build-tests')
  subdir('tests')
endif
if get_option('demos')
  subdir('demos')
endif

summary = [
  '',
  '------',
  'retro-gtk @0@ (@1@)'.format(retro_gtk_version, retro_gtk_api_version),
  '',
  '     Introspection: @0@'.format(get_option('introspection')),
  '       Build tests: @0@'.format(get_option('build-tests')),
  '     Install tests: @0@'.format(get_option('install-tests')),
  '             Demos: @0@'.format(get_option('demos')),
  'Directories:',
  '            prefix: @0@'.format(retro_gtk_prefix),
  '           datadir: @0@'.format(retro_gtk_datadir),
  '------',
  ''
]

message('\n'.join(summary))
