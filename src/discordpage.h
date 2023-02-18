#pragma once

#include "userscript.h"

#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QStandardPaths>
#include <QWebEngineFullScreenRequest>
#include <QWebEnginePage>
#include <QWebEngineScript>

class DiscordPage : public QWebEnginePage {
  Q_OBJECT

public:
  explicit DiscordPage(QWidget *parent = nullptr);
  UserScript *userScript();

private:
  UserScript m_userScript;
  QFile *m_userStylesFile;
  QString m_userStylesContent;
  QNetworkAccessManager m_networkAccessManager;
  const QDir m_configLocation =
      QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  void setupPermissions();
  void setupUserStyles();
  void fetchUserStyles();
  bool acceptNavigationRequest(const QUrl &url,
                               QWebEnginePage::NavigationType type,
                               bool isMainFrame) override;
  QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type) override;
  void
  javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                           const QString &message, int lineNumber,
                           const QString &sourceID) override;
  void injectScript(QString name, QString content,
                    QWebEngineScript::InjectionPoint injectionPoint);
  void injectScript(QString name, QString content);
  void injectStylesheet(QString name, QString content);
  void injectFile(void (DiscordPage::*inject)(QString, QString), QString name,
                  QString source);

private Q_SLOTS:
  void featurePermissionRequested(const QUrl &securityOrigin,
                                  QWebEnginePage::Feature feature);

public Q_SLOTS:
  void getUserStyles(QString url);
};

// Will immediately get destroyed again but is needed for navigation to
// target="_blank" links, since QWebEnginePage::newWindowRequested is
// only available sinec Qt 6.3.
class ExternalPage : public QWebEnginePage {
  Q_OBJECT

private:
  bool acceptNavigationRequest(const QUrl &url,
                               QWebEnginePage::NavigationType type,
                               bool isMainFrame) override;
};
