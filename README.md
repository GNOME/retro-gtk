# retro-gtk

retro-gtk is a toolkit to write  GTK+ 3 based Libretro frontends.

## Dependencies

retro-gtk depends on the following libraries at compile time and at run time:
- gobject-2.0
- glib-2.0
- gio-2.0
- gmodule-2.0
- gtk+-3.0
- cairo
- libpulse

## Compiling

To configure the compilation, do:
`./autogen.sh`

You can specify the installation prefix by doing:
`./autogen.sh --prefix /my/prefix`

Then compile:
`./configure`
`make`

## Installing

`make install`
