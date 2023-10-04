// SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "mainwindow.h"

#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>

bool isProgramRunning(const QString &program_name);
void showErrorMessage(const char *text);
