ipc_runner_src = gnome.gdbus_codegen(
  'ipc-runner',
  sources: 'org.gnome.Retro.Runner.xml',
  interface_prefix: 'org.gnome.Retro.',
  namespace: 'Ipc'
)

shared_sources = files([
  'retro-controller-codes.c',
  'retro-controller-state.c',
  'retro-controller-type.c',
  'retro-framebuffer.c',
  'retro-input.c',
  'retro-memfd.c',
  'retro-memory-type.c',
  'retro-pixel-format.c',
  'retro-rumble-effect.c',
])

shared_headers = files([
  'retro-controller-codes.h',
  'retro-controller-type.h',
  'retro-input.h',
  'retro-memory-type.h',
  'retro-rumble-effect.h',
])
