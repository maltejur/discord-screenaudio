#include "mainwindow.h"
#include "virtmic.h"

#ifdef KNOTIFICATIONS
#include <KNotification>
#endif

#include <QApplication>
#include <QColor>
#include <QComboBox>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QThread>
#include <QTimer>
#include <QUrl>
#include <QWebEngineNotification>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>
#include <QWidget>

MainWindow *MainWindow::m_instance = nullptr;

MainWindow::MainWindow(bool useNotifySend, QWidget *parent)
    : QMainWindow(parent) {
  assert(MainWindow::m_instance == nullptr);
  MainWindow::m_instance = this;
  m_useNotifySend = useNotifySend;
  setupSettings();
  setupWebView();
  setupTrayIcon();
  resize(1000, 700);
  showMaximized();
  if (m_settings->value("trayIcon", false).toBool() &&
      m_settings->value("startHidden", false).toBool()) {
    hide();
    QTimer::singleShot(0, [=]() { hide(); });
  }
}

void MainWindow::setupWebView() {
  auto page = new DiscordPage(this);
  connect(page, &QWebEnginePage::fullScreenRequested, this,
          &MainWindow::fullScreenRequested);

  m_webView = new QWebEngineView(this);
  m_webView->setPage(page);

  if (m_useKF5Notifications || m_useNotifySend)
    QWebEngineProfile::defaultProfile()->setNotificationPresenter(
        [&](std::unique_ptr<QWebEngineNotification> notificationInfo) {
          if (m_useNotifySend) {
            auto title = notificationInfo->title();
            auto message = notificationInfo->message();
            auto image_path =
                QString("/tmp/discord-screenaudio-%1.png").arg(title);
            notificationInfo->icon().save(image_path);
            QProcess::execute("notify-send",
                              {"--icon", image_path, "--app-name",
                               "discord-screenaudio", title, message});
          } else if (m_useKF5Notifications) {
#ifdef KNOTIFICATIONS
            KNotification *notification =
                new KNotification("discordNotification");
            notification->setTitle(notificationInfo->title());
            notification->setText(notificationInfo->message());
            notification->setPixmap(
                QPixmap::fromImage(notificationInfo->icon()));
            notification->setDefaultAction("View");
            connect(notification, &KNotification::defaultActivated,
                    [&, notificationInfo = std::move(notificationInfo)]() {
                      notificationInfo->click();
                      show();
                      activateWindow();
                    });
            notification->sendEvent();
#endif
          }
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

void MainWindow::setupTrayIcon() {
  if (m_settings->value("trayIcon", false).toBool() == false ||
      m_trayIcon != nullptr)
    return;

  auto aboutAction = new QAction(
      "discord-screenaudio v" + QString(DISCORD_SCEENAUDIO_VERSION_FULL), this);
  aboutAction->setIcon(QIcon(":assets/de.shorsh.discord-screenaudio.png"));
  aboutAction->setEnabled(false);

  auto exitAction = new QAction("Exit", this);
  connect(exitAction, &QAction::triggered, []() { QApplication::quit(); });

  m_trayIconMenu = new QMenu(this);
  m_trayIconMenu->addAction(aboutAction);
  m_trayIconMenu->addAction(exitAction);

  m_trayIcon = new QSystemTrayIcon(this);
  m_trayIcon->setContextMenu(m_trayIconMenu);
  m_trayIcon->setIcon(QIcon(":assets/de.shorsh.discord-screenaudio.png"));
  m_trayIcon->show();

  connect(m_trayIcon, &QSystemTrayIcon::activated, [this](auto reason) {
    if (reason == QSystemTrayIcon::Trigger) {
      if (isVisible()) {
        hide();
      } else {
        show();
        activateWindow();
      }
    }
  });
}

void MainWindow::cleanTrayIcon() {
  if (m_trayIcon == nullptr)
    return;
  m_trayIcon->hide();
  m_trayIconMenu->deleteLater();
  m_trayIcon->deleteLater();
  m_trayIconMenu = nullptr;
  m_trayIcon = nullptr;
}

void MainWindow::setupSettings() {
  m_settings = new QSettings("maltejur", "discord-screenaudio", this);
  m_settings->beginGroup("settings");
  m_settings->endGroup();
}

QSettings *MainWindow::settings() const { return m_settings; }

void MainWindow::setTrayIcon(bool enabled) {
  m_settings->setValue("trayIcon", enabled);
  if (enabled) {
    setupTrayIcon();
  } else {
    cleanTrayIcon();
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (m_settings->value("trayIcon", false).toBool()) {
    hide();
  } else
    QApplication::quit();
}

MainWindow *MainWindow::instance() { return m_instance; }
