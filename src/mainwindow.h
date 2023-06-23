#pragma once

#include "centralwidget.h"

#include <QMainWindow>
#include <QMenu>
#include <QScopedPointer>
#include <QSettings>
#include <QString>
#include <QSystemTrayIcon>
#include <QVBoxLayout>
#include <QVector>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(bool useNotifySend = false, QWidget *parent = nullptr);
  static MainWindow *instance();
  QSettings *settings() const;
  static CentralWidget *centralWidget();

private:
  void setupTrayIcon();
  void cleanTrayIcon();
  void setupSettings();
  QWebEngineProfile *prepareProfile();
  void closeEvent(QCloseEvent *event) override;
  QSystemTrayIcon *m_trayIcon = nullptr;
  QMenu *m_trayIconMenu;
  QSettings *m_settings;
  bool m_wasMaximized;
  static MainWindow *m_instance;
  CentralWidget *m_centralWidget;

public Q_SLOTS:
  void setTrayIcon(bool enabled);
  void fullScreenRequested(QWebEngineFullScreenRequest fullScreenRequest);
  void toggleOrCloseWindow();
};
