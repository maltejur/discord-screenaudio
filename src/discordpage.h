#pragma once

#include "userscript.h"

#include <QWebEngineFullScreenRequest>
#include <QWebEnginePage>

class DiscordPage : public QWebEnginePage {
  Q_OBJECT

public:
  explicit DiscordPage(QWidget *parent = nullptr);

private:
  UserScript m_userScript;
  bool acceptNavigationRequest(const QUrl &url,
                               QWebEnginePage::NavigationType type,
                               bool isMainFrame) override;
  QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type) override;
  void
  javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                           const QString &message, int lineNumber,
                           const QString &sourceID) override;
  void injectScriptText(QString name, QString content);
  void injectScriptFile(QString name, QString source);

private Q_SLOTS:
  void featurePermissionRequested(const QUrl &securityOrigin,
                                  QWebEnginePage::Feature feature);
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
