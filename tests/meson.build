installed_test_bindir = libexecdir / 'installed-tests' / retro_gtk_module
installed_test_datadir = datadir / 'installed-tests' / retro_gtk_module

testlibretrodir = installed_test_datadir / 'libretro'
testexecdir = installed_test_bindir
testdatadir = installed_test_datadir

retro_reftest_sources = [
  'retro-reftest.c',
  'retro-reftest-file.c',
  'retro-test-controller.c',
]

retro_reftest = executable('retro-reftest',
  retro_reftest_sources,
  c_args: retro_gtk_c_args,
  dependencies: retro_gtk_dep,
  install: get_option('install-tests'),
  install_dir: installed_test_bindir,
)

retro_dummy_lib = shared_library(
  'retro-dummy',
  'retro-dummy.c',
  include_directories: [ libretro_inc ],
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
build_conf.set('testlibretrodir', meson.build_root() / 'tests')
build_conf.set('testexecdir', meson.build_root() / 'tests')
build_conf.set('testdatadir', meson.source_root() / 'tests')

test_c_args = [
  '-DRETRO_LOG_DOMAIN="Retro"',
]

tests = [
  ['RetroCore', 'test-core', [], [retro_dummy_lib]],
]

foreach t : tests
  test_display_name = t.get(0)
  test_name = t.get(1)
  test_srcs = ['@0@.c'.format(test_name)] + t.get(2, [])
  test_args = t.get(3)

  test_exe = executable(test_display_name, test_srcs,
    c_args: test_c_args,
    dependencies: retro_gtk_dep,
    install: get_option('install-tests'),
    install_dir: installed_test_bindir,
  )

  test(
    '@0@ test'.format(test_display_name),
    test_exe,
    args: test_args
  )
endforeach

reftests = [
  ['/retro-dummy', 'retro-dummy'],
]

foreach t : reftests
  test_display_name = t.get(0)
  test_name = t.get(1)
  input_name = '@0@.reftest.in'.format(test_name)

  if get_option('install-tests')
    configure_file(
      input: input_name,
      output: '@0@.reftest'.format(test_name),
      configuration: installed_conf,
      install_dir: testdatadir
    )
  endif

  build_test = configure_file(
    input: input_name,
    output: '@0@.reftest.build'.format(test_name),
    configuration: build_conf
  )

  test(
    '@0@ reftest'.format(test_display_name),
    retro_reftest,
    args: build_test,
  )
endforeach
