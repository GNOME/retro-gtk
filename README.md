# retro-gtk

retro-gtk is a toolkit empowering retro-gobject to write awesome Gtk+ 3 based Libretro frontends.

## Dependencies

retro-gtk depends on the following libraries at compile time and at run time:
- gobject-2.0
- glib-2.0
- gtk+-3.0
- cairo
- clutter-gtk-1.0
- libpulse
- jsk-0.1
- retro-gobject-0.3

It is known to work on Fedora 21.

retro-gobject can be found [here](https://github.com/Kekun/retro-gobject).
jsk can be found [here](https://github.com/Kekun/jsk).

## Compiling

If some of these libraries are installed in some unusual repository, such as `/usr/share/local`, remember to set `PKG_CONFIG_PATH` and `LD_LIBRARY_PATH` accordingly:
`export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig`
`export LD_LIBRARY_PATH=/usr/local/lib`


To configure the compilation, do:
`./autogen.sh`

You can specify the installation prefix by doing:
`./autogen.sh --prefix /my/prefix`

Then compile:
`make`

## Installing

`make install`

