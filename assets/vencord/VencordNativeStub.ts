let webclass;

const promise = new Promise((resolve) => {
  setTimeout(() => {
    new QWebChannel(qt.webChannelTransport, function (channel) {
      webclass = channel.objects.webclass;
      resolve();
    });
  });
});

async function prepareWebclass() {
  if (!webclass) await promise;
}

window.VencordNative = {
  getVersions: () => ({}),
  ipc: {
    send: async (event: string, ...args: any[]) => {
      await prepareWebclass();
      webclass.vencordSend(event, args);
    },
    sendSync: (event: string, ...args: any[]) => {
      if (event === "VencordGetSettings") {
        console.log("stub: IPC VencordGetSettings");
        return "{}";
      } else throw new Error("Synchroneous IPC not implemented");
    },
    on(event: string, listener: () => {}) {
      // TODO quickCss
    },
    off(event: string, listener: () => {}) {
      // not used for now
    },
    invoke: async (event: string, ...args: any[]) => {
      await prepareWebclass();
      return webclass.vencordSend(event, args);
    },
  },
};
