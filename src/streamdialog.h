#pragma once

#include <QComboBox>
#include <QDialog>
#include <QWidget>

class StreamDialog : public QWidget {
  Q_OBJECT

public:
  explicit StreamDialog();

private:
  QComboBox *m_targetComboBox;
  QComboBox *m_qualityResolutionComboBox;
  QComboBox *m_qualityFPSComboBox;

Q_SIGNALS:
  void requestedStreamStart(QString target, uint width, uint height,
                            uint frameRate);

private Q_SLOTS:
  void startStream();
};
