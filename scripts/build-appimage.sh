#!/usr/bin/bash

set -e

# Check if inside of docker container
if [ ! -f /.dockerenv ]; then
  # Check if docker is available
  if ! command -v docker >/dev/null 2>&1; then
    echo "Error: Docker is not available."
    exit 1
  fi
  docker run --rm -u $(id -u) -v "$PWD":/work discord-screenaudio-buildenv bash /work/scripts/build-appimage.sh
  exit 0
fi

tmpdir="$(mktemp -d)"
builddir="$tmpdir/build"
appdir="$tmpdir/AppDir"
export CMAKE_GENERATOR="Ninja"
cmake -B "$builddir" -S "$PWD"
cmake --build "$builddir" --config Release
DESTDIR="$appdir" cmake --install "$builddir" --prefix "/usr"
# Include libnss related files
mkdir -p "$appdir/AppDir/usr/lib/"
cp -rv "/usr/lib/x86_64-linux-gnu/nss" "$appdir/usr/lib/"

VERSION="$(cat version)" linuxdeploy \
  --appdir "$appdir" \
  --icon-file "assets/discord.png" \
  --plugin qt \
  --exclude-library "libpipewire-0.3.so.0" \
  --output appimage
