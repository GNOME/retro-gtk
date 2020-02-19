#!/bin/bash

MANIFEST_PATH="flatpak/org.gnome.Retro.UnitTests.json"
FLATPAK_MODULE="retro-gtk"
FLATPAK_BUILD_DIR="unit-tests/unit-tests"
MESON_ARGS="--libdir=lib -Dinstall-tests=true"

rm -rf ${FLATPAK_BUILD_DIR}
flatpak-builder --stop-at=${FLATPAK_MODULE} ${FLATPAK_BUILD_DIR} ${MANIFEST_PATH}
flatpak build ${FLATPAK_BUILD_DIR} meson --prefix=/app ${MESON_ARGS} unit-tests/build
flatpak build ${FLATPAK_BUILD_DIR} ninja -C unit-tests/build install
flatpak-builder --finish-only --repo=unit-tests/repo ${FLATPAK_BUILD_DIR} ${MANIFEST_PATH}

if [[ -z "${DISPLAY}" ]]; then
    xvfb-run -a -s "-screen 0 1024x768x24" flatpak-builder --run ${FLATPAK_BUILD_DIR} ${MANIFEST_PATH} retro-unit-tests
else
    flatpak-builder --run ${FLATPAK_BUILD_DIR} ${MANIFEST_PATH} retro-unit-tests
fi
