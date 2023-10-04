#!/usr/bin/bash

# SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later

set -e

checkout="89f4da610ccfac93f461826a446a17cd3b23953d"
version="3.2.0"

cd "$(dirname "$0")/../submodules"

echo_status() {
  echo
  echo
  echo "-> $1..."
}

if [ ! -d "arrpc" ]; then
  echo_status "Cloning arRPC"
  git clone https://github.com/OpenAsar/arrpc.git
  cd arrpc
else
  echo_status "Fetching arRPC changes"
  cd arrpc
  git fetch
fi

echo_status "Checking out commit"
git reset --hard $checkout
git checkout main

echo_status "Installing dependencies"
pnpm i -D @vercel/ncc

echo_status "Patching arRPC"
sed -i 's/"type": "module",//' package.json

echo_status "Building arRPC"
pnpm exec ncc build -m src/index.js

echo_status "Copying built file"
cp -v ./dist/index.js ../../assets/arrpc/arrpc.js

echo_status "Saving version"
echo "$version" >../../assets/arrpc/version.txt
