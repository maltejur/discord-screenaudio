#!/bin/sh

# Export variables
export CMAKE_GENERATOR="Ninja"

# Environment variables
CURRENT_USER=$(whoami)
REPOSITORY_NAME="discord-screenaudio"

# Colours
RED="\e[1;31m"
GREEN="\e[1;32m"
YELLOW="\e[1;33m"
BLUE="\e[1;34m"
PURPLE="\e[1;35m"
CYAN="\e[1;36m"
RESET_COLOUR="\e[0m"

# Set repository directory based on user or root
if [ ${CURRENT_USER} == "root" ]; then
	REPOSITORY_DIRECTORY="/root/.local/share/${REPOSITORY_NAME}/git"
else
	REPOSITORY_DIRECTORY="/home/${CURRENT_USER}/.local/share/${REPOSITORY_NAME}/git"
fi
mkdir -pv ${REPOSITORY_DIRECTORY}

# Check if the repository already exists
if ! [ -d ../${REPOSITORY_NAME} ]; then
	if ! [ -d ${REPOSITORY_DIRECTORY}/src ]; then
		git clone --recursive https://github.com/maltejur/${REPOSITORY_NAME}.git "${REPOSITORY_DIRECTORY}"
	fi
else
	printf "${YELLOW}""[!] Repository already exists.""${RESET_COLOUR}""\n"
	REPOSITORY_DIRECTORY=$(pwd)
fi
cd ${REPOSITORY_DIRECTORY}

# Check the package manager.
if type dpkg &>/dev/null; then
	package_manager="apt"
elif type pacman &>/dev/null; then
	package_manager="pacman"
fi

# Prepare the packages.
DE_PACKAGES="notification-daemon"
if [ "$1" == "--basic-notifications" ]; then
	case $XDG_CURRENT_DESKTOP in
		"LXQt")
			DE_PACKAGES="lxqt-notificationd"
			;;
		"MATE")
			DE_PACKAGES="mate-notification-daemon"
			;;
		"XFCE")
			DE_PACKAGES="xfce4-notifyd"
			;;
	esac
else
	case $package_manager in
		"apt")
			DE_PACKAGES="libkf5notifications-dev libkf5xmlgui-dev libkf5globalaccel-dev"
			;;
		"pacman")
			DE_PACKAGES="knotifications kxmlgui kglobalaccel"
			;;
	esac
fi

# Install the packages.
case $package_manager in
	"apt")
		sudo apt-get install -y build-essential git cmake qtbase5-dev qtwebengine5-dev pkg-config libpipewire-0.3-dev ${DE_PACKAGES}
		;;
	"pacman")
		sudo pacman -S --needed --noconfirm git cmake qt5-base qt5-webengine xdg-desktop-portal pipewire ${DE_PACKAGES}
		;;
esac

# Installation process.
cmake -B build
cmake --build build --config Release
sudo cmake --install build
rm -rfv ${REPOSITORY_DIRECTORY}/build/
printf "${GREEN}""${REPOSITORY_NAME} has been installed!""${RESET_COLOUR}""\n"
notify-send --icon="${REPOSITORY_DIRECTORY}/assets/de.shorsh.discord-screenaudio.png" --app-name="${REPOSITORY_NAME}" "${REPOSITORY_NAME}" "The application has been installed!"
