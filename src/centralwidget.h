#pragma once

#include <QLabel>
#include <QVBoxLayout>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <QWidget>

class CentralWidget : public QWidget {
  Q_OBJECT

public:
  CentralWidget(QWidget *parent = nullptr);

private:
  void setupWebView();
  QVBoxLayout *m_layout;
  QWebEngineView *m_webView;
#ifdef KNOTIFICATIONS
  bool m_useKF5Notifications = true;
#else
  bool m_useKF5Notifications = false;
#endif
  QLabel *m_loadingLabel = nullptr;

public Q_SLOTS:
  void setLoadingIndicator(QString text);
};
