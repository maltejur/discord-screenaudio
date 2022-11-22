// From v0.4

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
      console.log(
        `dsa: Did not find '${nameOfAudioDevice}', trying again in 100ms`
      );
    await sleep(100);
  }
  console.log(`dsa: Found '${nameOfAudioDevice}'`);
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

const clonedElements = [];
const hiddenElements = [];
let wasStreamActive = false;

setInterval(() => {
  const streamActive =
    document.getElementsByClassName("panel-2ZFCRb activityPanel-9icbyU")
      .length > 0;

  if (!streamActive && wasStreamActive)
    console.log("!discord-screenaudio-stream-stopped");
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
      document.getElementsByClassName("actionButtons-2vEOUh")?.[0]?.children[1],
      document.querySelector(
        ".wrapper-3t3Yqv > div > div > div > div > .controlButton-2PMNom"
      ),
    ]) {
      if (!el) continue;
      if (el.classList.contains("discord-screenaudio-cloned")) continue;
      el.classList.add("discord-screenaudio-cloned");
      elClone = el.cloneNode(true);
      elClone.title = "Share Your Screen with Audio";
      elClone.addEventListener("click", () => {
        console.log("!discord-screenaudio-start-stream");
      });

      const initialDisplay = el.style.display;

      window.discordScreenaudioStartStream = (
        video,
        width,
        height,
        frameRate
      ) => {
        window.discordScreenaudioResolutionString = video
          ? `${height}p ${frameRate}FPS`
          : "Audio Only";
        setGetDisplayMedia(video, {
          audio: true,
          video: { width, height, frameRate },
        });
        el.click();
        el.style.display = initialDisplay;
        elClone.remove();
      };

      el.style.display = "none";
      el.parentNode.insertBefore(elClone, el);

      clonedElements.push(elClone);
      hiddenElements.push(el);
    }
  }

  // Add about text in settings
  if (
    document.getElementsByClassName("dirscordScreenaudioAboutText").length == 0
  ) {
    for (const el of document.getElementsByClassName("info-3pQQBb")) {
      let aboutEl;
      if (window.discordScreenaudioKXMLGUI) {
        aboutEl = document.createElement("a");
        aboutEl.addEventListener("click", () => {
          console.log("!discord-screenaudio-about");
        });
      } else {
        aboutEl = document.createElement("div");
      }
      aboutEl.innerText = `discord-screenaudio ${window.discordScreenaudioVersion}`;
      aboutEl.style.fontSize = "12px";
      aboutEl.style.color = "var(--text-muted)";
      aboutEl.style.textTransform = "none";
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
        "container-3jbRo5 info-1hMolH fontSize16-3zr6Io browserNotice-1u-Y5o"
      ).length
  ) {
    const el = document
      .getElementById("keybinds-tab")
      .getElementsByClassName("children-1xdcWE")[0];
    const div = document.createElement("div");
    div.style.marginBottom = "50px";
    const button = document.createElement("button");
    button.classList =
      "button-f2h6uQ lookFilled-yCfaCM colorBrand-I6CyqQ sizeSmall-wU2dO- grow-2sR_-F";
    button.innerText = "Edit Global Keybinds";
    button.addEventListener("click", () => {
      console.log("!discord-screenaudio-keybinds");
    });
    div.appendChild(button);
    el.innerHTML = "";
    el.appendChild(div);
  }

  const buttonContainer =
    document.getElementsByClassName("container-YkUktl")[0];
  if (!buttonContainer) {
    console.log(
      "dsa: Cannot locate Mute/Deafen/Settings button container, please report this on GitHub"
    );
  }

  const muteBtn = buttonContainer
    ? buttonContainer.getElementsByClassName(
        "button-12Fmur enabled-9OeuTA button-f2h6uQ lookBlank-21BCro colorBrand-I6CyqQ grow-2sR_-F"
      )[0]
    : null;
  window.discordScreenaudioToggleMute = () => muteBtn && muteBtn.click();

  const deafenBtn = buttonContainer
    ? buttonContainer.getElementsByClassName(
        "button-12Fmur enabled-9OeuTA button-f2h6uQ lookBlank-21BCro colorBrand-I6CyqQ grow-2sR_-F"
      )[1]
    : null;

  window.discordScreenaudioToggleDeafen = () => deafenBtn && deafenBtn.click();

  if (window.discordScreenaudioResolutionString) {
    for (const el of document.getElementsByClassName(
      "qualityIndicator-39wQDy"
    )) {
      el.innerHTML = window.discordScreenaudioResolutionString;
    }
  }
}, 500);

// Fix for broken discord notifications after restart
// (https://github.com/maltejur/discord-screenaudio/issues/17)
Notification.requestPermission();
