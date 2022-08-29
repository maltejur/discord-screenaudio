# discord-screenaudio

A very WIP custom discord client that supports streaming with audio on Linux,
made possible by the
[great work](https://github.com/edisionnano/Screenshare-with-audio-on-Discord-with-Linux)
of [@edisionnano](https://github.com/edisionnano) and the
[Rohrkabel library](https://github.com/Soundux/rohrkabel) by
[@Curve](https://github.com/Curve).

![Screenshot_20220718_194357](https://user-images.githubusercontent.com/48161361/179571245-11ea05f3-fb5e-4aef-9132-2736e122ef04.png)

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
- Can only share primary screen on X11 (no other screens or specific applications)
  (see [#1](https://github.com/maltejur/discord-screenaudio/issues/1))

## Installation

You have multiple options:

- Install the Flatpak (recommended):
  `flatpak install de.shorsh.discord-screenaudio`
- If you are on Arch, you can build and install
  [`discord-screenaudio`](https://aur.archlinux.org/packages/discord-screenaudio)
  from the AUR
- If you are on openSUSE, you can use the
  [Open Build Service package](https://software.opensuse.org/download.html?project=home%3AVortexAcherontic&package=discord-screenaudio)
  by [@VortexAcherontic](https://github.com/VortexAcherontic)
- You can [build it yourself](#building-from-source)

## Building from Source

### Requirements

- Basic building tools
- CMake
- Qt5 and QtWebEngine
- **PipeWire** (it currently doesn't work with PulseAudio)
- Git
- notification-daemon (optional, for basic desktop notifications)
- Kf5Notifications (optional, for KDE users)

Optional packages will appear at the end of the line. Remove them, if you wish.

On Debian **as [root](https://en.wikipedia.org/wiki/Superuser)**:<br>
`apt update && apt install -y build-essential git cmake qtbase5-dev qtwebengine5-dev pkg-config libpipewire-0.3-dev notification-daemon libkf5notifications-dev`

On Arch **as [root](https://en.wikipedia.org/wiki/Superuser)**:<br>
`pacman -Sy --needed --noconfirm git cmake qt5-base qt5-webengine xdg-desktop-portal-impl pipewire notification-daemon knotifications`

### Building

### Automatic install

**Supported distributions**:<br>
- Debian-based
- Arch-based

Choose your URL fetcher:

- curl:
```sh
sh -c "$(curl -fsSL https://raw.githubusercontent.com/maltejur/discord-screenaudio/master/install.sh)"
```
- wget:
```sh
sh -c "$(wget -O- https://raw.githubusercontent.com/maltejur/discord-screenaudio/master/install.sh)"
```
- fetch:
```sh
sh -c "$(fetch -o - https://raw.githubusercontent.com/maltejur/discord-screenaudio/master/install.sh)"
```
**Forks**: Change the `user/repo` URL, if the repository supports automatic installation.

### Manual install

First, clone the repository:

```bash
git clone --recursive https://github.com/maltejur/discord-screenaudio.git
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

## How it works

This whole project is based on
[this](https://github.com/edisionnano/Screenshare-with-audio-on-Discord-with-Linux)
repository, which very nicely explains how to stream audio in the web version of
Discord. Basically: a virtual microphone is created which captures the
application audio, and this microphone is then fed to the Discord stream by
intercepting a API call of Discord.

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
