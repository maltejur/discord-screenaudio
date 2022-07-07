#include "mainwindow.h"
#include "virtmic.h"

#include <QColor>
#include <QComboBox>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QThread>
#include <QUrl>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  auto centralWidget = new QWidget;

  auto layout = new QGridLayout;
  layout->setAlignment(Qt::AlignCenter);

  auto label = new QLabel;
  label->setText("Which app do you want to stream sound from?");

  auto comboBox = new QComboBox;
  for (auto target : Virtmic::getTargets()) {
    comboBox->addItem(target);
  }

  auto button = new QPushButton;
  button->setText("Confirm");
  connect(button, &QPushButton::clicked, [=]() {
    auto target = comboBox->currentText();
    auto thread = QThread::create([=]() { Virtmic::start(target); });
    thread->start();
    setupWebView();
  });

  layout->addWidget(label, 0, 0);
  layout->addWidget(comboBox, 1, 0);
  layout->addWidget(button, 2, 0, Qt::AlignRight);
  centralWidget->setLayout(layout);
  setCentralWidget(centralWidget);
  resize(1000, 700);
  showMaximized();
}

void MainWindow::setupWebView() {
  m_webView = new QWebEngineView(this);
  m_webView->page()->setBackgroundColor(QColor("#202225"));

  // TODO: Custom QWebEnginePage that implements acceptNavigationRequest
  connect(m_webView->page(), &QWebEnginePage::featurePermissionRequested, this,
          &MainWindow::featurePermissionRequested);
  m_webView->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled,
                                      true);
  m_webView->settings()->setAttribute(
      QWebEngineSettings::JavascriptCanOpenWindows, true);
  m_webView->settings()->setAttribute(
      QWebEngineSettings::AllowRunningInsecureContent, true);
  m_webView->settings()->setAttribute(
      QWebEngineSettings::AllowWindowActivationFromJavaScript, true);
  m_webView->settings()->setAttribute(
      QWebEngineSettings::FullScreenSupportEnabled, true);
  m_webView->settings()->setAttribute(
      QWebEngineSettings::PlaybackRequiresUserGesture, false);

  m_webView->setUrl(QUrl("https://discord.com/app"));

  const char *userscriptSrc = ":/assets/userscript.js";
  QFile userscript(userscriptSrc);

  if (!userscript.open(QIODevice::ReadOnly)) {
    qFatal("Failed to load %s with error: %s", userscriptSrc,
           userscript.errorString().toLatin1().constData());
  } else {
    QByteArray userscriptJs = userscript.readAll();

    QWebEngineScript script;

    script.setSourceCode(userscriptJs);
    script.setName("userscript.js");
    script.setWorldId(QWebEngineScript::MainWorld);
    script.setInjectionPoint(QWebEngineScript::DocumentCreation);
    script.setRunsOnSubFrames(false);

    m_webView->page()->scripts().insert(script);
  }

  setCentralWidget(m_webView);
}

void MainWindow::featurePermissionRequested(const QUrl &securityOrigin,
                                            QWebEnginePage::Feature feature) {
  // if (feature == QWebEnginePage::MediaAudioCapture ||
  //     feature == QWebEnginePage::MediaVideoCapture ||
  //     feature == QWebEnginePage::MediaAudioVideoCapture ||
  //     feature == QWebEnginePage::DesktopVideoCapture ||
  //     feature == QWebEnginePage::DesktopAudioVideoCapture)
  //   m_webView->page()->setFeaturePermission(
  //       securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
  // else
  //   m_webView->page()->setFeaturePermission(
  //       securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
  m_webView->page()->setFeaturePermission(
      securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
}

MainWindow::~MainWindow() = default;
