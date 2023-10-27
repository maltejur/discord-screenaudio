// SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

Object.hasOwn = function (o, p) {
    const obj = Object(o);
    const key = String(p);
    return Object.prototype.hasOwnProperty.call(obj, key);
}

Array.prototype.at = function (index) {
    if (index >= 0) {
        return this[index];
    } else {
        return this[this.length + index];
    }
};
