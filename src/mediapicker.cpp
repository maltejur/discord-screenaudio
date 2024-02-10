#include "mediapicker.h"

#include <QBoxLayout>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <qicon.h>

MediaPicker::MediaPicker(const QWebEngineDesktopMediaRequest &request,
                         QWidget *parent)
    : QDialog(parent), m_request(request) {
  auto layout = new QVBoxLayout(this);
  layout->setSizeConstraint(QLayout::SetFixedSize);

  auto screensLabel = new QLabel("Screens", this);
  layout->addWidget(screensLabel);

  auto screensView = new QListView(this);
  screensView->setModel(request.screensModel());
  screensView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  layout->addWidget(screensView);

  auto windowsLabel = new QLabel("Windows", this);
  layout->addWidget(windowsLabel);

  auto windowsView = new QListView(this);
  windowsView->setModel(request.windowsModel());
  windowsView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  windowsView->setMaximumWidth(300);
  layout->addWidget(windowsView);

  auto startButton = new QPushButton("Start Stream", this);
  layout->addWidget(startButton);

  setWindowTitle("discord-screenaudio Media Picker");

  connect(screensView, &QListView::clicked,
          [&, windowsView](const QModelIndex &index) {
            m_selectedWindow = false;
            m_selectedIndex = index;
            windowsView->clearSelection();
          });

  connect(windowsView, &QListView::clicked,
          [&, screensView](const QModelIndex &index) {
            m_selectedWindow = true;
            m_selectedIndex = index;
            screensView->clearSelection();
          });

  connect(startButton, &QPushButton::clicked, this, &MediaPicker::startStream);
  connect(screensView, &QListView::doubleClicked, this,
          &MediaPicker::startStream);
  connect(windowsView, &QListView::doubleClicked, this,
          &MediaPicker::startStream);
}

void MediaPicker::startStream() {
  if (m_selectedWindow) {
    m_request.selectWindow(m_selectedIndex);
  } else {
    m_request.selectScreen(m_selectedIndex);
  }
  accept();
}

void MediaPicker::reject() {
  m_request.cancel();
  QDialog::reject();
}
