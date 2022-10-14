#include "webclass.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QUrl>

QVariant WebClass::vencordSend(QString event, QVariantList args) {
  if (event == "VencordGetRepo") {
    return true;
  }
  if (event == "VencordGetSettingsDir") {
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  }
  if (event == "VencordGetQuickCss") {
    QString filename =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) +
        "/vencord/quickCss.css";
    if (QFile::exists(filename)) {
      QFile file(filename);
      file.open(QIODevice::ReadOnly);
      auto content = file.readAll();
      file.close();
      return QString(content);
    } else
      return "";
  }
  if (event == "VencordGetSettings") {
    return m_vencordSettings;
  }
  if (event == "VencordSetSettings") {
    m_vencordSettings = args[0].toString();
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
