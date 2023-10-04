#!/usr/bin/bash

# SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later

export CMAKE_GENERATOR="Ninja"
cmake -B build
cmake --build build --config Release
sudo cmake --install build
