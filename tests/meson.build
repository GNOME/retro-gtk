retro_gtk_libexecdir = join_paths(retro_gtk_prefix, get_option('libexecdir'))
installed_test_bindir = join_paths(retro_gtk_libexecdir, 'installed-tests', retro_gtk_module)
installed_test_datadir = join_paths(retro_gtk_datadir, 'installed-tests', retro_gtk_module)

testlibretrodir = join_paths(installed_test_datadir, 'libretro')
testexecdir = installed_test_bindir
testdatadir = installed_test_datadir

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

retro_dummy_lib = shared_library(
  'retro-dummy',
  'retro-dummy.c',
  install: get_option('install-tests'),
  install_dir: testlibretrodir,
  soversion: 0,
)

test_data = [
  'retro-dummy.png',
]

if get_option('install-tests')
  install_data(test_data, install_dir: testdatadir)
endif

installed_conf = configuration_data()
installed_conf.set('testlibretrodir', testlibretrodir)
installed_conf.set('testexecdir', testexecdir)
installed_conf.set('testdatadir', testdatadir)

build_conf = configuration_data()
build_conf.set('testlibretrodir', join_paths(meson.build_root(), 'tests'))
build_conf.set('testexecdir', join_paths(meson.build_root(), 'tests'))
build_conf.set('testdatadir', join_paths(meson.source_root(), 'tests'))

tests = [
  ['/retro-dummy', 'retro-dummy'],
]

foreach t : tests
  test_display_name = t.get(0)
  test_name = t.get(1)
  input_name = '@0@.test.in'.format(test_name)

  if get_option('install-tests')
    configure_file(
      input: input_name,
      output: '@0@.test'.format(test_name),
      configuration: installed_conf,
      install_dir: testdatadir
    )
  endif

  build_test = configure_file(
    input: input_name,
    output: '@0@.test.build'.format(test_name),
    configuration: build_conf
  )

  test(
    '@0@ reftest'.format(test_display_name),
    retro_reftest,
    args: build_test,
  )
endforeach
