// SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDialog>
#include <QWebEngineDesktopMediaRequest>

class MediaPicker : public QDialog {
  Q_OBJECT

public:
  explicit MediaPicker(const QWebEngineDesktopMediaRequest &request,
                       QWidget *parent = nullptr);

private:
  bool m_selectedWindow;
  QModelIndex m_selectedIndex;
  const QWebEngineDesktopMediaRequest &m_request;

private slots:
  void startStream();
  void reject() override;
};
