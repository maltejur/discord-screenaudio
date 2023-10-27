// SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

const hasOwn = function hasOwn(o, p) {
    const obj = Object(o);
    const key = String(p);
    return Object.prototype.hasOwnProperty.call(obj, key);
}

Object.hasOwn = hasOwn;