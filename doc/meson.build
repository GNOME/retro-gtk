subdir('xml')

glib_docpath = glib.get_pkgconfig_variable('prefix') / 'share' / 'gtk-doc' / 'html'
gtk_docpath = gtk.get_pkgconfig_variable('prefix') / 'share' / 'gtk-doc' / 'html'
docpath = get_option('datadir') / 'gtk-doc' / 'html'

private_headers = [
  'ipc-runner-private.h',
  'retro-cairo-display-private.h',
  'retro-controller-codes-private.h',
  'retro-controller-iterator-private.h',
  'retro-controller-state-private.h',
  'retro-core-view-controller-private.h',
  'retro-debug-private.h',
  'retro-framebuffer-private.h',
  'retro-gl-display-private.h',
  'retro-glsl-filter-private.h',
  'retro-glsl-shader-private.h',
  'retro-input-private.h',
  'retro-keyboard-key-private.h',
  'retro-keyboard-private.h',
  'retro-memfd-private.h',
  'retro-option-iterator-private.h',
  'retro-option-private.h',
  'retro-pixdata-private.h',
  'retro-pixel-format-private.h',
  'retro-runner-process-private.h',
]

content_files = [
  'libretro-core-descriptor.xml',
  'reference-test-case.xml',
]

gnome.gtkdoc(
  'retro-gtk',
  main_xml: 'retro-gtk-doc.xml',
  src_dir: [
    meson.source_root() / 'retro-gtk',
    meson.build_root()  / 'retro-gtk',
    meson.source_root() / 'shared',
    meson.build_root()  / 'shared',
  ],
  ignore_headers: private_headers,
  dependencies: retro_gtk_dep,
  gobject_typesfile: 'retro-gtk.types',
  scan_args: [
    '--rebuild-types',
  ],
  mkdb_args: [
    '--default-includes=retro-gtk/retro-gtk.h',
  ],
  fixxref_args: [
    '--html-dir=@0@'.format(docpath),
    '--extra-dir=@0@'.format(glib_docpath / 'glib'),
    '--extra-dir=@0@'.format(glib_docpath / 'gobject'),
    '--extra-dir=@0@'.format(glib_docpath / 'gio'),
    '--extra-dir=@0@'.format(glib_docpath / 'gi'),
    '--extra-dir=@0@'.format(gtk_docpath / 'gtk3'),
    '--extra-dir=@0@'.format(gtk_docpath / 'gdk-pixbuf'),
  ],
  install_dir: 'retro-gtk',
  content_files: content_files,
  install: get_option('install-doc'),
)
