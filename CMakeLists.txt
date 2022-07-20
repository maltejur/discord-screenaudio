cmake_minimum_required(VERSION 3.0)
project(discord-screenaudio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
# set(CMAKE_AUTOUIC ON)

find_package(Qt5 CONFIG REQUIRED COMPONENTS 
  Widgets
  WebEngineWidgets
)

set(discord-screenaudio_SRC
  src/main.cpp
  src/mainwindow.cpp
  src/virtmic.cpp
  src/discordpage.cpp
  src/streamdialog.cpp
  resources.qrc
)

include(FetchContent)

FetchContent_Declare(
  rohrkabel 
  GIT_REPOSITORY "https://github.com/Soundux/rohrkabel" 
  GIT_TAG "d87403f48d3a95aa4bcf4cd60112d9e4bb090d5d"
)
FetchContent_MakeAvailable(rohrkabel)

add_executable(discord-screenaudio ${discord-screenaudio_SRC})

target_link_libraries(discord-screenaudio Qt5::Widgets Qt5::WebEngineWidgets rohrkabel)

install(TARGETS discord-screenaudio DESTINATION bin)
install(PROGRAMS assets/discord-screenaudio.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
