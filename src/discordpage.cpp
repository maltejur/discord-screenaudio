#include "discordpage.h"
#include "log.h"
#include "mainwindow.h"
#include "virtmic.h"

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

  injectScriptFile("qwebchannel.js", ":/qtwebchannel/qwebchannel.js");

  setUrl(QUrl("https://discord.com/app"));

  setWebChannel(new QWebChannel(this));
  webChannel()->registerObject("userscript", &m_userScript);

  injectScriptFile("userscript.js", ":/assets/userscript.js");
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
    if (!m_userScript.isVirtmicRunning()) {
      qDebug(virtmicLog) << "Starting Virtmic with no target to make sure "
                            "Discord can find all the audio devices";
      m_userScript.startVirtmic("None");
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

void DiscordPage::javaScriptConsoleMessage(
    QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message,
    int lineNumber, const QString &sourceID) {
  qDebug(discordLog) << message;
}
