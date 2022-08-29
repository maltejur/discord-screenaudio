#include "mainwindow.h"
#include "virtmic.h"

#ifdef KNOTIFICATIONS
#include <KNotification>
#else
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#endif

#include "c_tools.h"

#include <QApplication>
#include <QBuffer>
#include <QColor>
#include <QComboBox>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QThread>
#include <QUrl>
#include <QWebEngineNotification>
#include <QWebEngineProfile>
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

#ifdef KNOTIFICATIONS
  QWebEngineProfile::defaultProfile()->setNotificationPresenter(
      [&](std::unique_ptr<QWebEngineNotification> notificationInfo) {
        KNotification *notification = new KNotification("discordNotification");
        notification->setTitle(notificationInfo->title());
        notification->setText(notificationInfo->message());
        notification->setPixmap(QPixmap::fromImage(notificationInfo->icon()));
        notification->setDefaultAction("View");
        connect(notification, &KNotification::defaultActivated,
                [&, notificationInfo = std::move(notificationInfo)]() {
                  notificationInfo->click();
                  activateWindow();
                });
        notification->sendEvent();
      });
#else
QWebEngineProfile::defaultProfile()->setNotificationPresenter(
    [&](std::unique_ptr<QWebEngineNotification> notificationInfo) {
      QByteArray title_ba = notificationInfo->title().toLocal8Bit();
      QByteArray message_ba = notificationInfo->message().toLocal8Bit();
      char image_path[1024], command[2048];
      const char *title = title_ba.data();
      const char *message = secureMe(message_ba.data());
      snprintf(image_path, 32+strlen(title), "/tmp/discord-screenaudio-%s.png", title);
      notificationInfo->icon().save(image_path);
      snprintf(command, 70+strlen(image_path)+strlen(title)+strlen(message), "notify-send --icon=\"%s\" --app-name=\"discord-screenaudio\" \"%s\" \"%s\"", image_path, title, message);
      system(command);
    });
#endif

  setCentralWidget(m_webView);
}

void MainWindow::fullScreenRequested(
    QWebEngineFullScreenRequest fullScreenRequest) {
  fullScreenRequest.accept();
  if (fullScreenRequest.toggleOn()) {
    m_wasMaximized = isMaximized();
    showFullScreen();
  } else {
    m_wasMaximized ? showMaximized() : showNormal();
  }
}

void MainWindow::closeEvent(QCloseEvent *event) { QApplication::quit(); }
