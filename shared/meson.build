ipc_runner_public_src = gnome.gdbus_codegen(
  'ipc-runner-public',
  sources: 'org.gnome.Retro.Runner.xml',
  interface_prefix: 'org.gnome.Retro.',
  namespace: 'Ipc'
)

# Make the generated D-Bus API documentation private.

make_ipc_runner_private = find_program('make-ipc-runner-private.sh')

ipc_runner_src = custom_target('make-ipc-runner-private',
  output : ['ipc-runner.c', 'ipc-runner-private.h'],
  input : ipc_runner_public_src,
  command : [make_ipc_runner_private, '@INPUT@'],
  install : true,
  install_dir : 'subdir')

shared_sources = files([
  'retro-controller-codes.c',
  'retro-controller-state.c',
  'retro-controller-type.c',
  'retro-framebuffer.c',
  'retro-input.c',
  'retro-memfd.c',
  'retro-pixel-format.c',
])

shared_headers = files([
  'retro-controller-codes.h',
  'retro-controller-type.h',
  'retro-input.h',
  'retro-memory-type.h',
  'retro-rumble-effect.h',
])

shared_enum_headers = files([
  'retro-controller-codes.h',
  'retro-controller-type.h',
  'retro-memory-type.h',
  'retro-rumble-effect.h',
])
