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
apt-get install -y curl build-essential qtbase5-dev qtwebengine5-dev qt5-qmake cmake git libpipewire-0.3-dev

echo "-> Installing linuxdeployqt..."
tmpdir="$(mktemp -d)"
curl -Lo "$tmpdir/linuxdeployqt.AppImage" "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod +x "$tmpdir/linuxdeployqt.AppImage"
sed '0,/AI\x02/{s|AI\x02|\x00\x00\x00|}' -i -i "$tmpdir/linuxdeployqt.AppImage"
(cd "$tmpdir" && ./linuxdeployqt.AppImage --appimage-extract)
mv -v "$tmpdir/squashfs-root" "/opt/linuxdeployqt"
ln -sv "/opt/linuxdeployqt/AppRun" "/usr/local/bin/linuxdeployqt"

echo "-> Cleaning up..."
rm -rf "$tmpdir"
rm /tmp/prepare-container.sh
apt-get clean
