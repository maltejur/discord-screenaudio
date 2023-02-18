navigator.mediaDevices.chromiumGetDisplayMedia =
  navigator.mediaDevices.getDisplayMedia;

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

const getAudioDevice = async (nameOfAudioDevice) => {
  await navigator.mediaDevices.getUserMedia({
    audio: true,
  });
  let audioDevice;
  while (audioDevice === undefined) {
    let devices = await navigator.mediaDevices.enumerateDevices();
    audioDevice = devices.find(({ label }) => label === nameOfAudioDevice);
    if (!audioDevice)
      userscript.log(
        `Did not find '${nameOfAudioDevice}', trying again in 100ms`
      );
    await sleep(100);
  }
  userscript.log(`Found '${nameOfAudioDevice}'`);
  return audioDevice;
};

function setGetDisplayMedia(video = true, overrideArgs = undefined) {
  const getDisplayMedia = async (...args) => {
    var id;
    try {
      let myDiscordAudioSink = await getAudioDevice(
        "discord-screenaudio-virtmic"
      );
      id = myDiscordAudioSink.deviceId;
    } catch (error) {
      id = "default";
    }
    let captureSystemAudioStream = await navigator.mediaDevices.getUserMedia({
      audio: {
        // We add our audio constraints here, to get a list of supported constraints use navigator.mediaDevices.getSupportedConstraints();
        // We must capture a microphone, we use default since its the only deviceId that is the same for every Chromium user
        deviceId: {
          exact: id,
        },
        // We want auto gain control, noise cancellation and noise suppression disabled so that our stream won't sound bad
        autoGainControl: false,
        echoCancellation: false,
        noiseSuppression: false,
        // By default Chromium sets channel count for audio devices to 1, we want it to be stereo in case we find a way for Discord to accept stereo screenshare too
        channelCount: 2,
        // You can set more audio constraints here, bellow are some examples
        //latency: 0,
        //sampleRate: 48000,
        //sampleSize: 16,
        //volume: 1.0
      },
    });
    let [track] = captureSystemAudioStream.getAudioTracks();
    const gdm = await navigator.mediaDevices.chromiumGetDisplayMedia(
      ...(overrideArgs
        ? [overrideArgs]
        : args || [{ video: true, audio: true }])
    );
    gdm.addTrack(track);
    if (!video) for (const track of gdm.getVideoTracks()) track.enabled = false;
    return gdm;
  };
  navigator.mediaDevices.getDisplayMedia = getDisplayMedia;
}

setGetDisplayMedia();

let userscript;
let muteBtn;
let deafenBtn;
let streamStartBtn;
let streamStartBtnInitialDisplay;
let streamStartBtnClone;
let resolutionString;
const clonedElements = [];
const hiddenElements = [];
let wasStreamActive = false;

function createButton(text, onClick) {
  const button = document.createElement("button");
  button.style.marginBottom = "20px";
  button.classList =
    "button-ejjZWC lookFilled-1H2Jvj colorBrand-2M3O3N sizeSmall-3R2P2p grow-2T4nbg";
  button.innerText = text;
  button.addEventListener("click", onClick);
  return button;
}

function createSwitch(text, enabled, onClick) {
  const container = document.createElement("div");
  container.style.marginBottom = "20px";
  container.className = "labelRow-NnoUIp";

  const label = document.createElement("label");
  label.innerText = text;
  label.className = "title-2yADjX";
  container.appendChild(label);

  const svg = document.createElement("div");
  container.appendChild(svg);

  function setSvgDisabled() {
    svg.innerHTML = `<div class="container-1QtPKm default-colors" style="opacity: 1; background-color: rgb(114, 118, 125);"><svg class="slider-HJFN2i" viewBox="0 0 28 20" preserveAspectRatio="xMinYMid meet" aria-hidden="true" style="left: -3px;"><rect fill="white" x="4" y="0" height="20" width="20" rx="10"></rect><svg viewBox="0 0 20 20" fill="none"><path fill="rgba(114, 118, 125, 1)" d="M5.13231 6.72963L6.7233 5.13864L14.855 13.2704L13.264 14.8614L5.13231 6.72963Z"></path><path fill="rgba(114, 118, 125, 1)" d="M13.2704 5.13864L14.8614 6.72963L6.72963 14.8614L5.13864 13.2704L13.2704 5.13864Z"></path></svg></svg><input id="uid_84" type="checkbox" class="input-125oad" tabindex="0"></div>`;
  }

  function setSvgEnabled() {
    svg.innerHTML = `<div class="container-1QtPKm default-colors checked-16gMAN" style="opacity: 1; background-color: rgb(59, 165, 92);"><svg class="slider-HJFN2i" viewBox="0 0 28 20" preserveAspectRatio="xMinYMid meet" aria-hidden="true" style="left: 12px;"><rect fill="white" x="4" y="0" height="20" width="20" rx="10"></rect><svg viewBox="0 0 20 20" fill="none"><path fill="rgba(59, 165, 92, 1)" d="M7.89561 14.8538L6.30462 13.2629L14.3099 5.25755L15.9009 6.84854L7.89561 14.8538Z"></path><path fill="rgba(59, 165, 92, 1)" d="M4.08643 11.0903L5.67742 9.49929L9.4485 13.2704L7.85751 14.8614L4.08643 11.0903Z"></path></svg></svg><input id="uid_74" type="checkbox" class="input-125oad" tabindex="0" checked=""></div>`;
  }

  function updateSvg() {
    if (enabled) setSvgEnabled();
    else setSvgDisabled();
  }

  container.addEventListener("click", () => {
    enabled = !enabled;
    updateSvg();
    onClick(enabled);
  });
  updateSvg();

  return container;
}

// Fix for broken discord notifications after restart
// (https://github.com/maltejur/discord-screenaudio/issues/17)
Notification.requestPermission();

setTimeout(() => {
  new QWebChannel(qt.webChannelTransport, (channel) => {
    userscript = channel.objects.userscript;
    main();
  });
});

function main() {
  userscript.muteToggled.connect(() => {
    muteBtn && muteBtn.click();
  });

  userscript.deafenToggled.connect(() => {
    deafenBtn && deafenBtn.click();
  });

  userscript.streamStarted.connect((video, width, height, frameRate) => {
    resolutionString = video ? `${height}p ${frameRate}FPS` : "Audio Only";
    setGetDisplayMedia(video, {
      audio: true,
      video: { width, height, frameRate },
    });
    streamStartBtn.click();
    streamStartBtn.style.display = streamStartBtnInitialDisplay;
    streamStartBtnClone.remove();
  });

  function updateUserstyles() {
    userscript.log("Loading userstyles...");
    userscript.loadingMessage = "Loading userstyles...";
    let stylesheet = document.getElementById("discordScreenaudioUserstyles");
    if (stylesheet) {
      userscript.log("Removing old userstyles...");
      stylesheet.remove();
    }
    stylesheet = document.createElement("style");
    stylesheet.id = "discordScreenaudioUserstyles";
    stylesheet.innerText = userscript.userstyles;
    document.head.appendChild(stylesheet);
    userscript.log("Finished loading userstyles");
    userscript.loadingMessage = "";
  }

  userscript.userstylesChanged.connect(updateUserstyles);
  setTimeout(() => updateUserstyles());

  setInterval(async () => {
    const streamActive =
      document.getElementsByClassName("panel-2ZFCRb activityPanel-9icbyU")
        .length > 0;

    if (!streamActive && wasStreamActive) userscript.stopVirtmic();
    wasStreamActive = streamActive;

    if (streamActive) {
      clonedElements.forEach((el) => {
        el.remove();
      });
      clonedElements.length = 0;

      hiddenElements.forEach((el) => {
        el.style.display = "block";
      });
      hiddenElements.length = 0;
    } else {
      for (const el of [
        document.getElementsByClassName("actionButtons-2vEOUh")?.[0]
          ?.children[1],
        document.querySelector(
          ".wrapper-3t3Yqv > div > div > div > div > .controlButton-2PMNom"
        ),
      ]) {
        if (!el) continue;
        if (el.classList.contains("discord-screenaudio-cloned")) continue;
        streamStartBtn = el;
        streamStartBtn.classList.add("discord-screenaudio-cloned");

        streamStartBtnClone = streamStartBtn.cloneNode(true);
        streamStartBtnClone.title = "Share Your Screen with Audio";
        streamStartBtnClone.addEventListener("click", () => {
          userscript.showStreamDialog();
        });

        streamStartBtnInitialDisplay = streamStartBtn.style.display;

        streamStartBtn.style.display = "none";
        streamStartBtn.parentNode.insertBefore(streamStartBtnClone, el);

        clonedElements.push(streamStartBtnClone);
        hiddenElements.push(streamStartBtn);
      }
    }

    // Add about text in settings
    if (
      document.getElementsByClassName("dirscordScreenaudioAboutText").length ==
      0
    ) {
      for (const el of document.getElementsByClassName("info-3pQQBb")) {
        let aboutEl;
        if (userscript.kxmlgui) {
          aboutEl = document.createElement("a");
          aboutEl.addEventListener("click", () => {
            userscript.showHelpMenu();
          });
        } else {
          aboutEl = document.createElement("div");
        }
        aboutEl.innerText = `discord-screenaudio ${userscript.version}`;
        aboutEl.style.fontSize = "12px";
        aboutEl.style.color = "var(--text-muted)";
        aboutEl.style.textTransform = "none";
        aboutEl.style.display = "inline-block";
        aboutEl.style.width = "100%";
        aboutEl.classList.add("dirscordScreenaudioAboutText");
        aboutEl.style.cursor = "pointer";
        el.appendChild(aboutEl);
      }
    }

    // Remove stream settings if stream is active
    document.getElementById("manage-streams-change-windows")?.remove();
    document.querySelector(`[aria-label="Stream Settings"]`)?.remove();

    // Add event listener for keybind tab
    if (
      document
        .getElementById("keybinds-tab")
        ?.getElementsByClassName(
          "container-3jbRo5 info-1hMolH browserNotice-1u-Y5o"
        ).length
    ) {
      const el = document
        .getElementById("keybinds-tab")
        .getElementsByClassName("children-1xdcWE")[0];
      const div = document.createElement("div");
      div.style.marginBottom = "50px";
      div.appendChild(
        createButton("Edit Global Keybinds", () => {
          userscript.showShortcutsDialog();
        })
      );
      el.innerHTML = "";
      el.appendChild(div);
    }

    const buttonContainer =
      document.getElementsByClassName("container-YkUktl")[0];
    if (!buttonContainer) {
      userscript.log(
        "Cannot locate Mute/Deafen/Settings button container, please report this on GitHub"
      );
    }

    muteBtn = buttonContainer
      ? buttonContainer.getElementsByClassName(
          "button-12Fmur enabled-9OeuTA button-f2h6uQ lookBlank-21BCro colorBrand-I6CyqQ grow-2sR_-F"
        )[0]
      : null;

    deafenBtn = buttonContainer
      ? buttonContainer.getElementsByClassName(
          "button-12Fmur enabled-9OeuTA button-f2h6uQ lookBlank-21BCro colorBrand-I6CyqQ grow-2sR_-F"
        )[1]
      : null;

    if (resolutionString) {
      for (const el of document.getElementsByClassName(
        "qualityIndicator-39wQDy"
      )) {
        el.innerHTML = resolutionString;
      }
    }

    const accountTab = document.getElementById("my-account-tab");
    if (accountTab) {
      const discordScreenaudioSettings = document.getElementById(
        "discord-screenaudio-settings"
      );
      if (!discordScreenaudioSettings) {
        const firstDivider = accountTab.getElementsByClassName(
          "divider-3nqZNm marginTop40-Q4o1tS"
        )[0];
        if (firstDivider) {
          const section = document.createElement("div");
          section.className = "marginTop40-Q4o1tS";
          section.id = "discord-screenaudio-settings";

          const title = document.createElement("h2");
          title.className =
            "h1-3iMExa title-lXcL8p defaultColor-3Olr-9 defaultMarginh1-1UYutH";
          title.innerText = "discord-screenaudio";
          section.appendChild(title);

          section.appendChild(
            createButton("Edit Global Keybinds", () => {
              userscript.showShortcutsDialog();
            })
          );

          section.appendChild(
            createButton("Install Theme", () => {
              userscript.showThemeDialog();
            })
          );

          section.appendChild(
            createButton("Uninstall Theme", () => {
              userscript.installUserStyles("");
            })
          );

          section.appendChild(
            createSwitch(
              "Move discord-screenaudio to the system tray instead of closing",
              await userscript.getBoolPref("trayIcon", false),
              (enabled) => {
                userscript.setTrayIcon(enabled);
              }
            )
          );

          section.appendChild(
            createSwitch(
              "Start discord-screenaudio hidden to tray",
              await userscript.getPref("startHidden", false),
              (hidden) => {
                userscript.setPref("startHidden", hidden);
              }
            )
          );

          const divider = document.createElement("div");
          divider.className = "divider-3nqZNm marginTop40-Q4o1tS";

          firstDivider.after(section);
          section.after(divider);
        }
      }
    }
  }, 500);
}
