retro_runner_enum_headers = [
  shared_enum_headers,
]

retro_runner_enums = gnome.mkenums('retro-runner-enums',
  h_template: 'retro-runner-enums.h.in',
  c_template: 'retro-runner-enums.c.in',
  sources: retro_runner_enum_headers,
  install_header: false,
)

retro_runner_generated_sources = [
  retro_runner_enums[0],
]

retro_runner_sources = [
  shared_sources,
  retro_runner_generated_sources,

  'ipc-runner-impl.c',
  'retro-runner.c',

  'retro-core.c',
  'retro-environment.c',
  'retro-game-info.c',
  'retro-gl-renderer.c',
  'retro-input-descriptor.c',
  'retro-main-loop-source.c',
  'retro-module.c',
  'retro-pa-player.c',
  'retro-renderer.c',

  ipc_runner_src,
]

retro_runner_deps = [
  epoxy,
  gio,
  gio_unix,
  gmodule,
  m,
  samplerate,
]

retro_runner_c_args = [
  '-DRETRO_GTK_COMPILATION',
  '-DG_LOG_DOMAIN="RetroRunner"',
  '-DRETRO_RUNNER_COMPILATION',
]

if libpulse_simple.found()
  retro_runner_c_args += '-DPULSEAUDIO_ENABLED'
  retro_runner_deps += libpulse_simple
endif

executable(
  'retro-runner',
  retro_runner_sources,
  c_args: retro_runner_c_args,
  dependencies: retro_runner_deps,
  include_directories: [ confinc, shared_inc ],
  install_dir: libexecdir,
  install: true
)
