#!/bin/sh

# Export variables
export CMAKE_GENERATOR="Ninja"

# Environment variables
CURRENT_USER=$(whoami)
CURRENT_PWD=$(pwd)

# Colours
RED="\e[1;31m"
GREEN="\e[1;32m"
YELLOW="\e[1;33m"
BLUE="\e[1;34m"
PURPLE="\e[1;35m"
CYAN="\e[1;36m"
RESET_COLOUR="\e[0m"

# Set home based on user or root
if [ ${CURRENT_USER} == "root" ]; then
	CURRENT_HOME="/root"
else
	CURRENT_HOME="/home/${CURRENT_USER}"
fi

# Check if the repository already exists
if ! [ -d ../discord-screenaudio ]; then
	if ! [ -d ${CURRENT_HOME}/rtl8188eus ]; then
		git clone --recursive https://github.com/maltejur/discord-screenaudio.git
	else
		printf "${YELLOW}""[!] Repository already exists.""${RESET_COLOUR}""\n"
	fi
	cd "${CURRENT_HOME}/discord-screenaudio"
fi

# Check the package manager.
if type dpkg &>/dev/null; then
	package_manager="apt"
elif type pacman &>/dev/null; then
	package_manager="pacman"
fi

# Prepare the packages.
de_packages="notification-daemon"
if [ $1 == "--basic-notifications" ]; then
	case $XDG_CURRENT_DESKTOP in
		"LXQt")
			de_packages="lxqt-notificationd"
			;;
		"MATE")
			de_packages="mate-notification-daemon"
			;;
		"XFCE")
			de_packages="xfce4-notifyd"
			;;
	esac
else
	case $package_manager in
		"apt")
			de_packages="libkf5notifications-dev"
			;;
		"pacman")
			de_packages="knotifications"
			;;
	esac
fi

# Install the packages.
case $package_manager in
	"apt")
		sudo apt-get install -y build-essential git cmake qtbase5-dev qtwebengine5-dev pkg-config libpipewire-0.3-dev $de_packages
		;;
	"pacman")
		sudo pacman -S --noconfirm git cmake qt5-base qt5-webengine xdg-desktop-portal pipewire $de_packages
		;;
esac

# Installation process.
cmake -B build
cmake --build build --config Release
sudo cmake --install build
rm -rfv ./build/
printf "$GREEN""discord-screenaudio has been installed!""$RESET_COLOUR""\n"
notify-send --icon="$CURRENT_PWD/assets/de.shorsh.discord-screenaudio.png" --app-name="discord-screenaudio" "discord-screenaudio" "The application has been installed!"
