#pragma once

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
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private:
  void setupWebView();
  QWebEngineView *m_webView;
  QWebEngineProfile *prepareProfile();
  QThread *m_virtmicThread;

private Q_SLOTS:
  void featurePermissionRequested(const QUrl &securityOrigin,
                                  QWebEnginePage::Feature feature);
};
