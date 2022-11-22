#pragma once

#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QWidget>

class StreamDialog : public QWidget {
  Q_OBJECT

public:
  explicit StreamDialog();

private:
  QComboBox *m_targetComboBox;
  QComboBox *m_resolutionComboBox;
  QComboBox *m_framerateComboBox;
  QGroupBox *m_videoGroupBox;
  QGroupBox *m_audioGroupBox;

Q_SIGNALS:
  void requestedStreamStart(bool video, bool audio, uint width, uint height,
                            uint frameRate, QString target);

public Q_SLOTS:
  void updateTargets();

private Q_SLOTS:
  void startStream();
};
