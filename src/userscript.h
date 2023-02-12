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

class UserScript : public QObject {
  Q_OBJECT

public:
  UserScript();
  bool isVirtmicRunning();
  Q_PROPERTY(QString version READ version);
  Q_PROPERTY(bool kxmlgui MEMBER m_kxmlgui);
  Q_PROPERTY(bool kglobalaccel MEMBER m_kglobalaccel);

private:
  QProcess m_virtmicProcess;
  StreamDialog m_streamDialog;
  bool m_kxmlgui = false;
  bool m_kglobalaccel = false;
#ifdef KXMLGUI
  KHelpMenu *m_helpMenu;
#ifdef KGLOBALACCEL
  KActionCollection *m_actionCollection;
  KShortcutsDialog *m_shortcutsDialog;
#endif
#endif
  void setupHelpMenu();
  void setupShortcutsDialog();
  void setupStreamDialog();
  void setupVirtmic();

Q_SIGNALS:
  void muteToggled();
  void deafenToggled();
  void streamStarted(bool video, int width, int height, int frameRate);

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

private Q_SLOTS:
  void startStream(bool video, bool audio, int width, int height, int frameRate,
                   QString target);
};
