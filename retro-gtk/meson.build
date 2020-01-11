retro_gtk_resources = gnome.compile_resources(
  'retro_gtk_resources',
  'retro-gtk.gresource.xml',
  c_name: 'retro_gtk',
  source_dir: '.',
)

retro_gtk_sources = [
  retro_gtk_resources,
  'retro-cairo-display.c',
  'retro-controller.c',
  'retro-controller-codes.c',
  'retro-controller-iterator.c',
  'retro-controller-type.c',
  'retro-core.c',
  'retro-core-descriptor.c',
  'retro-core-view.c',
  'retro-core-view-controller.c',
  'retro-environment.c',
  'retro-game-info.c',
  'retro-gl-display.c',
  'retro-glsl-filter.c',
  'retro-input.c',
  'retro-input-descriptor.c',
  'retro-keyboard.c',
  'retro-key-joypad-mapping.c',
  'retro-log.c',
  'retro-main-loop.c',
  'retro-memory-type.c',
  'retro-module.c',
  'retro-module-iterator.c',
  'retro-module-query.c',
  'retro-option.c',
  'retro-option-iterator.c',
  'retro-pa-player.c',
  'retro-pixbuf.c',
  'retro-pixdata.c',
  'retro-pixel-format.c',
  'retro-rumble-effect.c',
  'retro-video-filter.c',
]

retro_gtk_main_header = 'retro-gtk.h'

retro_gtk_headers = [
  retro_gtk_main_header,
  'retro-controller.h',
  'retro-controller-codes.h',
  'retro-controller-iterator.h',
  'retro-controller-type.h',
  'retro-core.h',
  'retro-core-descriptor.h',
  'retro-core-view.h',
  'retro-input.h',
  'retro-key-joypad-mapping.h',
  'retro-log.h',
  'retro-main-loop.h',
  'retro-memory-type.h',
  'retro-module-iterator.h',
  'retro-module-query.h',
  'retro-option.h',
  'retro-option-iterator.h',
  'retro-pixbuf.h',
  'retro-pixdata.h',
  'retro-rumble-effect.h',
  'retro-video-filter.h',
]

version_split = meson.project_version().split('.')
MAJOR_VERSION = version_split[0]
MINOR_VERSION = version_split[1]
MICRO_VERSION = version_split[2]

version_conf = configuration_data()
version_conf.set('VERSION', meson.project_version())
version_conf.set('MAJOR_VERSION', MAJOR_VERSION)
version_conf.set('MINOR_VERSION', MINOR_VERSION)
version_conf.set('MICRO_VERSION', MICRO_VERSION)

retro_gtk_version = configure_file(
  input: 'retro-gtk-version.h.in',
  output: 'retro-gtk-version.h',
  configuration: version_conf,
  install: true,
  install_dir: join_paths(get_option('includedir'), 'retro-gtk')
)

retro_gtk_c_args = [
  '-DRETRO_GTK_COMPILATION',
  '-DG_LOG_DOMAIN="Retro"',
  '-DRETRO_PLUGIN_PATH="@0@:@1@"'.format(libretrodir, libdir),
]

retro_gtk_deps = [
  epoxy,
  gio,
  glib,
  gmodule,
  gobject,
  gtk,
  libpulse_simple,
]

retro_gtk_lib = shared_library(
  retro_gtk_module,
  retro_gtk_sources,
  c_args: retro_gtk_c_args,
  dependencies: retro_gtk_deps,
  include_directories: confinc,
  install: true,
  soversion: 0,
)

retro_gtk_dep_sources = [
  retro_gtk_version,
]

if get_option('introspection')
  retro_gtk_gir_args = [
     '--quiet',
    '-DRETRO_GTK_USE_UNSTABLE_API',
  ]

  retro_gtk_gir_includes = [
    'Gio-2.0',
    'GLib-2.0',
    'GModule-2.0',
    'GObject-2.0',
    'Gtk-3.0',
  ]

  retro_gtk_gir = gnome.generate_gir(
    retro_gtk_lib,
    extra_args: retro_gtk_gir_args,
    export_packages: retro_gtk_module,
    sources: retro_gtk_headers + retro_gtk_sources,
    nsversion: retro_gtk_api_version,
    namespace: 'Retro',
    identifier_prefix: 'Retro',
    includes: retro_gtk_gir_includes,
    header: retro_gtk_main_header,
    install: true,
    symbol_prefix: 'retro',
  )

  retro_gtk_vapi_packages = [
    'gio-2.0',
    'glib-2.0',
    'gmodule-2.0',
    'gobject-2.0',
    'gtk+-3.0',
    # 'libpulse-simple',
  ]

  gnome.generate_vapi(
    retro_gtk_module,
    install: true,
    packages: retro_gtk_vapi_packages,
    sources: retro_gtk_gir[0],
  )

  retro_gtk_dep_sources += retro_gtk_gir
endif

retro_gtk_inc = include_directories('.')

retro_gtk_dep = declare_dependency(
  link_with: retro_gtk_lib,
  sources: retro_gtk_dep_sources,
  include_directories: [ confinc, retro_gtk_inc ],
  dependencies: retro_gtk_deps,
)

install_headers(retro_gtk_headers, subdir: 'retro-gtk')

pkg = import('pkgconfig')

pkg.generate(
    description: 'A simple GObject game controller library',
      libraries: retro_gtk_lib,
           name: 'retro-gtk',
      namespace: 'Retro',
export_packages: 'retro-gtk',
       filebase: 'retro-gtk-' + retro_gtk_api_version,
        version: meson.project_version(),
        subdirs: 'retro-gtk',
       requires: 'glib-2.0',
    install_dir: join_paths(get_option('libdir'), 'pkgconfig')
)
