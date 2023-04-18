#include "discordpage.h"
#include "log.h"
#include "mainwindow.h"
#include "virtmic.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QNetworkReply>
#include <QTemporaryFile>
#include <QTimer>
#include <QWebChannel>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>

DiscordPage::DiscordPage(QWidget *parent) : QWebEnginePage(parent) {
  setBackgroundColor(QColor("#313338"));

  connect(this, &QWebEnginePage::featurePermissionRequested, this,
          &DiscordPage::featurePermissionRequested);
  connect(this, &DiscordPage::fullScreenRequested, MainWindow::instance(),
          &MainWindow::fullScreenRequested);

  setupPermissions();

  injectFile(&DiscordPage::injectScript, "qwebchannel.js",
             ":/qtwebchannel/qwebchannel.js");

  setUrl(QUrl("https://discord.com/app"));

  setWebChannel(new QWebChannel(this));
  webChannel()->registerObject("userscript", &m_userScript);

  injectFile(&DiscordPage::injectScript, "userscript.js",
             ":/assets/userscript.js");

  setupUserStyles();
  setupArrpc();
}

void DiscordPage::setupPermissions() {
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
}

void DiscordPage::setupUserStyles() {
  qDebug(userstylesLog).noquote()
      << "Looking for userstyles in" << m_configLocation.absolutePath();
  m_userStylesFile =
      new QFile(m_configLocation.absoluteFilePath("userstyles.css"));
  if (m_userStylesFile->exists()) {
    qDebug(userstylesLog).noquote()
        << "Found userstyles:" << m_userStylesFile->fileName();
    m_userStylesFile->open(QIODevice::ReadOnly);
    m_userStylesContent = m_userStylesFile->readAll();
    m_userStylesFile->close();
    fetchUserStyles();
  }
  connect(&m_userScript, &UserScript::shouldInstallUserStyles, this,
          &DiscordPage::getUserStyles);
}

const QRegularExpression importRegex(
    R"r(@import (?:url\(|)['"]{0,1}(?!.*usrbgs?\.css)([^'"]+?)['"]{0,1}(?:|\));)r");
const QRegularExpression urlRegex(
    R"r(url\(['"]{0,1}((?!https:\/\/fonts.gstatic.com)(?!data:)(?!.*\.woff2)(?!.*\.ttf)[^'"]+?)['"]{0,1}\))r");

void DiscordPage::fetchUserStyles() {
  m_userScript.setProperty(
      "loadingMessage", "Loading userstyles: Fetching additional resources...");
  bool foundImport = true;
  auto match = importRegex.match(m_userStylesContent);
  if (!match.hasMatch()) {
    foundImport = false;
    match = urlRegex.match(m_userStylesContent);
  }
  if (match.hasMatch()) {
    auto url = match.captured(1);
    qDebug(userstylesLog) << "Fetching" << url;
    m_userScript.setProperty(
        "loadingMessage",
        QString("Loading userstyles: Fetching %1...").arg(url));
    QNetworkRequest request(url);
    auto reply = m_networkAccessManager.get(request);
    connect(reply, &QNetworkReply::finished, [=]() {
      QByteArray content = "";
      if (reply->error() == QNetworkReply::NoError) {
        if (!reply->attribute(QNetworkRequest::RedirectionTargetAttribute)
                 .isNull())
          content =
              reply->attribute(QNetworkRequest::RedirectionTargetAttribute)
                  .toByteArray();
        else
          content = reply->readAll();
      } else
        qDebug(userstylesLog) << reply->errorString().toUtf8().constData();
      reply->deleteLater();
      m_userStylesContent = m_userStylesContent.replace(
          match.captured(0), foundImport
                                 ? content
                                 : "url(data:application/octet-stream;base64," +
                                       content.toBase64() + ")");
      fetchUserStyles();
    });
    return;
  }
  qDebug(userstylesLog) << "Injecting userstyles";
  m_userScript.setProperty("userstyles", m_userStylesContent);
  m_userScript.setProperty("loadingMessage", "");
  if (!m_configLocation.exists())
    m_configLocation.mkpath(".");
  m_userStylesFile->open(QIODevice::WriteOnly);
  m_userStylesFile->write(m_userStylesContent.toUtf8());
  m_userStylesFile->close();
}

void DiscordPage::getUserStyles(QString url) {
  m_userStylesContent = url == "" ? "" : QString("@import url(%1);").arg(url);
  fetchUserStyles();
}

void DiscordPage::injectScript(
    QString name, QString content,
    QWebEngineScript::InjectionPoint injectionPoint) {
  qDebug(mainLog) << "Injecting " << name;

  QWebEngineScript script;

  script.setSourceCode(content);
  script.setName(name);
  script.setWorldId(QWebEngineScript::MainWorld);
  script.setInjectionPoint(injectionPoint);
  script.setRunsOnSubFrames(false);

  scripts().insert(script);
}

void DiscordPage::injectScript(QString name, QString content) {
  injectScript(name, content, QWebEngineScript::DocumentCreation);
}

void DiscordPage::injectStylesheet(QString name, QString content) {
  auto script = QString(R"(const stylesheet = document.createElement("style");
stylesheet.id = "%1";
stylesheet.innerText = `%2`;
document.head.appendChild(stylesheet);
)")
                    .arg(name)
                    .arg(content);
  injectScript(name, script, QWebEngineScript::DocumentReady);
}

void DiscordPage::injectFile(void (DiscordPage::*inject)(QString, QString),
                             QString name, QString source) {
  QFile file(source);

  if (!file.open(QIODevice::ReadOnly)) {
    qFatal("Failed to load %s with error: %s", source.toLatin1().constData(),
           file.errorString().toLatin1().constData());
  } else {
    (this->*inject)(name, file.readAll());
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

const QMap<QString, QString> cssAnsiColorMap = {{"black", "30"},
                                                {"red", "31"},
                                                {"green", "32"},
                                                {"yellow", "33"},
                                                {"blue", "34"},
                                                {"magenta", "35"},
                                                {"cyan", "36"},
                                                {"white", "37"},
                                                {"gray", "90"},
                                                {"bright-red", "91"},
                                                {"bright-green", "92"},
                                                {"bright-yellow", "93"},
                                                {"bright-blue", "94"},
                                                {"bright-magenta", "95"},
                                                {"bright-cyan", "96"},
                                                {"bright-white", "97"},
                                                {"orange", "38;5;208"},
                                                {"pink", "38;5;205"},
                                                {"brown", "38;5;94"},
                                                {"light-gray", "38;5;251"},
                                                {"dark-gray", "38;5;239"},
                                                {"light-red", "38;5;203"},
                                                {"light-green", "38;5;83"},
                                                {"light-yellow", "38;5;227"},
                                                {"light-blue", "38;5;75"},
                                                {"light-magenta", "38;5;207"},
                                                {"light-cyan", "38;5;87"},
                                                {"turquoise", "38;5;80"},
                                                {"violet", "38;5;92"},
                                                {"purple", "38;5;127"},
                                                {"lavender", "38;5;183"},
                                                {"maroon", "38;5;124"},
                                                {"beige", "38;5;230"},
                                                {"olive", "38;5;142"},
                                                {"indigo", "38;5;54"},
                                                {"teal", "38;5;30"},
                                                {"gold", "38;5;220"},
                                                {"silver", "38;5;7"},
                                                {"navy", "38;5;17"},
                                                {"steel", "38;5;188"},
                                                {"salmon", "38;5;173"},
                                                {"peach", "38;5;217"},
                                                {"khaki", "38;5;179"},
                                                {"coral", "38;5;209"},
                                                {"crimson", "38;5;160"}};

void DiscordPage::javaScriptConsoleMessage(
    QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message,
    int lineNumber, const QString &sourceID) {
  auto colorSegments = message.split("%c");
  if (colorSegments[0] != "") {
    for (auto line : colorSegments[0].split("\n"))
      qDebug(discordLog) << line.toUtf8().constData();
  }
  for (auto segment : colorSegments.mid(1)) {
    auto lines = segment.split("\n");
    QString ansi;
    uint endOfStyles = lines.length();
    for (auto line = 1; line < lines.length(); line++) {
      if (!lines[line].endsWith(";")) {
        endOfStyles = line;
        break;
      }
      if (lines[line] == "font-weight: bold;")
        ansi += "\033[1m";
      else if (lines[line].startsWith("color: ")) {
        auto color = lines[line].mid(7).chopped(1);
        if (cssAnsiColorMap.find(color) != cssAnsiColorMap.end())
          ansi += "\033[" + cssAnsiColorMap[color] + "m";
      }
    }
    if (endOfStyles < lines.length())
      qDebug(discordLog) << (ansi + lines[0].trimmed() + "\033[0m " +
                             lines[endOfStyles].trimmed())
                                .toUtf8()
                                .constData();
    for (auto line : lines.mid(endOfStyles + 1)) {
      qDebug(discordLog) << line.toUtf8().constData();
    }
  }
}

UserScript *DiscordPage::userScript() { return &m_userScript; }

void DiscordPage::setupArrpc() {
  QFile nodejs("/usr/bin/node");
  if (nodejs.exists()) {
    auto arrpcSource = QTemporaryFile::createNativeFile(":/assets/arrpc.js");
    qDebug(mainLog).noquote()
        << "NodeJS found, starting arRPC located at" << arrpcSource->fileName();
    m_arrpcProcess.setProcessChannelMode(QProcess::ForwardedChannels);
    m_arrpcProcess.setProgram(nodejs.fileName());
    m_arrpcProcess.setArguments(QStringList{arrpcSource->fileName()});
    m_arrpcProcess.start();

    injectFile(&DiscordPage::injectScript, "arrpc_bridge_mod.js",
               ":/assets/arrpc_bridge_mod.js");
  }
}
