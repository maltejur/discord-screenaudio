#pragma once

#include <QObject>
#include <QVariant>

class WebClass : public QObject {
  Q_OBJECT
public slots:
  QVariant vencordSend(QString event, QVariantList args);

private:
  QString m_vencordSettings;
};
