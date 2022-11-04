#pragma once

#include "discordpage.h"

#include <QMainWindow>
#include <QScopedPointer>
#include <QString>
#include <QVector>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineView>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(bool useNotifySend = false, QWidget *parent = nullptr);
  static MainWindow *instance();

private:
  void setupWebView();
  QWebEngineView *m_webView;
  QWebEngineProfile *prepareProfile();
  DiscordPage *m_discordPage;
  void closeEvent(QCloseEvent *event) override;
  bool m_wasMaximized;
  static MainWindow *m_instance;
  bool m_useNotifySend;
#ifdef KNOTIFICATIONS
  bool m_useKF5Notifications = true;
#else
  bool m_useKF5Notifications = false;
#endif

private Q_SLOTS:
  void fullScreenRequested(QWebEngineFullScreenRequest fullScreenRequest);
};
