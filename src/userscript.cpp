#include "userscript.h"
#include "log.h"
#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>

#ifdef KXMLGUI
#include <KActionCollection>
#endif

UserScript::UserScript() : QObject() {
  setupHelpMenu();
  setupShortcutsDialog();
  setupStreamDialog();
  setupVirtmic();
}

void UserScript::setupHelpMenu() {
#ifdef KXMLGUI
  m_kxmlgui = true;

  KAboutData aboutData(
      "discord-screenaudio", "discord-screenaudio",
      QApplication::applicationVersion(),
      "Custom Discord client with the ability to stream audio on Linux",
      KAboutLicense::GPL_V3, "Copyright 2022 (C) Malte Jürgens");
  aboutData.setBugAddress("https://github.com/maltejur/discord-screenaudio");
  aboutData.addAuthor("Malte Jürgens", "Author", "maltejur@dismail.de",
                      "https://github.com/maltejur");
  aboutData.addCredit("edisionnano",
                      "For creating and documenting the approach for streaming "
                      "audio in Discord used in this project.",
                      QString(),
                      "https://github.com/edisionnano/"
                      "Screenshare-with-audio-on-Discord-with-Linux");
  aboutData.addCredit(
      "Curve", "For creating the Rohrkabel library used in this project.",
      QString(), "https://github.com/Curve");
  aboutData.addComponent("Rohrkabel", "A C++ RAII Pipewire-API Wrapper", "1.3",
                         "https://github.com/Soundux/rohrkabel");
  m_helpMenu = new KHelpMenu(MainWindow::instance(), aboutData);
#endif
}

void UserScript::setupShortcutsDialog() {
#ifdef KXMLGUI
#ifdef KGLOBALACCEL
  m_kglobalaccel = true;

  auto toggleMuteAction = new QAction(this);
  toggleMuteAction->setText("Toggle Mute");
  toggleMuteAction->setIcon(QIcon::fromTheme("microphone-sensitivity-muted"));
  connect(toggleMuteAction, &QAction::triggered, this,
          &UserScript::muteToggled);

  auto toggleDeafenAction = new QAction(this);
  toggleDeafenAction->setText("Toggle Deafen");
  toggleDeafenAction->setIcon(QIcon::fromTheme("audio-volume-muted"));
  connect(toggleMuteAction, &QAction::triggered, this,
          &UserScript::deafenToggled);

  m_actionCollection = new KActionCollection(this);
  m_actionCollection->addAction("toggleMute", toggleMuteAction);
  KGlobalAccel::setGlobalShortcut(toggleMuteAction, QList<QKeySequence>{});
  m_actionCollection->addAction("toggleDeafen", toggleDeafenAction);
  KGlobalAccel::setGlobalShortcut(toggleDeafenAction, QList<QKeySequence>{});

  m_shortcutsDialog = new KShortcutsDialog(KShortcutsEditor::GlobalAction);
  m_shortcutsDialog->addCollection(m_actionCollection);
#endif
#endif
}

void UserScript::setupStreamDialog() {
  connect(&m_streamDialog, &StreamDialog::requestedStreamStart, this,
          &UserScript::startStream);
}

void UserScript::setupVirtmic() {
  m_virtmicProcess.setProcessChannelMode(QProcess::ForwardedChannels);
}

bool UserScript::isVirtmicRunning() {
  return m_virtmicProcess.state() != QProcess::NotRunning;
}

QString UserScript::version() { return QApplication::applicationVersion(); }

QVariant UserScript::getPref(QString name, QVariant fallback) {
  return MainWindow::instance()->settings()->value(name, fallback);
}

bool UserScript::getBoolPref(QString name, bool fallback) {
  return getPref(name, fallback).toBool();
}

void UserScript::setPref(QString name, QVariant value) {
  return MainWindow::instance()->settings()->setValue(name, value);
}

void UserScript::setTrayIcon(bool value) {
  setPref("trayIcon", value);
  MainWindow::instance()->setTrayIcon(value);
}

void UserScript::log(QString message) {
  qDebug(userscriptLog) << message.toUtf8().constData();
}

void UserScript::showShortcutsDialog() {
#ifdef KXMLGUI
#ifdef KGLOBALACCEL
  m_shortcutsDialog->show();
#else
  QMessageBox::information(MainWindow::instance(), "discord-screenaudio",
                           "Keybinds are not supported on this platform "
                           "(KGlobalAccel is not available).",
                           QMessageBox::Ok);
#endif
#else
  QMessageBox::information(MainWindow::instance(), "discord-screenaudio",
                           "Keybinds are not supported on this platform "
                           "(KXmlGui and KGlobalAccel are not available).",
                           QMessageBox::Ok);
#endif
}

void UserScript::showHelpMenu() {
#ifdef KXMLGUI
  m_helpMenu->aboutApplication();
#endif
}

void UserScript::stopVirtmic() {
  if (m_virtmicProcess.state() == QProcess::Running) {
    qDebug(virtmicLog) << "Stopping Virtmic";
    m_virtmicProcess.kill();
    m_virtmicProcess.waitForFinished();
  }
}

void UserScript::startVirtmic(QString target) {
  qDebug(virtmicLog) << "Starting Virtmic with target" << target;
  m_virtmicProcess.start(QApplication::arguments()[0], {"--virtmic", target});
}

void UserScript::startStream(bool video, bool audio, int width, int height,
                             int frameRate, QString target) {
  stopVirtmic();
  startVirtmic(audio ? target : "[None]");
  // Wait a bit for the virtmic to start
  QTimer::singleShot(
      200, [=]() { emit streamStarted(video, width, height, frameRate); });
}

void UserScript::showStreamDialog() {
  if (m_streamDialog.isHidden())
    m_streamDialog.setHidden(false);
  else
    m_streamDialog.activateWindow();
  m_streamDialog.updateTargets();
}