#pragma once

#include "streamdialog.h"
#include "virtmic.h"
#include "webclass.h"

#ifdef KXMLGUI
#include <KActionCollection>
#include <KHelpMenu>
#include <KShortcutsDialog>
#endif

#include <QProcess>
#include <QWebEngineFullScreenRequest>
#include <QWebEnginePage>
#include <QWebEngineScript>

class DiscordPage : public QWebEnginePage {
  Q_OBJECT

public:
  explicit DiscordPage(QWidget *parent = nullptr);

private:
  StreamDialog m_streamDialog;
  QProcess m_virtmicProcess;
  WebClass m_webClass;
#ifdef KXMLGUI
  KHelpMenu *m_helpMenu;
#ifdef KGLOBALACCEL
  KActionCollection *m_actionCollection;
  KShortcutsDialog *m_shortcutsDialog;
#endif
#endif
  bool acceptNavigationRequest(const QUrl &url,
                               QWebEnginePage::NavigationType type,
                               bool isMainFrame) override;
  QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type) override;
  void
  javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                           const QString &message, int lineNumber,
                           const QString &sourceID) override;
  void injectScriptText(QString name, QString content,
                        QWebEngineScript::InjectionPoint injectionPoint =
                            QWebEngineScript::DocumentCreation);
  void injectScriptFile(QString name, QString source,
                        QWebEngineScript::InjectionPoint injectionPoint =
                            QWebEngineScript::DocumentCreation);
  void stopVirtmic();
  void startVirtmic(QString target);
  void toggleMute();
  void toggleDeafen();

private Q_SLOTS:
  void featurePermissionRequested(const QUrl &securityOrigin,
                                  QWebEnginePage::Feature feature);
  void startStream(QString target, uint width, uint height, uint frameRate);
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
