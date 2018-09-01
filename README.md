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

## Example

Writing a Libretro frontend with retro-gtk can be as simple as the following
Vala code:

```
int main (string[] args) {
    Gtk.init (ref args);

    var core = new Retro.Core ("/path/to/your/core_libretro.so");
    try {
        core.boot();
    }
    catch (Error e) {
        critical (e.message);

        return 1;
    }

    var view = new Retro.CoreView ();
    view.set_as_default_controller (core);
    view.set_core (core);
    view.show ();

    var loop = new Retro.MainLoop (core);
    loop.start ();

    var win = new Gtk.Window ();
    win.destroy.connect (Gtk.main_quit);
    win.add (view);
    win.present ();

    Gtk.main ();

    return 0;
}
```

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
