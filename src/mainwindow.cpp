#include "mainwindow.h"
#include "virtmic.h"

#include <QApplication>
#include <QColor>
#include <QComboBox>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QThread>
#include <QUrl>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupWebView();
  resize(1000, 700);
  showMaximized();
}

void MainWindow::setupWebView() {
  auto page = new DiscordPage(this);
  connect(page, &QWebEnginePage::fullScreenRequested, this,
          &MainWindow::fullScreenRequested);

  m_webView = new QWebEngineView(this);
  m_webView->setPage(page);

  setCentralWidget(m_webView);
}

void MainWindow::fullScreenRequested(
    QWebEngineFullScreenRequest fullScreenRequest) {
  fullScreenRequest.accept();
  fullScreenRequest.toggleOn() ? showFullScreen() : showNormal();
}
