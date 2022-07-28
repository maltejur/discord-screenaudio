// From v0.4

navigator.mediaDevices.chromiumGetDisplayMedia =
  navigator.mediaDevices.getDisplayMedia;

const getAudioDevice = async (nameOfAudioDevice) => {
  await navigator.mediaDevices.getUserMedia({
    audio: true,
  });
  let devices = await navigator.mediaDevices.enumerateDevices();
  let audioDevice = devices.find(({ label }) => label === nameOfAudioDevice);
  return audioDevice;
};

function setGetDisplayMedia(overrideArgs = undefined) {
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

      window.discordScreenaudioStartStream = (width, height, frameRate) => {
        setGetDisplayMedia({
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
      const aboutEl = document.createElement("div");
      aboutEl.innerText = "discord-screenaudio v1.0.0-rc.9";
      aboutEl.style.fontSize = "12px";
      aboutEl.style.color = "var(--text-muted)";
      aboutEl.classList.add("dirscordScreenaudioAboutText");
      el.appendChild(aboutEl);
    }
  }

  // Remove stream settings if stream is active
  document.getElementById("manage-streams-change-windows")?.remove();
  document.querySelector(`[aria-label="Stream Settings"]`)?.remove();
}, 500);

// Fix for broken discord notifications after restart
// (https://github.com/maltejur/discord-screenaudio/issues/17)
Notification.requestPermission();
