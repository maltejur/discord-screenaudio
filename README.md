# discord-screenaudio

A custom discord client that supports streaming with audio on Linux, made
possible by the
[great work](https://github.com/edisionnano/Screenshare-with-audio-on-Discord-with-Linux)
of [@edisionnano](https://github.com/edisionnano) and the
[Rohrkabel library](https://github.com/Soundux/rohrkabel) by
[@Curve](https://github.com/Curve).

Unlike a lot of other solutions, the audio here is directly fed into the
screenshare and not passed to the user microphone
([see explanation](#how-does-this-work)).

![Screenshot_20221211_185028](https://user-images.githubusercontent.com/48161361/206920213-58a8091a-d8f9-4bb7-ae3d-3f8581b84d24.png)

The purpose of this project is **not** to provide an alternative to the original
Discord client. Rather, it should be used in addition to the original client in
case you want to stream something, maybe used with a second account. For
anything else, this client has way too many things that work less well than in
the original client.

Technically this could be against
[Discord's TOS](https://discord.com/terms#software-in-discord%E2%80%99s-services),
so be warned. Discord probably won't ban you for using this, but if they do I
told you and it's not my fault.

## Known Issues

- Only works with **PipeWire**
- Can only share primary screen on X11 (no other screens or specific
  applications) (see
  [#1](https://github.com/maltejur/discord-screenaudio/issues/1))

## Installation

You have multiple options:

- Install the Flatpak (recommended):
  `flatpak install de.shorsh.discord-screenaudio`
- If you are on Arch, you can build and install
  [`discord-screenaudio`](https://aur.archlinux.org/packages/discord-screenaudio)
  from the AUR
- If you are on openSUSE, you can use the
  [Open Build Service package](https://software.opensuse.org//download.html?project=games%3Atools&package=discord-screenaudio)
  by [@VortexAcherontic](https://github.com/VortexAcherontic)
- You can [build it yourself](#building-from-source)

## Building from Source

### Requirements

- Basic building tools
- An up-to-date system (I can't guarantee that it works on Debian or Ubuntu
  20/21)
- CMake
- Qt5 and QtWebEngine
- **PipeWire** (it currently doesn't work with PulseAudio)
- Git
- _Kf5Notifications (optional, for better notifications)_
- _KXMLGui and KGlobalAccel (optional, for keybinds)_

With apt:
`apt install -y build-essential cmake qtbase5-dev qtwebengine5-dev libkf5notifications-dev libkf5xmlgui-dev libkf5globalaccel-dev pkg-config libpipewire-0.3-dev git`

### Building

First, clone the repository:

```bash
git clone https://github.com/maltejur/discord-screenaudio.git
cd discord-screenaudio
```

Then, to build the program, run this in the source directory:

```bash
cmake -B build
cmake --build build --config Release
```

And then to optionally install it, run:

```bash
sudo cmake --install build
```

## FAQ

### How does this work?

This whole project is based on
[this](https://github.com/edisionnano/Screenshare-with-audio-on-Discord-with-Linux)
repository, which very nicely explains how to stream audio in the web version of
Discord. Basically: a virtual microphone is created which captures the
application audio, and this microphone is then fed to the Discord stream by
intercepting a API call of Discord.

### Drag and drop doesn't work in the Flatpak

This is due to sandboxing limitations of Flatpak. The main Discord Flatpak has
the same problem. If you still want to use drag and drop, you can disable most
of Flatpak's sandboxing by installing
[Flatseal](https://flathub.org/apps/details/com.github.tchx84.Flatseal) and
allowing access to "All system files" under the "Filesystem" section.

### Is there any way to add custom CSS / a theme?

Yes, you can add all your styles into
`~/.config/discord-screenaudio/userstyles.css` (or
`~/.var/app/de.shorsh.discord-screenaudio/config/discord-screenaudio/userstyles.css`
if you are using the Flatpak). But please note that due to QtWebEngine
limitations concerning content security policies, you can't use any external
files (like `@import` or `url()`).

## License

Copyright (C) 2022 Malte Jürgens

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
