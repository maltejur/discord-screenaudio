#pragma once
#include "mainwindow.h"

#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>


bool isProgramRunning(const QString &program_name);
void showErrorMessage(const char *text);
