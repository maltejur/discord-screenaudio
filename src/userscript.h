#pragma once

#include "streamdialog.h"

#include <QObject>
#include <QProcess>

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

#ifdef KNOTIFICATIONS
#include <KNotification>
#include <KJob>
#endif


class UserScript : public QObject {
  Q_OBJECT

public:
  UserScript();
  bool isVirtmicRunning();
  Q_PROPERTY(QString version READ version CONSTANT);
  Q_PROPERTY(bool kxmlgui MEMBER m_kxmlgui CONSTANT);
  Q_PROPERTY(bool kglobalaccel MEMBER m_kglobalaccel CONSTANT);
  Q_PROPERTY(QString userstyles MEMBER m_userstyles NOTIFY userstylesChanged);
  Q_PROPERTY(QString loadingMessage MEMBER m_loadingMessage NOTIFY
                 loadingMessageChanged);

private:
  QProcess m_virtmicProcess;
  StreamDialog *m_streamDialog;
  bool m_kxmlgui = false;
  bool m_kglobalaccel = false;
  QString m_userstyles;
  QString m_loadingMessage;
#ifdef KXMLGUI
  KHelpMenu *m_helpMenu;
#ifdef KGLOBALACCEL
  KActionCollection *m_actionCollection;
  KShortcutsDialog *m_shortcutsDialog;
#endif
#endif
#ifdef KNOTIFICATIONS
  KJob *m_loadingJob = nullptr;
#endif
  void setupHelpMenu();
  void setupShortcutsDialog();
  void setupStreamDialog();
  void setupVirtmic();

Q_SIGNALS:
  void muteToggled();
  void deafenToggled();
  void streamStarted(bool video, int width, int height, int frameRate);
  void userstylesChanged();
  void loadingMessageChanged(QString message);
  void shouldInstallUserStyles(QString url);

public Q_SLOTS:
  void log(QString message);
  QString version();
  QVariant getPref(QString name, QVariant fallback);
  bool getBoolPref(QString name, bool fallback);
  void setPref(QString name, QVariant value);
  void setTrayIcon(bool value);
  void showShortcutsDialog();
  void showHelpMenu();
  void showStreamDialog();
  void stopVirtmic();
  void startVirtmic(QString target);
  void showThemeDialog();
  void installUserStyles(QString url);

private Q_SLOTS:
  void startStream(bool video, bool audio, int width, int height, int frameRate,
                   QString target);
};
