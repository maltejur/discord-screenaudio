#!/usr/bin/bash

set -e

# Check if inside of docker container
if [ ! -f /.dockerenv ]; then
  # Check if docker is available
  if ! command -v docker >/dev/null 2>&1; then
    echo "Error: Docker is not available."
    exit 1
  fi
  docker run --rm -v "$PWD":/work discord-screenaudio-buildenv bash /work/scripts/build-appimage.sh
  exit 0
fi

tmpdir="$(mktemp -d)"
builddir="$tmpdir/build"
appdir="$tmpdir/AppDir"
cmake -B "$builddir" -S "$PWD"
cmake --build "$builddir" --config Release
DESTDIR="$appdir" cmake --install "$builddir" --prefix "/usr"

linuxdeployqt "$appdir/usr/share/applications/discord-screenaudio.desktop" -appimage -extra-plugins=iconengines,platformthemes/libqgtk3.so
