#!/usr/bin/bash
set -e

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

echo_status "Checking out latest commit"
git reset --hard HEAD
git checkout main

echo_status "Installing dependencies"
pnpm i -D @vercel/ncc

echo_status "Patching arRPC"
sed -i 's/"type": "module",//' package.json

echo_status "Building arRPC"
pnpm exec ncc build -m src/index.js

echo_status "Copying built file"
cp -v ./dist/index.js ../../assets/arrpc.js
