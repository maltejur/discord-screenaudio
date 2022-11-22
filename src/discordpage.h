#pragma once

#include "streamdialog.h"
#include "virtmic.h"

#ifdef KXMLGUI
#include <KActionCollection>
#include <KHelpMenu>
#include <KShortcutsDialog>
#endif

#include <QProcess>
#include <QWebEngineFullScreenRequest>
#include <QWebEnginePage>

class DiscordPage : public QWebEnginePage {
  Q_OBJECT

public:
  explicit DiscordPage(QWidget *parent = nullptr);

private:
  StreamDialog m_streamDialog;
  QProcess m_virtmicProcess;
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
  void injectScriptText(QString name, QString content);
  void injectScriptFile(QString name, QString source);
  void stopVirtmic();
  void startVirtmic(QString target);
  void toggleMute();
  void toggleDeafen();

private Q_SLOTS:
  void featurePermissionRequested(const QUrl &securityOrigin,
                                  QWebEnginePage::Feature feature);
  void startStream(bool video, bool audio, uint width, uint height,
                   uint frameRate, QString target);
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
