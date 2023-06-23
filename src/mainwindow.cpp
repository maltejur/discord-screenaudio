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
#include <QShortcut>
#include <QSpacerItem>
#include <QThread>
#include <QTimer>
#include <QUrl>
#include <QWebEngineFullScreenRequest>
#include <QWidget>

MainWindow *MainWindow::m_instance = nullptr;

MainWindow::MainWindow(bool useNotifySend, QWidget *parent)
    : QMainWindow(parent) {
  assert(MainWindow::m_instance == nullptr);
  MainWindow::m_instance = this;
  setupSettings();
  m_settings->setValue("useNotifySend", useNotifySend);
  m_centralWidget = new CentralWidget(this);
  setCentralWidget(m_centralWidget);
  setupTrayIcon();
  setMinimumSize(800, 300);
  connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this),
          &QShortcut::activated, this, &MainWindow::toggleOrCloseWindow);
  if (m_settings->contains("geometry")) {
    restoreGeometry(m_settings->value("geometry").toByteArray());
  } else {
    resize(1000, 700);
    showMaximized();
  }
  if (m_settings->value("trayIcon", false).toBool() &&
      m_settings->value("startHidden", false).toBool()) {
    hide();
    QTimer::singleShot(0, [=]() { hide(); });
  }
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
      toggleOrCloseWindow();
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
  m_settings =
      new QSettings("discord-screenaudio", "discord-screenaudio", this);
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
  } else {
    m_settings->setValue("geometry", saveGeometry());
    QApplication::quit();
  }
}

MainWindow *MainWindow::instance() { return m_instance; }

CentralWidget *MainWindow::centralWidget() {
  return instance()->m_centralWidget;
};

void MainWindow::toggleOrCloseWindow() {
  if (isVisible()) {
    if (m_trayIcon == nullptr)
      QApplication::quit();
    else
      hide();
  } else {
    show();
    activateWindow();
  }
}
