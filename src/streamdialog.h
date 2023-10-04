// SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QWidget>

class StreamDialog : public QDialog {
  Q_OBJECT

public:
  explicit StreamDialog(QWidget *parent = nullptr);

private:
  QComboBox *m_targetComboBox;
  QComboBox *m_resolutionComboBox;
  QComboBox *m_framerateComboBox;
  QGroupBox *m_videoGroupBox;
  QGroupBox *m_audioGroupBox;

Q_SIGNALS:
  void requestedStreamStart(bool video, bool audio, int width, int height,
                            int frameRate, QString target);

public Q_SLOTS:
  void updateTargets();

private Q_SLOTS:
  void startStream();
};
