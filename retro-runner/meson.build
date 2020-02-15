retro_runner_sources = [
  shared_sources,

  'ipc-runner-impl.c',
  'retro-runner.c',

  'retro-core.c',
  'retro-environment.c',
  'retro-game-info.c',
  'retro-input-descriptor.c',
  'retro-main-loop-source.c',
  'retro-module.c',
  'retro-pa-player.c',

  ipc_runner_src,
]

retro_runner_deps = [
  gio,
  gio_unix,
  gmodule,
  libpulse_simple,
  m,
  samplerate,
]

retro_runner_c_args = [
  '-DRETRO_GTK_COMPILATION',
  '-DG_LOG_DOMAIN="RetroRunner"',
  '-DRETRO_RUNNER_COMPILATION',
]

executable(
  'retro-runner',
  retro_runner_sources,
  c_args: retro_runner_c_args,
  dependencies: retro_runner_deps,
  include_directories: [ confinc, shared_inc ],
  install_dir: libexecdir,
  install: true
)
