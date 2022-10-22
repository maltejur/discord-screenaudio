#include "webclass.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QUrl>

QVariant WebClass::vencordSend(QString event, QVariantList args) {
  QString configFolder =
      QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) +
      "/vencord";
  QString quickCssFile = configFolder + "/quickCss.css";
  QString settingsFile = configFolder + "/settings.json";

  if (!QDir().exists(configFolder))
    QDir().mkpath(configFolder);

  if (event == "VencordGetRepo") {
    return true;
  }
  if (event == "VencordGetSettingsDir") {
    return configFolder;
  }
  if (event == "VencordGetQuickCss") {
    if (QFile::exists(quickCssFile)) {
      QFile file(quickCssFile);
      if (!file.open(QIODevice::WriteOnly))
        qFatal("Failed to load %s with error: %s",
               quickCssFile.toLatin1().constData(),
               file.errorString().toLatin1().constData());
      auto content = file.readAll();
      file.close();
      return QString(content);
    } else
      return "";
  }
  if (event == "VencordGetSettings") {
    if (QFile::exists(settingsFile)) {
      QFile file(settingsFile);
      if (!file.open(QIODevice::ReadOnly))
        qFatal("Failed to load %s with error: %s",
               settingsFile.toLatin1().constData(),
               file.errorString().toLatin1().constData());
      auto content = file.readAll();
      file.close();
      return QString(content);
    } else
      return "{}";
  }
  if (event == "VencordSetSettings") {
    QFile file(settingsFile);
    if (!file.open(QIODevice::WriteOnly))
      qFatal("Failed to load %s with error: %s",
             settingsFile.toLatin1().constData(),
             file.errorString().toLatin1().constData());
    file.write(args[0].toString().toUtf8());
    file.close();
    return true;
  }
  if (event == "VencordGetUpdates") {
    return QVariantMap{{"ok", true}, {"value", QVariantList()}};
  }
  if (event == "VencordOpenExternal") {
    QDesktopServices::openUrl(QUrl(args[0].toString()));
    return true;
  }
  if (event == "VencordOpenQuickCss") {
    return true;
  }
  assert(false);
}
