retro_demo_c_args = [
  '-DRETRO_GTK_USE_UNSTABLE_API',
]

executable(
  'retro-demo',
  'retro-demo.c',
  c_args: retro_demo_c_args,
  dependencies: retro_gtk_dep,
  include_directories: [ confinc, srcinc ],
  gui_app: true,
  install: true,
)
