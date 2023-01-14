#pragma once

#include "discordpage.h"

#include <QMainWindow>
#include <QMenu>
#include <QScopedPointer>
#include <QSettings>
#include <QString>
#include <QSystemTrayIcon>
#include <QVector>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineView>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(bool useNotifySend = false, QWidget *parent = nullptr);
  static MainWindow *instance();
  QSettings *settings() const;

private:
  void setupWebView();
  void setupTrayIcon();
  void cleanTrayIcon();
  void setupSettings();
  QWebEngineView *m_webView;
  QWebEngineProfile *prepareProfile();
  DiscordPage *m_discordPage;
  void closeEvent(QCloseEvent *event) override;
  QSystemTrayIcon *m_trayIcon = nullptr;
  QMenu *m_trayIconMenu;
  QSettings *m_settings;
  bool m_wasMaximized;
  static MainWindow *m_instance;
  bool m_useNotifySend;
#ifdef KNOTIFICATIONS
  bool m_useKF5Notifications = true;
#else
  bool m_useKF5Notifications = false;
#endif

public Q_SLOTS:
  void setTrayIcon(bool enabled);

private Q_SLOTS:
  void fullScreenRequested(QWebEngineFullScreenRequest fullScreenRequest);
};
