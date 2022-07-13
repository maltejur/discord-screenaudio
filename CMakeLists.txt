cmake_minimum_required(VERSION 3.11)
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

set(${CMAKE_PROJECT_NAME}_SRC
  src/main.cpp
  src/mainwindow.cpp
  src/virtmic.cpp
  src/discordpage.cpp
  src/streamdialog.cpp
  resources.qrc
)

include(FetchContent)

FetchContent_Declare(rohrkabel GIT_REPOSITORY "https://github.com/Soundux/rohrkabel")
FetchContent_MakeAvailable(rohrkabel)

add_executable(${CMAKE_PROJECT_NAME} ${${CMAKE_PROJECT_NAME}_SRC})

target_link_libraries(${CMAKE_PROJECT_NAME} Qt5::Widgets Qt5::WebEngineWidgets rohrkabel)

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
install(PROGRAMS assets/${CMAKE_PROJECT_NAME}.desktop DESTINATION share/applications)

add_custom_target(appimage ${CMAKE_SOURCE_DIR}/scripts/build-appimage.sh WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
