#include "streamdialog.h"
#include "virtmic.h"

#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

StreamDialog::StreamDialog() : QWidget() {
  setAttribute(Qt::WA_QuitOnClose, false);

  auto layout = new QVBoxLayout(this);
  layout->setSizeConstraint(QLayout::SetFixedSize);

  auto targetLabel = new QLabel(this);
  targetLabel->setText("Which app do you want to stream sound from?");
  layout->addWidget(targetLabel);

  m_targetComboBox = new QComboBox(this);
  updateTargets();
  layout->addWidget(m_targetComboBox);

  auto qualityLabel = new QLabel(this);
  qualityLabel->setText("Stream Quality");
  layout->addWidget(qualityLabel);

  auto qualityHBox = new QHBoxLayout(this);
  layout->addLayout(qualityHBox);

  m_qualityResolutionComboBox = new QComboBox(this);
  m_qualityResolutionComboBox->addItem("2160p", "3840x2160");
  m_qualityResolutionComboBox->addItem("1440p", "2560x1440");
  m_qualityResolutionComboBox->addItem("1080p", "1920x1080");
  m_qualityResolutionComboBox->addItem("720p", "1280x720");
  m_qualityResolutionComboBox->addItem("480p", "854x480");
  m_qualityResolutionComboBox->addItem("360p", "640x360");
  m_qualityResolutionComboBox->addItem("240p", "426x240");
  m_qualityResolutionComboBox->setCurrentText("720p");
  qualityHBox->addWidget(m_qualityResolutionComboBox);

  m_qualityFPSComboBox = new QComboBox(this);
  m_qualityFPSComboBox->addItem("144 FPS", 144);
  m_qualityFPSComboBox->addItem("60 FPS", 60);
  m_qualityFPSComboBox->addItem("30 FPS", 30);
  m_qualityFPSComboBox->addItem("15 FPS", 15);
  m_qualityFPSComboBox->addItem("5 FPS", 5);
  m_qualityFPSComboBox->setCurrentText("30 FPS");
  qualityHBox->addWidget(m_qualityFPSComboBox);

  auto button = new QPushButton(this);
  button->setText("Start Stream");
  connect(button, &QPushButton::clicked, this, &StreamDialog::startStream);
  layout->addWidget(button, Qt::AlignRight | Qt::AlignBottom);

  setLayout(layout);

  setWindowTitle("discord-screenaudio Stream Dialog");
}

void StreamDialog::startStream() {
  auto resolution =
      m_qualityResolutionComboBox->currentData().toString().split('x');
  emit requestedStreamStart(m_targetComboBox->currentText(),
                            resolution[0].toUInt(), resolution[1].toUInt(),
                            m_qualityFPSComboBox->currentData().toUInt());
  setHidden(true);
}

void StreamDialog::updateTargets() {
  m_targetComboBox->clear();
  m_targetComboBox->addItem("[None]");
  m_targetComboBox->addItem("[All Desktop Audio]");
  for (auto target : Virtmic::getTargets()) {
    m_targetComboBox->addItem(target);
  }
  m_targetComboBox->setCurrentText("[All Desktop Audio]");
}
