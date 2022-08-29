cmake_minimum_required(VERSION 3.0)
project(discord-screenaudio)

include(version.cmake)
determine_version("${CMAKE_CURRENT_SOURCE_DIR}" DISCORD_SCEENAUDIO)
add_definitions( -DDISCORD_SCEENAUDIO_VERSION_FULL="${DISCORD_SCEENAUDIO_VERSION_FULL}" )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
string(TIMESTAMP TIMESTAMP %s)
# set(CMAKE_AUTOUIC ON)

find_package(Qt5 CONFIG REQUIRED COMPONENTS 
  Widgets
  WebEngineWidgets
)

find_package(KF5Notifications)
if(KF5Notifications_FOUND)
  add_definitions( -DKNOTIFICATIONS )
endif()

set(discord-screenaudio_SRC
  src/main.cpp
  src/mainwindow.cpp
  src/virtmic.cpp
  src/discordpage.cpp
  src/streamdialog.cpp
  src/log.cpp
  src/c_tools.c
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

add_subdirectory(submodules/rohrkabel)

add_executable(discord-screenaudio ${discord-screenaudio_SRC})

target_link_libraries(discord-screenaudio Qt5::Widgets Qt5::WebEngineWidgets rohrkabel)

if(KF5Notifications_FOUND)
  target_link_libraries(discord-screenaudio KF5::Notifications)
  install(FILES assets/discord-screenaudio.notifyrc DESTINATION ${CMAKE_INSTALL_PREFIX}/share/knotifications5)
endif()

install(TARGETS discord-screenaudio DESTINATION bin)
install(FILES assets/de.shorsh.discord-screenaudio.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/256x256/apps)
install(PROGRAMS assets/de.shorsh.discord-screenaudio.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
configure_file(assets/de.shorsh.discord-screenaudio.metainfo.xml.in de.shorsh.discord-screenaudio.metainfo.xml)
install(FILES ${CMAKE_BINARY_DIR}/de.shorsh.discord-screenaudio.metainfo.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/metainfo)
