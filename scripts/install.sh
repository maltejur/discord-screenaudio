#!/usr/bin/bash

export CMAKE_GENERATOR="Ninja"
cmake -B build
cmake --build build --config Release
sudo cmake --install build
