#pragma once

#include <QString>
#include <QVector>
#include <iostream>
#include <rohrkabel/registry/registry.hpp>

namespace Virtmic {

QVector<QString> getTargets();
void start(QString _target);

} // namespace Virtmic
