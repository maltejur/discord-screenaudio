// SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "localserver.h"

bool isProgramRunning(const QString &program_name) {
  QLocalSocket socket;
  socket.connectToServer(program_name);
  if (socket.waitForConnected()) {
    return true; // program is already running
  }
  return false;
}

void showErrorMessage(const char *text) {
  QMessageBox msgBox;

  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText(text);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.setWindowIcon(QIcon(":assets/de.shorsh.discord-screenaudio.png"));

  msgBox.exec();
}
