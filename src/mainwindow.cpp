#include "mainwindow.h"
#include "virtmic.h"

#ifdef KNOTIFICATIONS
#include <KNotification>
#else
#include <QProcess>
#endif

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

MainWindow *MainWindow::m_instance = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  assert(MainWindow::m_instance == nullptr);
  MainWindow::m_instance = this;
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

  QWebEngineProfile::defaultProfile()->setNotificationPresenter(
      [&](std::unique_ptr<QWebEngineNotification> notificationInfo) {
#ifdef KNOTIFICATIONS
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
#else
        auto title = notificationInfo->title();
        auto message = notificationInfo->message();
        auto image_path = QString("/tmp/discord-screenaudio-%1.png").arg(title);
        notificationInfo->icon().save(image_path);
        QProcess::execute("notify-send",
                          {"--icon", image_path, "--app-name",
                           "discord-screenaudio", title, message});
#endif
      });

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

MainWindow *MainWindow::instance() { return m_instance; }
