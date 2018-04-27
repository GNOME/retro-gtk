retro_gtk_libexecdir = join_paths(retro_gtk_prefix, get_option('libexecdir'))
installed_test_bindir = join_paths(retro_gtk_libexecdir, 'installed-tests', retro_gtk_module)

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
  install_dir: installed_test_bindir,
)
