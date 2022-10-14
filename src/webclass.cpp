#include "webclass.h"

#include <QDesktopServices>
#include <QUrl>

QVariant WebClass::vencordSend(QString event, QVariantList args) {
  if (event == "VencordGetRepo") {
    return true;
  }
  if (event == "VencordGetSettingsDir") {
    return "~/.config/discord-screenaudio/vencord";
  }
  if (event == "VencordGetQuickCss") {
    // TODO
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
