// SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "centralwidget.h"
#include "discordpage.h"
#include "mainwindow.h"

#include <QWebEngineNotification>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>

CentralWidget::CentralWidget(QWidget *parent) : QWidget(parent) {
  setStyleSheet("background-color:#313338;");
  m_layout = new QVBoxLayout(this);
  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->setSpacing(0);
  setupWebView();
}

void CentralWidget::setupWebView() {
  auto page = new DiscordPage(this);

  m_webView = new QWebEngineView(this);
  m_webView->setPage(page);

  bool useNotifySend = MainWindow::instance()
                           ->settings()
                           ->value("useNotifySend", false)
                           .toBool();
  if (m_useKF5Notifications || useNotifySend)
    QWebEngineProfile::defaultProfile()->setNotificationPresenter(
        [&](std::unique_ptr<QWebEngineNotification> notificationInfo) {
          if (useNotifySend) {
            auto title = notificationInfo->title();
            auto message = notificationInfo->message();
            auto image_path =
                QString("/tmp/discord-screenaudio-%1.png").arg(title);
            notificationInfo->icon().save(image_path);
            QProcess::execute("notify-send",
                              {"--icon", image_path, "--app-name",
                               "discord-screenaudio", title, message});
          } else if (m_useKF5Notifications) {
#ifdef KNOTIFICATIONS
            KNotification *notification =
                new KNotification("discordNotification");
            notification->setTitle(notificationInfo->title());
            notification->setText(notificationInfo->message());
            notification->setPixmap(
                QPixmap::fromImage(notificationInfo->icon()));
#ifdef QT5
            notification->setDefaultAction("View");
            connect(notification, &KNotification::defaultActivated,
                    [&, notificationInfo = std::move(notificationInfo)]() {
                      notificationInfo->click();
                      show();
                      activateWindow();
                    });
            notification->sendEvent();
#else
            auto action = notification->addDefaultAction("View");
            connect(action, &KNotificationAction::activated,
                    [&, notificationInfo = std::move(notificationInfo)]() {
                      notificationInfo->click();
                      show();
                      activateWindow();
                    });
            notification->sendEvent();
#endif
#endif
          }
        });

  connect(page->userScript(), &UserScript::loadingMessageChanged, this,
          &CentralWidget::setLoadingIndicator);

  m_layout->addWidget(m_webView);
}

void CentralWidget::setLoadingIndicator(QString text) {
  if (text != "") {
    if (m_loadingLabel == nullptr) {
      m_loadingLabel = new QLabel(this);
      m_loadingLabel->setMaximumHeight(20);
      m_loadingLabel->setAlignment(Qt::AlignHCenter);
      m_loadingLabel->setStyleSheet("color:#dedede;");
      m_layout->addWidget(m_loadingLabel);
    }
    m_loadingLabel->setText(text.mid(0, 100));
  } else {
    if (m_loadingLabel != nullptr) {
      m_layout->removeWidget(m_loadingLabel);
      m_loadingLabel->deleteLater();
      m_loadingLabel = nullptr;
    }
  }
}
