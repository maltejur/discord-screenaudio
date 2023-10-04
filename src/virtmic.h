// SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>
#include <QVector>
#include <iostream>

namespace Virtmic {

QVector<QString> getTargets();
void start(QString _target);

} // namespace Virtmic
