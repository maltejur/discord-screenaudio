#!/bin/sh

# Environment variables
CURRENT_USER=$(whoami)
CURRENT_HOME=$HOME
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
case $XDG_CURRENT_DESKTOP in
	"KDE")
		case $package_manager in
			"apt")
				de_packages="libkf5notifications-dev"
				;;
			"pacman")
				de_packages="knotifications"
				;;
		esac
		;;
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
case $package_manager in
	"apt")
		packages="build-essential git cmake qtbase5-dev qtwebengine5-dev pkg-config libpipewire-0.3-dev $de_packages"
		;;
	"pacman")
		packages="git cmake qt5-base qt5-webengine xdg-desktop-portal-impl pipewire $de_packages"
		;;
esac

# Install the packages.
for package in $packages; do
	case $package_manager in
		"apt")
			if [ -z "$(dpkg --list | grep "$package")" ]; then
				sudo apt-get install -y $package
				printf "$GREEN""$PURPLE""$package""$GREEN"" has been installed!""$RESET_COLOUR""\n"
			else
				printf "$PURPLE""$package""$RESET_COLOUR"" has already been satisfied!\n"
			fi
			;;
		"pacman")
			if [ -z "$(pacman -Q | grep "$package")" ]; then
				sudo pacman -S --noconfirm $package
				printf "$GREEN""$PURPLE""$package""$GREEN"" has been installed!""$RESET_COLOUR""\n"
			else
				printf "$PURPLE""$package""$RESET_COLOUR"" has already been satisfied!\n"
			fi
			;;
	esac
done

# Installation process.
if [[ $XDG_CURRENT_DESKTOP == "KDE" ]]; then
	cmake -B build
else
	cmake -DKF5NOTIFICATIONS=OFF -B build
fi
cmake --build build --config Release
sudo cmake --install build
rm -rfv ./build/
printf "$GREEN""discord-screenaudio has been installed!""$RESET_COLOUR""\n"
notify-send "discord-screenaudio has been installed!"