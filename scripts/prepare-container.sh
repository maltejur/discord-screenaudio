#!/bin/sh

set -e
cd "$(dirname "$0")"

if [ ! -f /.dockerenv ]; then
  # Check if docker is available
  if ! command -v docker >/dev/null 2>&1; then
    echo "Error: Docker is not available."
    exit 1
  fi
  docker build -t discord-screenaudio-buildenv .
  exit 0
fi

echo "-> Installing dependencies with apt..."
export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y curl build-essential qtbase5-dev qtwebengine5-dev qt5-qmake cmake ninja-build pkg-config git libpipewire-0.3-dev file

echo "-> Installing linuxdeploy..."
tmpdir="$(mktemp -d)"
install_appimage() {
  curl -Lo "$tmpdir/$1.AppImage" "$2"
  chmod +x "$tmpdir/$1.AppImage"
  sed '0,/AI\x02/{s|AI\x02|\x00\x00\x00|}' -i -i "$tmpdir/$1.AppImage"
  (cd "$tmpdir" && ./$1.AppImage --appimage-extract)
  mv -v "$tmpdir/squashfs-root" "/opt/$1"
  ln -sv "/opt/$1/AppRun" "/usr/local/bin/$1"
}
install_appimage "linuxdeploy" "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
install_appimage "linuxdeploy-plugin-qt" "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"

echo "-> Cleaning up..."
rm -rf "$tmpdir"
rm /tmp/prepare-container.sh
apt-get clean
