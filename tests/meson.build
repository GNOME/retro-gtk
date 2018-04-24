retro_reftest_sources = [
  'retro-reftest.c',
  'retro-reftest-file.c',
]

retro_reftest_dep = [
  retro_gtk_dep,
  x11_dep,
  xtst_dep,
]

retro_reftest = executable('retro-reftest',
  retro_reftest_sources,
  c_args: retro_gtk_c_args,
  dependencies: retro_reftest_dep,
  gui_app: true,
  install: true,
)
