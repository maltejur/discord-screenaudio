// SPDX-FileCopyrightText: 2022 Vendicated and contributors
// SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

async function prepareUserscript() {
  if (!userscript) await userscriptPromise();
}

/// <reference path="../src/modules.d.ts" />
/// <reference path="../src/globals.d.ts" />

import * as DataStore from "../src/api/DataStore";
import { debounce } from "../src/utils";
import { getThemeInfo } from "../src/main/themes";

// Discord deletes this so need to store in variable
const { localStorage } = window;

// listeners for ipc.on
const cssListeners = new Set<(css: string) => void>();
const NOOP = () => {};
const NOOP_ASYNC = async () => {};

const themeStore = DataStore.createStore("VencordThemes", "VencordThemeData");

// probably should make this less cursed at some point
window.VencordNative = {
  themes: {
    uploadTheme: (fileName: string, fileData: string) =>
      DataStore.set(fileName, fileData, themeStore),
    deleteTheme: (fileName: string) => DataStore.del(fileName, themeStore),
    getThemesDir: async () => "",
    getThemesList: () =>
      DataStore.entries(themeStore).then((entries) =>
        entries.map(([name, css]) => getThemeInfo(css, name.toString()))
      ),
    getThemeData: (fileName: string) => DataStore.get(fileName, themeStore),
    getSystemValues: async () => ({}),
  },

  native: {
    getVersions: () => ({}),
    openExternal: async (url) => {
      await prepareUserscript();
      userscript.openURL(url);
    },
  },

  updater: {
    getRepo: async () => ({
      ok: true,
      value: "https://github.com/Vendicated/Vencord",
    }),
    getUpdates: async () => ({ ok: true, value: [] }),
    update: async () => ({ ok: true, value: false }),
    rebuild: async () => ({ ok: true, value: true }),
  },

  quickCss: {
    get: () => {
      return userscript.getConfigFile("vencord/quickCss.css");
    },
    set: async (s: string) => {
      await prepareUserscript();
      userscript.setConfigFile("vencord/quickCss.css", s);
    },
    addChangeListener(cb) {
      cssListeners.add(cb);
    },
    addThemeChangeListener: NOOP,
    openFile: NOOP_ASYNC,
    async openEditor() {
      userscript.editConfigFile("vencord/quickCss.css");
    },
  },

  settings: {
    get: () => localStorage.getItem("VencordSettings") || "{}",
    set: async (s: string) => localStorage.setItem("VencordSettings", s),
    getSettingsDir: async () => "LocalStorage",
  },

  pluginHelpers: {} as any,
};
