#include "discordpage.h"
#include "log.h"
#include "mainwindow.h"
#include "virtmic.h"

#ifdef KXMLGUI
#include <KAboutData>
#include <KHelpMenu>
#include <KShortcutsDialog>
#include <KXmlGuiWindow>
#include <QAction>

#ifdef KGLOBALACCEL
#include <KGlobalAccel>
#endif

#endif

#include <QApplication>
#include <QDesktopServices>
#include <QFile>
#include <QMessageBox>
#include <QNetworkReply>
#include <QTimer>
#include <QWebChannel>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>

DiscordPage::DiscordPage(QWidget *parent) : QWebEnginePage(parent) {
  setBackgroundColor(QColor("#202225"));
  m_virtmicProcess.setProcessChannelMode(QProcess::ForwardedChannels);

  connect(this, &QWebEnginePage::featurePermissionRequested, this,
          &DiscordPage::featurePermissionRequested);

  connect(this, &QWebEnginePage::loadStarted, [=]() {
    runJavaScript(QString("window.discordScreenaudioVersion = '%1';")
                      .arg(QApplication::applicationVersion()));
  });

  settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);
  settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
  settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent,
                           true);
  settings()->setAttribute(
      QWebEngineSettings::AllowWindowActivationFromJavaScript, true);
  settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
  settings()->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture,
                           false);
  settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
  settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);

  setUrl(QUrl("https://discord.com/app"));

  injectScriptFile("userscript.js", ":/assets/userscript.js");

  injectScriptText("version.js",
                   QString("window.discordScreenaudioVersion = '%1';")
                       .arg(QApplication::applicationVersion()));

#ifdef KXMLGUI
  injectScriptText("xmlgui.js", "window.discordScreenaudioKXMLGUI = true;");

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
  m_helpMenu = new KHelpMenu(parent, aboutData);

#ifdef KGLOBALACCEL
  injectScriptText("kglobalaccel.js",
                   "window.discordScreenaudioKGLOBALACCEL = true;");

  auto toggleMuteAction = new QAction(this);
  toggleMuteAction->setText("Toggle Mute");
  toggleMuteAction->setIcon(QIcon::fromTheme("microphone-sensitivity-muted"));
  connect(toggleMuteAction, &QAction::triggered, this,
          &DiscordPage::toggleMute);

  auto toggleDeafenAction = new QAction(this);
  toggleDeafenAction->setText("Toggle Deafen");
  toggleDeafenAction->setIcon(QIcon::fromTheme("audio-volume-muted"));
  connect(toggleDeafenAction, &QAction::triggered, this,
          &DiscordPage::toggleDeafen);

  m_actionCollection = new KActionCollection(this);
  m_actionCollection->addAction("toggleMute", toggleMuteAction);
  KGlobalAccel::setGlobalShortcut(toggleMuteAction, QList<QKeySequence>{});
  m_actionCollection->addAction("toggleDeafen", toggleDeafenAction);
  KGlobalAccel::setGlobalShortcut(toggleDeafenAction, QList<QKeySequence>{});

  m_shortcutsDialog = new KShortcutsDialog(KShortcutsEditor::GlobalAction);
  m_shortcutsDialog->addCollection(m_actionCollection);
#endif
#endif

  connect(&m_streamDialog, &StreamDialog::requestedStreamStart, this,
          &DiscordPage::startStream);
}

void DiscordPage::injectScriptText(QString name, QString content) {
  qDebug(mainLog) << "Injecting " << name;

  QWebEngineScript script;

  script.setSourceCode(content);
  script.setName(name);
  script.setWorldId(QWebEngineScript::MainWorld);
  script.setInjectionPoint(QWebEngineScript::DocumentCreation);
  script.setRunsOnSubFrames(false);

  scripts().insert(script);
}

void DiscordPage::injectScriptFile(QString name, QString source) {
  QFile file(source);

  if (!file.open(QIODevice::ReadOnly)) {
    qFatal("Failed to load %s with error: %s", source.toLatin1().constData(),
           file.errorString().toLatin1().constData());
  } else {
    injectScriptText(name, file.readAll());
  }
}

void DiscordPage::featurePermissionRequested(const QUrl &securityOrigin,
                                             QWebEnginePage::Feature feature) {
  // Allow every permission asked
  setFeaturePermission(securityOrigin, feature,
                       QWebEnginePage::PermissionGrantedByUser);

  if (feature == QWebEnginePage::Feature::MediaAudioCapture) {
    if (m_virtmicProcess.state() == QProcess::NotRunning) {
      qDebug(virtmicLog) << "Starting Virtmic with no target to make sure "
                            "Discord can find all the audio devices";
      m_virtmicProcess.start(QApplication::arguments()[0],
                             {"--virtmic", "None"});
    }
  }
}

bool DiscordPage::acceptNavigationRequest(const QUrl &url,
                                          QWebEnginePage::NavigationType type,
                                          bool isMainFrame) {
  if (type == QWebEnginePage::NavigationTypeLinkClicked) {
    QDesktopServices::openUrl(url);
    return false;
  }
  return true;
};

bool ExternalPage::acceptNavigationRequest(const QUrl &url,
                                           QWebEnginePage::NavigationType type,
                                           bool isMainFrame) {
  QDesktopServices::openUrl(url);
  deleteLater();
  return false;
}

QWebEnginePage *DiscordPage::createWindow(QWebEnginePage::WebWindowType type) {
  return new ExternalPage;
}

void DiscordPage::stopVirtmic() {
  if (m_virtmicProcess.state() == QProcess::Running) {
    qDebug(virtmicLog) << "Stopping Virtmic";
    m_virtmicProcess.kill();
    m_virtmicProcess.waitForFinished();
  }
}

void DiscordPage::startVirtmic(QString target) {
  if (target != "None") {
    qDebug(virtmicLog) << "Starting Virtmic with target" << target;
    m_virtmicProcess.start(QApplication::arguments()[0], {"--virtmic", target});
  }
}

void DiscordPage::javaScriptConsoleMessage(
    QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message,
    int lineNumber, const QString &sourceID) {
  if (message == "!discord-screenaudio-start-stream") {
    if (m_streamDialog.isHidden())
      m_streamDialog.setHidden(false);
    else
      m_streamDialog.activateWindow();
    m_streamDialog.updateTargets();
  } else if (message == "!discord-screenaudio-stream-stopped") {
    stopVirtmic();
  } else if (message == "!discord-screenaudio-about") {
#ifdef KXMLGUI
    m_helpMenu->aboutApplication();
#endif
  } else if (message == "!discord-screenaudio-keybinds") {
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
  } else if (message.startsWith("dsa: ")) {
    qDebug(userscriptLog) << message.mid(5).toUtf8().constData();
  } else {
    qDebug(discordLog) << message;
  }
}

void DiscordPage::startStream(QString target, uint width, uint height,
                              uint frameRate) {
  stopVirtmic();
  startVirtmic(target);
  // Wait a bit for the virtmic to start
  QTimer::singleShot(target == "None" ? 0 : 200, [=]() {
    runJavaScript(QString("window.discordScreenaudioStartStream(%1, %2, %3);")
                      .arg(width)
                      .arg(height)
                      .arg(frameRate));
  });
}

void DiscordPage::toggleMute() {
  qDebug(shortcutLog) << "Toggling mute";
  runJavaScript("window.discordScreenaudioToggleMute();");
}

void DiscordPage::toggleDeafen() {
  qDebug(shortcutLog) << "Toggling deafen";
  runJavaScript("window.discordScreenaudioToggleDeafen();");
}
