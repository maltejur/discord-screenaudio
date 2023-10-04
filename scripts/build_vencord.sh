#!/usr/bin/bash

# SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later

set -e

cd "$(dirname "$0")/../submodules"

echo_status() {
  echo
  echo
  echo "-> $1..."
}

if [ ! -d "Vencord" ]; then
  echo_status "Cloning Vencord"
  git clone https://github.com/Vendicated/Vencord.git
  cd Vencord
else
  echo_status "Fetching Vencord changes"
  cd Vencord
  git fetch
fi

echo_status "Checking out latest commit"
git reset --hard HEAD
git checkout main
git reset --hard origin/main
version="$(git rev-parse --short HEAD) ($(date --iso-8601))"

echo_status "Installing dependencies"
pnpm i

echo_status "Patching Vencord"
cp -v ../../assets/vencord/plugin.js ./src/plugins/discord-screenaudio.js
cp -v ../../assets/vencord/VencordNativeStub.ts ./browser/VencordNativeStub.ts
patch -p1 -i ../../assets/vencord/disable-plugins.patch

echo_status "Building Vencord"
pnpm run buildWeb

echo_status "Copying built file"
cp -v ./dist/browser.js ../../assets/vencord/vencord.js
cp -v ./dist/browser.css ../../assets/vencord/vencord.css

echo_status "Saving version"
echo "$version" >../../assets/vencord/version.txt
