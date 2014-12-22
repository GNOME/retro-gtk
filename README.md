# retro-gobject

retro-gobject is a GObject based Libretro wrapper for frontends.

## Dependencies

retro-gobject depends on the following libraries at compile time and at run time:
- gobject-2.0
- glib-2.0 >= 2.32
- gmodule-2.0
- gio-2.0

It is known to work on Fedora 21, but it currently can't compile on Fedora 20.

## Compiling

To configure the compilation, do:
`./autogen.sh`

You can specify the installation prefix by doing:
`./autogen.sh --prefix /my/prefix`

Then compile:
`make`

## Installing

`make install`

## Documentation

[Retro 0.2 Vala documentation](http://kekun.github.io/retro-gobject/doc/retro-gobject-0.2/vala/retro-gobject/index.htm)

You can compile the Vala documentation by doing:
`make -f Makefile.valadoc`

