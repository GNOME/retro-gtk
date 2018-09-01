#!/bin/bash

# A replacement for `ninja dist` until it can exclude `flatpak` and `tools` directories.

PROJECT=retro-gtk
VERSION=$(grep -Pom1 "version:\s+'\K[\w.]+" ../meson.build)

cd ..
git archive --prefix $PROJECT-$VERSION/ -o tools/$PROJECT-$VERSION.tar HEAD .
xz -f tools/$PROJECT-$VERSION.tar
