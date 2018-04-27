retro_reftest_sources = [
  'retro-reftest.c',
  'retro-reftest-file.c',
]

retro_reftest = executable('retro-reftest',
  retro_reftest_sources,
  c_args: retro_gtk_c_args,
  dependencies: retro_gtk_dep,
  include_directories: [ confinc, srcinc ],
  install: get_option('install-tests'),
)
