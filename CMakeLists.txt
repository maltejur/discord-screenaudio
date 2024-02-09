# SPDX-FileCopyrightText: 2022 Malte Jürgens and contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.5)
project(discord-screenaudio)

include(version.cmake)
determine_version("${CMAKE_CURRENT_SOURCE_DIR}" DISCORD_SCEENAUDIO)
add_definitions( -DDISCORD_SCEENAUDIO_VERSION_FULL="${DISCORD_SCEENAUDIO_VERSION_FULL}" )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
string(TIMESTAMP TIMESTAMP %s)
# set(CMAKE_AUTOUIC ON)

option(PREFER_QT6 "Prefer Qt6 over Qt5" ON)
option(SKIP_KDE "Do not include features requiring KDE Frameworks (notifications, global shortcuts)" OFF)

if(NOT PREFER_QT6)
  find_package(Qt5 COMPONENTS Widgets QUIET)
endif()
if (Qt5_FOUND)
  find_package(Qt5 CONFIG REQUIRED COMPONENTS Widgets WebEngineWidgets)
  message(STATUS "Using Qt5")

  if(NOT SKIP_KDE)
    find_package(KF5Notifications QUIET)
    if(KF5Notifications_FOUND)
      add_definitions( -DKNOTIFICATIONS )
    else()
      message(WARNING "KF5Notifications not found, notifications will not work")
    endif()

    find_package(KF5XmlGui QUIET)
    if(KF5XmlGui_FOUND)
      add_definitions( -DKXMLGUI )
    else()
      message(WARNING "KF5XmlGui not found, some UI elements and global shortcuts will not work")
    endif()

    find_package(KF5GlobalAccel QUIET)
    if(KF5GlobalAccel_FOUND)
      add_definitions( -DKGLOBALACCEL )
    else()
      message(WARNING "KF5GlobalAccel not found, global shortcuts will not work")
    endif()
  endif()
else()
  find_package(Qt6 CONFIG REQUIRED COMPONENTS Widgets WebEngineWidgets)
  message(STATUS "Using Qt6")
  add_definitions( -DQT6 )

  if(NOT SKIP_KDE)
    find_package(KF6Notifications QUIET)
    if(KF6Notifications_FOUND)
      add_definitions( -DKNOTIFICATIONS )
    else()
      message(WARNING "KF6Notifications not found, notifications will not work")
    endif()

    find_package(KF6XmlGui QUIET)
    find_package(KF6CoreAddons QUIET)
    if(KF6XmlGui_FOUND AND KF6CoreAddons_FOUND)
      add_definitions( -DKXMLGUI )
    else()
      message(WARNING "KF6XmlGui not found, some UI elements and global shortcuts will not work")
    endif()

    find_package(KF6GlobalAccel QUIET)
    if(KF6GlobalAccel_FOUND)
      add_definitions( -DKGLOBALACCEL )
    else()
      message(WARNING "KF6GlobalAccel not found, global shortcuts will not work")
    endif()
  endif()
endif()

set(discord-screenaudio_SRC
  src/main.cpp
  src/mainwindow.cpp
  src/virtmic.cpp
  src/discordpage.cpp
  src/streamdialog.cpp
  src/log.cpp
  src/userscript.cpp
  src/centralwidget.cpp
  src/localserver.cpp
  resources.qrc
)

# Adapted from https://cliutils.gitlab.io/modern-cmake/chapters/projects/submodule.html
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
  option(GIT_SUBMODULE "Check submodules during build" ON)
  if(GIT_SUBMODULE)
    message(STATUS "Updating submodules")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --checkout
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
      message(FATAL_ERROR "`git submodule update --init --recursive --checkout` failed with ${GIT_SUBMOD_RESULT}, please provide the submodules manually")
    endif()
  endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/submodules/rohrkabel/CMakeLists.txt")
  message(FATAL_ERROR "Rohrkabel was not found since you are not in a Git checkout or have GIT_SUBMODULE disabled. Please provide rohrkabel manually to `./submodules/rohrkabel`.")
endif()

add_subdirectory(submodules/rohrkabel EXCLUDE_FROM_ALL)

add_executable(discord-screenaudio ${discord-screenaudio_SRC})

target_link_libraries(discord-screenaudio Qt::Widgets Qt::WebEngineWidgets rohrkabel)

if(KF5Notifications_FOUND)
  target_link_libraries(discord-screenaudio KF5::Notifications)
  install(FILES assets/discord-screenaudio.notifyrc DESTINATION ${CMAKE_INSTALL_PREFIX}/share/knotifications5)
endif()
if(KF5XmlGui_FOUND)
  target_link_libraries(discord-screenaudio KF5::XmlGui)
endif()
if(KF5GlobalAccel_FOUND)
  target_link_libraries(discord-screenaudio KF5::GlobalAccel)
endif()

if(KF6Notifications_FOUND)
  target_link_libraries(discord-screenaudio KF6::Notifications)
  install(FILES assets/discord-screenaudio.notifyrc DESTINATION ${CMAKE_INSTALL_PREFIX}/share/knotifications6)
endif()
if(KF6XmlGui_FOUND)
  target_link_libraries(discord-screenaudio KF6::XmlGui KF6::CoreAddons)
endif()
if(KF6GlobalAccel_FOUND)
  target_link_libraries(discord-screenaudio KF6::GlobalAccel)
endif()

install(TARGETS discord-screenaudio DESTINATION bin)
install(FILES assets/de.shorsh.discord-screenaudio.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/256x256/apps)
install(PROGRAMS assets/de.shorsh.discord-screenaudio.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
configure_file(assets/de.shorsh.discord-screenaudio.metainfo.xml.in de.shorsh.discord-screenaudio.metainfo.xml)
install(FILES ${CMAKE_BINARY_DIR}/de.shorsh.discord-screenaudio.metainfo.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/metainfo)
