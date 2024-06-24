cmake_minimum_required(VERSION 3.16)
project(CheckersGame)

set(CMAKE_CXX_STANDARD 20)

# Find the Qt6 package
set(CMAKE_PREFIX_PATH "/Users/mateuszwojtaszek/Qt/6.7.1/macos/lib/cmake")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Add the Qt6 modules
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${Qt6Widgets_INCLUDE_DIRS})

add_executable(CheckersGame
    main.cpp
    mainwindow.h
    mainwindow.cpp
    checkersgame.h
    checkersgame.cpp
        evalbar.cpp
)

target_link_libraries(CheckersGame Qt6::Widgets)
