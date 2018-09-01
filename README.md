# retro-gtk

The GTK+ Libretro frontend framework.

[Libretro](https://www.libretro.com/) is a plugin format design to implement
video game console emulators, video games and similar multimedia software. Such
plugins are called Libretro cores.

retro-gtk is a framework easing the usage of Libretro cores in conjunction with
[GTK+](https://www.gtk.org/).

It encourages the cores to be installed in a well defined centralized place —
namely the `libretro` subdirectory of your `lib` directory — and it recommends
them to come with [Libretro core descriptors]
(https://wiki.gnome.org/Apps/Games/Documentation/LibretroDescriptorSpec).

## Dependencies

retro-gtk depends on the following libraries at compile time and at run time:
- gobject-2.0
- glib-2.0
- gio-2.0
- gmodule-2.0
- gtk+-3.0
- cairo
- libpulse
- libpulse-simple

## Compiling

To configure the compilation, do:
`meson build`

You can specify the installation prefix by doing:
`meson build --prefix /my/prefix`

Then compile:
`ninja -C build`

## Installing

`ninja -C build install`
